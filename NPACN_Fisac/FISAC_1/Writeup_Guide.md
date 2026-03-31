# FISAC-1 Assignment — IoT Smart Home Controller


---

# MEMBER 1 — Architecture, WebSocket Server & Concurrency
---

## Q1 — Communication Architecture, TCP Socket Lifecycle & Concurrency Model

### 1.1 System Overview

The Smart Home Controller connects three types of participants over a single TCP-based
WebSocket server:

```
  [Web Browser]  ──── WebSocket (ws://) ────┐
                                             │
  [IoT Device 1] ──── WebSocket (ws://) ────┤──► [C WebSocket Server :8080]
                                             │          │
  [IoT Device 2] ──── WebSocket (ws://) ────┘          │
                                                    [SQLite DB]
                                                    [Shared Memory]
```

- **Web Clients** send login, `get_devices`, `command`, and `get_log` messages.
- **IoT Device Clients** send `register`, `heartbeat`, and `state_update` messages.
- The server routes commands from web clients to the correct device child process
  using POSIX shared memory + named semaphores + SIGUSR1 signals.

---

### 1.2 TCP Socket Lifecycle

Every connection follows the standard TCP state machine:

```
SERVER                              CLIENT
  │                                   │
  │◄──── SYN ─────────────────────────│  (client initiates)
  │──── SYN-ACK ──────────────────────►│
  │◄──── ACK ─────────────────────────│  (ESTABLISHED)
  │                                   │
  │◄──── HTTP GET (Upgrade) ──────────│  (WebSocket handshake request)
  │──── HTTP 101 Switching Protocols ─►│  (handshake complete)
  │                                   │
  │◄═══ WebSocket Frames ════════════►│  (data exchange)
  │                                   │
  │◄──── FIN ─────────────────────────│  (client closes)
  │──── FIN-ACK ──────────────────────►│
  │──── FIN ──────────────────────────►│
  │◄──── ACK ─────────────────────────│  (TIME_WAIT on server side)
```

**Key TCP socket options applied in our server:**

| Option | Applied On | Purpose |
|--------|-----------|---------|
| `SO_REUSEADDR` | Server socket | Allows rebinding port 8080 even if it is in TIME_WAIT state after a restart |
| `TCP_NODELAY` | Each accepted client socket | Disables Nagle's algorithm — commands are sent immediately without buffering, reducing latency |
| `SO_KEEPALIVE` | Each accepted client socket | OS sends keepalive probes to detect dead connections and clean up CLOSE_WAIT states |

**Code reference (`src/main.c`):**
```c
int opt = 1;
setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

// On each accepted client_fd:
int flag = 1;
setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY,  &flag, sizeof(flag));
setsockopt(client_fd, SOL_SOCKET,  SO_KEEPALIVE, &flag, sizeof(flag));
```

> **[INSERT SCREENSHOT: netstat output showing ESTABLISHED and TIME_WAIT states]**

---

### 1.3 WebSocket Handshake Mechanism (RFC 6455)

WebSocket begins as an HTTP/1.1 upgrade request. Our server implements the full
RFC 6455 handshake in `src/websocket.c`:

**Step 1 — Client sends HTTP Upgrade request:**
```
GET / HTTP/1.1
Host: localhost:8080
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==
Sec-WebSocket-Version: 13
```

**Step 2 — Server computes the accept key:**
```
accept_key = Base64( SHA-1( client_key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11" ) )
```

**Step 3 — Server replies with HTTP 101:**
```
HTTP/1.1 101 Switching Protocols
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Accept: <computed_key>
```

After this exchange, the TCP connection is "upgraded" — both sides use the WebSocket
framing protocol instead of HTTP.

**Code reference (`src/websocket.c` — `ws_handshake()`):**
```c
SHA1((unsigned char *)combined, strlen(combined), sha1_res);
char *accept_key = base64_encode(sha1_res, 20);
snprintf(response, sizeof(response),
    "HTTP/1.1 101 Switching Protocols\r\n"
    "Upgrade: websocket\r\n"
    "Connection: Upgrade\r\n"
    "Sec-WebSocket-Accept: %s\r\n\r\n", accept_key);
```

> **[INSERT SCREENSHOT: Wireshark or browser DevTools showing the 101 response]**

---

### 1.4 Concurrency Model — fork() + pthreads

**Chosen model:** `fork()` per connection + `pthread` for background tasks.

**Justification:**

| Alternative | Why Rejected |
|-------------|-------------|
| Single-threaded select() loop | Cannot block on recv() without starving other connections |
| Thread-per-connection (pthreads only) | Shared address space means memory bugs in one thread crash everything |
| Async I/O (epoll) | Significantly more complex; banned by project constraints |
| **fork() per connection ✓** | Process isolation — a crash in one child does not affect the parent or other connections |

**Thread responsibilities (in parent process):**

| Thread | Name | Purpose |
|--------|------|---------|
| Main thread | accept loop | Accepts new TCP connections, forks a child per client |
| `db_write_thread` | DB writer | Drains the log pipe and writes to SQLite — serialises all log writes |
| `heartbeat_thread` | Heartbeat monitor | Every 30s, scans shared memory and marks devices offline if no heartbeat |

**IPC between parent and children:**

```
Parent process                    Child process (per device)
     │                                    │
     │── POSIX Shared Memory (SharedRegistry) ◄──► mutex via named semaphore
     │
     │── Pipe (db_pipe) ◄─────────────── child writes LogEntry struct
     │
     │── SIGUSR1 ────────────────────────► child wakes up, reads pending_command
```

> **[INSERT DIAGRAM: Draw a box for parent, two boxes for child1/child2, arrows showing shm + pipe + signal]**

---

## Q2 — WebSocket Server Implementation Details

### 2.1 Handling Multiple Client Types

The server distinguishes clients by their first message:

```
TCP connect → WS handshake → first message:
   type=auth      → CLIENT_WEB  (browser dashboard)
   type=register  → CLIENT_DEVICE (IoT device)
```

Each child process holds a `Connection` struct that tracks the client's type, state,
session token, device ID, and a receive buffer for partial reads.

---

### 2.2 Partial Read & Write Handling

**The Problem:** TCP is a stream protocol. A single `recv()` call may return less data
than one complete WebSocket frame. Similarly, `send()`/`write()` may send only part of
the frame.

**Partial Reads — solution:** Accumulate data in `recv_buf` and only process frames
when `ws_parse_frame()` returns a positive value (meaning a complete frame was found).

```c
ssize_t n = recv(client_fd, buf + recv_len, sizeof(buf) - recv_len, 0);
recv_len += (int)n;
// Only process if a complete frame is available:
while ((consumed = ws_parse_frame(buf, recv_len, ...)) > 0) {
    // handle frame
    memmove(buf, buf + consumed, recv_len - consumed);
    recv_len -= consumed;
}
```

**Partial Writes — solution:** `ws_send_frame()` loops until all bytes are written:

```c
while (total < frame_len) {
    ssize_t n = write(fd, frame + total, frame_len - total);
    if (n < 0 && errno == EAGAIN) { usleep(1000); continue; }
    total += (int)n;
}
```

---

### 2.3 WebSocket Frame Format (RFC 6455)

```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
┌─┬─┬─┬─┬───────┬─┬─────────────────────────────────────────────┐
│F│R│R│R│  Op   │M│         Payload Length (7/16/64 bits)        │
│I│S│S│S│ code  │S│                                              │
│N│V│V│V│(4 bit)│K│                                              │
│ │1│2│3│       │ │                                              │
└─┴─┴─┴─┴───────┴─┴─────────────────────────────────────────────┘
│                  Masking Key (if MASK=1, 4 bytes)              │
│                  Payload Data                                   │
```

Our frame parser handles all three payload-length encodings:
- `< 126` → 7-bit length in byte 1
- `= 126` → next 2 bytes give length
- `= 127` → next 8 bytes give length

**Opcodes handled:**

| Opcode | Hex | Action |
|--------|-----|--------|
| Text frame | 0x1 | Route to `handle_message()` |
| Binary frame | 0x2 | Reject with close code 1003 |
| Close | 0x8 | Echo close frame, exit child |
| Ping | 0x9 | Respond with Pong (same payload) |
| Pong | 0xA | Silently ignore |
| Oversized (>4096B) | — | Close with code 1009 |

---

### 2.4 Command Routing (Web → Device)

```
Web browser                 Server (parent shared mem)       Device child
    │                               │                            │
    │── command{device_id, action} ─►│                            │
    │                        sem_wait()                          │
    │                        dev->pending_command = action       │
    │                        kill(dev->child_pid, SIGUSR1)       │
    │                        sem_post()                          │
    │                               │                            │
    │                               │◄──── SIGUSR1 ─────────────│
    │                               │                     command_received=1
    │                               │                            │
    │                               │                    sem_wait()
    │                               │                    copy pending_command
    │                               │                    pending_command=""
    │                               │                    sem_post()
    │                               │                    ws_send_text(cmd)──►[device]
    │◄── command_ack ───────────────│                            │
```

> **[INSERT SCREENSHOT: Server terminal showing child PIDs and command routing logs]**

> **[INSERT SCREENSHOT: Dashboard showing a device state change after a command]**

---
---

# MEMBER 2 — Database Design & Synchronization

## Q3 — Database Integration & Real-Time Impact

### 3.1 Schema Design

Three tables are used, created in `src/db.c`:

**`users` table** — stores admin credentials:
```sql
CREATE TABLE IF NOT EXISTS users (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    username      TEXT    UNIQUE NOT NULL,
    password_hash TEXT    NOT NULL,
    created_at    INTEGER NOT NULL
);
```

**`devices` table** — persistent device registry:
```sql
CREATE TABLE IF NOT EXISTS devices (
    device_id   TEXT    PRIMARY KEY,
    device_type TEXT    NOT NULL,
    state       TEXT    NOT NULL DEFAULT 'offline',
    last_seen   INTEGER NOT NULL DEFAULT 0
);
```

**`activity_log` table** — command audit trail:
```sql
CREATE TABLE IF NOT EXISTS activity_log (
    id        INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp INTEGER NOT NULL,
    username  TEXT    NOT NULL DEFAULT '',
    device_id TEXT    NOT NULL DEFAULT '',
    action    TEXT    NOT NULL,
    source_ip TEXT    NOT NULL DEFAULT ''
);
```

> **[INSERT SCREENSHOT: SQLite browser or sqlite3 shell showing all three tables with data]**

---

### 3.2 Prepared Statements

All queries are compiled once at startup as `sqlite3_stmt*` pointers and reused
with `sqlite3_bind_*` / `sqlite3_reset()`. This prevents SQL injection and avoids
per-query compilation cost.

Six prepared statements:

| Variable | SQL Purpose |
|----------|------------|
| `stmt_user_lookup` | Fetch password hash by username |
| `stmt_device_upsert` | Insert or replace device row |
| `stmt_device_update` | Update device state + last_seen |
| `stmt_device_list` | Fetch all devices (sorted) |
| `stmt_log_insert` | Insert activity log row |
| `stmt_log_fetch` | Fetch last N log rows (DESC) |

---

### 3.3 Database Synchronization & Real-Time Impact

**Problem:** Multiple child processes (one per device/client) can call
`db_update_device_state()` concurrently.

**Solution — WAL mode:**
```sql
PRAGMA journal_mode=WAL;   -- Writer does not block readers
PRAGMA synchronous=NORMAL; -- Balanced durability vs. speed
```

**Activity log write path — serialised via pipe:**

All log writes go through a `pipe` to the parent's `db_write_thread`, which is the
**only** thread that calls `db_log_activity()`. This eliminates concurrent writer
contention for the most frequent write path.

**Real-time impact analysis:**

| Operation | Frequency | Latency | Impact |
|-----------|-----------|---------|--------|
| `db_log_activity` | Every command | ~0.2ms (WAL) | Low — async via pipe |
| `db_update_device_state` | Every heartbeat (30s) | ~0.3ms | Negligible |
| `db_get_activity_log` | Every 5s (poll) | ~1ms (SELECT) | Acceptable |

**Authoritative real-time source:** The in-memory `SharedRegistry` (POSIX shared memory),
NOT the database. The DB is used only for persistence and log queries.

> **[INSERT SCREENSHOT: Activity log shown in the web dashboard]**

---
---

# MEMBER 3 — Authentication & TCP State Analysis

## Q4 — Authentication Mechanisms & TCP State Impact

### 4.1 Authentication Flow

**Step 1 — Password hashing (SHA-256):**

Passwords are never stored in plain text. On login, the input password is hashed
with SHA-256 using OpenSSL and compared against the stored hash:

```c
char *auth_hash_password(const char *password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)password, strlen(password), hash);
    // convert to 64-char hex string
}
```

Default seed: username `admin`, password `admin123`
→ SHA-256: `240be518fabd2724ddb6f04eeb1da5967448d7e831c08c8fa822809f74c720a9`

**Step 2 — Session token generation:**

On successful login, 32 random bytes are read from `/dev/urandom` and hex-encoded
into a 64-character token:

```c
void auth_generate_token(char *out_token) {
    int fd = open("/dev/urandom", O_RDONLY);
    read(fd, buf, 32);  // cryptographically random
    // encode to hex...
}
```

**Step 3 — Token validation on every subsequent message:**

Every `get_devices`, `command`, and `get_log` message must include the token.
The server calls `auth_validate_token()` before processing:

```c
int auth_validate_token(const Connection *conn, const char *token) {
    return (strncmp(conn->session_token, token, 64) == 0) ? 1 : 0;
}
```

**3-Attempt Lockout:**

After 3 consecutive failed login attempts, the server sends a final error message,
closes the WebSocket connection with code 1008 (Policy Violation), and sets a
`close_conn` flag to break the child process loop.

> **[INSERT SCREENSHOT: Browser DevTools showing auth_result messages]**

---

### 4.2 WebSocket Security Design

Our implementation uses **plain `ws://`** (no TLS) as required by the project spec.
In a production deployment, `wss://` (WebSocket Secure over TLS) would be used — this
encrypts the entire connection so that tokens and commands cannot be intercepted.

**Why `ws://` is acceptable for this lab:**
- The server runs only on `localhost`
- Network traffic does not traverse public infrastructure
- The SHA-256 password hash still protects credentials at rest in the DB

---

### 4.3 TCP State Analysis: TIME_WAIT and CLOSE_WAIT

**TIME_WAIT:**
- Occurs on the **server side** after the server actively closes a connection.
- The socket stays in TIME_WAIT for `2 × MSL` (≈60s) to absorb any delayed TCP
  segments that arrive after the connection closes.
- **Impact on our system:** If the server restarts without `SO_REUSEADDR`, `bind()` fails
  because port 8080 is occupied by a TIME_WAIT socket.
- **Our fix:** `setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))` —
  allows immediate re-binding even during TIME_WAIT.

**CLOSE_WAIT:**
- Occurs when the **remote peer** (client/device) sends FIN but our server has not yet
  closed its end.
- A large number of CLOSE_WAIT sockets = resource leak (each holds a file descriptor).
- **Our fix:** `SO_KEEPALIVE` is set on every accepted socket. The OS sends keepalive
  probes after ~2 hours of inactivity; if the remote is unreachable, the connection is
  reset, moving the socket out of CLOSE_WAIT automatically.
- Additionally, the heartbeat thread marks devices as offline after
  `HEARTBEAT_TIMEOUT` seconds using the in-memory registry.

```
TCP State flow for a device disconnect:

ESTABLISHED  →  (device dies, no FIN)
     │
     │  keepalive probes sent every ~75s (SO_KEEPALIVE)
     │  no response after 9 probes
     ▼
CLOSE_WAIT   →  kernel resets connection
     ▼
CLOSED       →  child process breaks out of recv loop
     ▼
dm_set_offline() called → device marked offline in shared memory + DB
```

> **[INSERT SCREENSHOT: netstat -tn output showing any CLOSE_WAIT or TIME_WAIT entries]**

> **[INSERT SCREENSHOT: Dashboard showing a device going offline after simulator stops]**

---

*End of FISAC-1 Document Guide*
