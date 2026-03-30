# ARCHITECTURE.md — All Design Decisions (Do Not Change)

## Concurrency Model
Two concurrency mechanisms are used together:

**fork() — per-connection process:**
- On accept(), fork() a child process to handle the new connection
- Child process handles the full lifecycle: WebSocket handshake, authentication, message loop
- Parent process closes the client fd and loops back to accept()
- Child process closes the server fd after fork()
- On child exit: parent handles SIGCHLD with waitpid() to prevent zombies

**pthreads — background tasks within the server:**
- One dedicated pthread for heartbeat checking (runs every 5 seconds, marks devices offline if last_seen > 30s)
- One dedicated pthread for database write queue (decouples DB writes from the message handling path)
- Shared state between threads protected by pthread_mutex_t

**Shared state between processes:**
- device_registry stored in POSIX shared memory (shm_open + mmap) so all forked children can read/write device states
- activity_log writes go through the DB write queue thread via a pipe
- pthread_mutex_t replaced by a named semaphore (sem_open) for cross-process locking on shared memory

**Why this model:**
- fork() demonstrates process-level concurrency and TCP socket inheritance
- pthreads demonstrates thread-level concurrency within a single process
- Together they satisfy "concurrency management" at both OS levels
- Named semaphores demonstrate IPC synchronization

---

## Connection Lifecycle

TCP CONNECT
    │
    ▼
parent: accept(server_fd) → fork()
    │
    ├─ PARENT: close(client_fd) → back to accept()
    │
    └─ CHILD: close(server_fd)
           │
           ▼
       ws_handshake(fd)
           │
           ▼
       First message
       ├─ "auth"     → CLIENT_WEB
       └─ "register" → CLIENT_DEVICE
           │
           ▼
       message loop (blocking recv, no select needed)
           │
           ▼
       EOF / close frame / error
           │
           ▼
       cleanup_connection()
       update shared memory device state
       exit(0)

---

## Per-Child Connection Handling
Each forked child process owns exactly one Connection struct on its stack:

```c
typedef enum { CLIENT_UNKNOWN, CLIENT_WEB, CLIENT_DEVICE } ClientType;
typedef enum { STATE_TCP_CONNECTED, STATE_WS_HANDSHAKE_DONE, STATE_AUTHENTICATED } ConnState;

typedef struct {
    int fd;
    ClientType type;
    ConnState state;
    char session_token[65];       // 32-byte hex + null
    char username[64];            // for web clients
    char device_id[64];           // for iot devices
    char recv_buf[4096];          // partial read buffer
    int  recv_len;                // bytes currently in recv_buf
    char remote_ip[INET_ADDRSTRLEN];
} Connection;
```

### Child process flow:
1. `fork()` returns 0 (child)
2. `close(server_fd)`
3. `ws_handshake(conn.fd)`
4. message loop: `recv` → `ws_parse_frame` → `handle_message` → repeat
5. `cleanup_connection(&conn)`
6. `exit(0)`

### Parent process flow:
1. `accept()` → `fork()`
2. `close(client_fd)`
3. loop back to `accept()`
4. `SIGCHLD` handler calls `waitpid(-1, NULL, WNOHANG)` in a loop

---

## Socket Options (apply exactly these, in this order)

### Server listening socket:
```c
int opt = 1;
setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
```

### Each accepted client socket:
```c
int flag = 1;
setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));

int keepalive = 1;
setsockopt(client_fd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));

// Note: sockets are BLOCKING in this model
```

---

## WebSocket Implementation (RFC 6455)

### Handshake:
1. Read HTTP request into buffer
2. Find `Sec-WebSocket-Key:` header value
3. Concatenate with magic string `258EAFA5-E914-47DA-95CA-C5AB0DC85B11`
4. SHA-1 hash the concatenated string (use OpenSSL `SHA1()`)
5. Base64 encode the 20-byte hash (implement base64 manually or use OpenSSL `BIO_f_base64`)
6. Send HTTP 101 response with `Sec-WebSocket-Accept:` header

### Frame structure:
```
Byte 0: FIN(1) + RSV(3) + OPCODE(4)
Byte 1: MASK(1) + PAYLOAD_LEN(7)
[Extended payload length: 0, 2, or 8 bytes]
[Masking key: 4 bytes if MASK=1]
[Payload]
```

### Opcodes to handle:
- 0x0: Continuation frame
- 0x1: Text frame — process as JSON
- 0x8: Close frame — send close response, cleanup
- 0x9: Ping — respond with Pong (0xA)
- 0xA: Pong — ignore
- All others: send close frame with code 1003, disconnect

### Read strategy:
```c
// Blocking recv into recv_buf
// Accumulate bytes until we have a complete frame
// Complete frame = header parsed + payload_len bytes received
// Only process message when full frame is in buffer
// Shift buffer left after processing
```

### Write strategy:
```c
ssize_t ws_send(int fd, const char *payload, size_t len) {
    // Build frame into local buffer
    // Loop: write() until all bytes sent or error
    // On error: return -1
}
```

---

## Authentication Flow

### Login:
1. Web client sends: `{"type":"auth","username":"admin","password":"admin123"}`
2. Server looks up username in DB
3. SHA-256 hash the received password, compare with stored hash
4. If match: generate 32-byte random session token (use `/dev/urandom`), store in Connection struct, send success response with token
5. If no match: send failure response, close connection after 3 failed attempts

### Session validation:
- Every subsequent message from web client must include `"token":"<session_token>"`
- Server checks token against Connection.session_token
- Mismatch: send auth_required response, exit child

### Password hashing:
```c
// Use OpenSSL SHA256()
// Output: 32 bytes → convert to 64-char hex string
// Store hex string in DB
```

### Default user seeded at DB init:
- username: `admin`
- password: `admin123` (stored as SHA-256 hex)

---

## Device Manager
- On device register: add to SharedRegistry, upsert to DB
- On device command: find device by device_id in SharedRegistry, update state in registry and DB
- On device disconnect: update SharedRegistry state to "offline", update DB
- On heartbeat: update last_seen in SharedRegistry and DB

---

## Database Access Pattern
- Single `sqlite3*` db handle, opened at startup, closed at shutdown
- All queries use prepared statements compiled once at startup
- Writes: INSERT/UPDATE — call after in-memory state already updated
- DB write failure: log error, do NOT crash server (DB is persistent backup, memory is authoritative)
- No transactions needed for single-row ops; use BEGIN/COMMIT for batch log inserts

---

## Error Handling Philosophy
- Every syscall return checked
- Child process non-fatal error (bad frame, auth fail): log, send error response, continue loop
- Child process fatal error (recv returns 0 or -1 non-EAGAIN): cleanup and exit(0)
- Parent process fatal error (bind/listen fail): log to stderr, exit(1)
- pthread errors: log to stderr, exit(1) — background threads are non-optional
- Shared memory errors: log to stderr, exit(1) — required for device registry
- All errors: `fprintf(stderr, "[ERROR] %s: %s\n", __func__, strerror(errno));`
- All info logs: `printf("[INFO] %s\n", message);`

---

## TCP State Management
- TIME_WAIT: mitigated by SO_REUSEADDR on server socket
- CLOSE_WAIT: prevented by always calling close(fd) in cleanup_connection()
- Lingering connections: SO_KEEPALIVE detects dead peers
- Server shutdown: close all client fds first, then server fd, then sqlite3_close()

---

## Shared Memory Layout

```c
#define SHM_NAME     "/smarthome_devices"
#define SEM_NAME     "/smarthome_sem"
#define MAX_DEVICES  32

typedef struct {
    char device_id[64];
    char type[16];
    char state[16];
    time_t last_seen;
    int connected;      // 1 = connected, 0 = offline
    pid_t child_pid;    // pid of child process handling this device
} Device;

typedef struct {
    Device devices[MAX_DEVICES];
    int device_count;
} SharedRegistry;
```

### Access pattern in every child process:
```c
// sem_t *sem = sem_open(SEM_NAME, 0);
// sem_wait(sem);
// ... read/write SharedRegistry ...
// sem_post(sem);
// sem_close(sem);
```
