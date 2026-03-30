# TASKS.md — Ordered Build Sequence

## Instructions for Gemini CLI
- Complete tasks in order. Do not skip ahead.
- After each task, the code must compile (if C) or run (if Python/HTML) before moving on.
- Each task has a Verify step — run it and confirm it passes before proceeding.
- Do not add features not listed in the task. Scope creep breaks later tasks.
- All decisions are in ARCHITECTURE.md, PROTOCOL.md, DB_SCHEMA.md — follow them exactly.

---

## TASK 1 — Project Skeleton and Makefile

**Goal:** Create all files as stubs, verify clean compilation.

Create these files in server/:
- `main.c` — just `int main() { return 0; }`
- `websocket.c` + `websocket.h` — empty stubs
- `device_manager.c` + `device_manager.h` — empty stubs
- `auth.c` + `auth.h` — empty stubs
- `db.c` + `db.h` — empty stubs
- `logger.c` + `logger.h` — empty stubs
- `utils.c` + `utils.h` — empty stubs

Create `server/Makefile`:
```makefile
CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -std=c99 -g
LIBS    = -lsqlite3 -lssl -lcrypto
TARGET  = server
SRCS    = main.c websocket.c device_manager.c auth.c db.c logger.c utils.c

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

clean:
	rm -f $(TARGET)
```

**Verify:** `cd server && make` → zero errors, zero warnings.

---

## TASK 2 — TCP Server Socket

**File:** `main.c`

Implement:
- Parse port from `argv[1]`, default to 8080 if not provided
- Parse optional `--db <path>` argument, default to `smarthome.db`
- Create server socket with `socket(AF_INET, SOCK_STREAM, 0)`
- Apply `SO_REUSEADDR` (see ARCHITECTURE.md)
- `bind()` to `0.0.0.0:<port>`
- `listen()` with backlog 10
- Print: `[INFO] Server listening on port <port>`
- Accept loop: `accept()`, print `[INFO] New connection from <ip>:<port>`, immediately close(fd) (placeholder)
- Handle SIGINT with a flag: `volatile sig_atomic_t running = 1;`

**Verify:** 
```bash
./server 8080
# In another terminal:
nc localhost 8080
# Server should print connection message
```

---

## TASK 3 — WebSocket Handshake

**Files:** `websocket.c`, `websocket.h`

Implement `int ws_handshake(int fd)`:
1. `recv()` HTTP request into buffer (up to 4096 bytes)
2. Check for `GET` and `Upgrade: websocket` headers
3. Extract `Sec-WebSocket-Key` value
4. Concatenate with magic: `258EAFA5-E914-47DA-95CA-C5AB0DC85B11`
5. SHA-1 hash using `SHA1()` from `<openssl/sha.h>`
6. Base64 encode 20-byte result using OpenSSL BIO
7. Send HTTP 101 response:
```
HTTP/1.1 101 Switching Protocols\r\n
Upgrade: websocket\r\n
Connection: Upgrade\r\n
Sec-WebSocket-Accept: <base64>\r\n
\r\n
```
8. Return 0 on success, -1 on failure

Wire into `main.c`: after `accept()`, call `ws_handshake(fd)` instead of close().

**Verify:**
```bash
# Install wscat: npm install -g wscat
wscat -c ws://localhost:8080
# Should connect without error
```

---

## TASK 4 — WebSocket Frame Parser

**Files:** `websocket.c`, `websocket.h`

Implement:
```c
// Returns bytes consumed from buf, 0 if incomplete frame, -1 on error
// Fills out_payload (caller-allocated), out_len, out_opcode
int ws_parse_frame(const char *buf, int buf_len,
                   char *out_payload, int *out_len, uint8_t *out_opcode);
```

Handle:
- FIN bit (assert FIN=1 for now, no fragmentation support needed)
- MASK bit (unmask if set using 4-byte masking key)
- Payload length: 7-bit, 16-bit extended, 64-bit extended
- Opcodes: 0x1 (text), 0x8 (close), 0x9 (ping), 0xA (pong)
- Return 0 if buffer doesn't contain a complete frame yet

**Verify:** Unit test in main.c temporarily — construct a masked text frame manually, call ws_parse_frame, print payload.

---

## TASK 5 — WebSocket Frame Writer

**Files:** `websocket.c`, `websocket.h`

Implement:
```c
// Send a text frame to fd. Handles partial writes.
// Returns 0 on success, -1 on error
int ws_send_text(int fd, const char *payload, int len);

// Send a close frame
int ws_send_close(int fd, uint16_t code);

// Send a pong frame
int ws_send_pong(int fd, const char *payload, int len);
```

Frame format for server→client (unmasked):
```
Byte 0: 0x81 (FIN=1, opcode=text)
Byte 1: payload_len (if <= 125)
        or 0x7E + 2-byte big-endian len (if 126-65535)
[Payload bytes]
```

Partial write loop:
```c
int total = 0;
while (total < frame_len) {
    int n = write(fd, frame + total, frame_len - total);
    if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) { usleep(1000); continue; }
    if (n < 0) return -1;
    total += n;
}
```

**Verify:**
```bash
wscat -c ws://localhost:8080
# Server should be able to send a test message on connect
```

---

## TASK 6 — fork() Connection Handler + SIGCHLD + Shared Memory

**Files:** `main.c`, `device_manager.c`, `device_manager.h`

Part A — Shared memory setup (in `main.c`, before accept loop):
- `shm_open(SHM_NAME, O_CREAT|O_RDWR, 0666)` → `ftruncate` → `mmap`
- `sem_open(SEM_NAME, O_CREAT, 0666, 1)`
- Store `SharedRegistry*` as a global pointer accessible by parent and children after fork

Part B — SIGCHLD handler (in `main.c`):
```c
void sigchld_handler(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}
```
Register with: `signal(SIGCHLD, sigchld_handler)`

Part C — fork() accept loop (in `main.c`):
- `accept()` → `fork()`
- Parent: `close(client_fd)`, continue
- Child: `close(server_fd)`, run full connection lifecycle, `exit(0)`

Part D — Heartbeat thread (in `main.c`):
- `pthread_create` for heartbeat_thread function
- Every 5 seconds: `sem_wait`, scan `SharedRegistry`, mark `connected=0` if `last_seen > 30s`, `sem_post`

Part E — Cleanup on SIGINT (in `main.c`):
- Close all fds
- `sem_unlink(SEM_NAME)`
- `shm_unlink(SHM_NAME)`
- `db_close()`
- `exit(0)`

**Verify:**
- Start server, connect simulator
- `ps aux | grep server` → should show multiple processes
- Kill one device → heartbeat thread marks it offline within 35 seconds

---

## TASK 7 — SQLite Database Layer

**Files:** `db.c`, `db.h`

Implement all functions from DB_SCHEMA.md exactly:
- `db_init()` — open file, PRAGMAs, CREATE TABLE IF NOT EXISTS, seed admin user, compile prepared statements
- `db_close()`
- `db_get_user()`
- `db_upsert_device()`
- `db_update_device_state()`
- `db_get_all_devices()` — returns malloc'd JSON string, caller must free()
- `db_log_activity()`
- `db_get_activity_log()` — returns malloc'd JSON string, caller must free()

Wire `db_init(db_path)` into `main.c` startup. Wire `db_close()` into shutdown.

**Verify:**
```bash
./server 8080
# Should create smarthome.db
sqlite3 smarthome.db "SELECT * FROM users;"
# Should show admin row
```

---

## TASK 8 — Authentication Module

**Files:** `auth.c`, `auth.h`

Implement:
```c
// Hash password with SHA-256, return hex string (caller must free)
char *auth_hash_password(const char *password);

// Generate 32-byte random session token as 64-char hex string
// Writes into out_token (must be char[65])
void auth_generate_token(char *out_token);

// Validate token against connection's stored token
// Returns 1 if valid, 0 if not
int auth_validate_token(const Connection *conn, const char *token);

// Full login flow: lookup user, compare hash, generate token
// Returns 0 on success (fills conn->session_token, conn->username)
// Returns -1 on failure
int auth_login(Connection *conn, const char *username, const char *password);
```

Use `/dev/urandom` for token generation:
```c
int fd = open("/dev/urandom", O_RDONLY);
unsigned char buf[32];
read(fd, buf, 32);
close(fd);
// Convert to hex string
```

**Verify:** Unit test — call auth_hash_password("admin123"), verify output matches the hash in DB_SCHEMA.md.

---

## TASK 9 — Device Manager

**Files:** `device_manager.c`, `device_manager.h`

Implement SharedRegistry management via shared memory:

```c
// Initialize registry (called by parent before fork)
void dm_init(void);

// Register device (from register message)
// Returns 0 on success, -1 if registry full
// Must acquire named semaphore
int dm_register_device(const char *device_id, const char *device_type,
                       const char *initial_state, int fd);

// Find device by device_id in SharedRegistry
Device *dm_find_device(const char *device_id);

// Update device state in SharedRegistry and DB
int dm_update_state(const char *device_id, const char *state);

// Mark device offline (on disconnect)
void dm_set_offline(const char *device_id);

// Build JSON array of all devices from SharedRegistry (caller must free)
char *dm_get_all_json(void);

// Update last_seen timestamp in SharedRegistry
void dm_heartbeat(const char *device_id, const char *state);
```

**Verify:** Register 3 devices, call dm_get_all_json(), print output — verify JSON matches PROTOCOL.md device_list format.

---

## TASK 10 — Message Router

**Files:** `main.c` (add `handle_message` function)

Implement `void handle_message(Connection *conn, const char *payload, int len)`:

Parse JSON with cJSON. Route by `"type"` field:

**If conn->state == STATE_WS_HANDSHAKE_DONE (not yet authenticated):**
- `"auth"` → call auth_login(), send auth_result, set state to STATE_AUTHENTICATED or exit
- `"register"` → call dm_register_device(), send register_ack, set conn type and state

**If conn->type == CLIENT_WEB and state == STATE_AUTHENTICATED:**
- Validate token first. On failure: send INVALID_TOKEN error, exit.
- `"get_devices"` → dm_get_all_json(), send device_list response
- `"command"` → validate action, find device in SharedRegistry, find child_pid, send command_ack, db_log_activity()
- `"get_log"` → db_get_activity_log(), send activity_log response

**If conn->type == CLIENT_DEVICE:**
- `"heartbeat"` → dm_heartbeat(), db_update_device_state()
- `"state_update"` → dm_update_state(), db_update_device_state()

**Note on Broadcast:** Child processes are isolated. State updates are written to SharedRegistry. Web clients poll via `get_devices` every 3 seconds (app.js polling loop). Real broadcast would require a pipe or Unix socket between children, which is out of scope — polling is the chosen approach.

**Unknown type:** log warning, send INVALID_MESSAGE error.

**Verify:** Full manual test — wscat as device, wscat as web client, send auth, send get_devices, send command.

---

## TASK 11 — Web Dashboard

**Files:** `client/index.html`, `client/app.js`, `client/style.css`

### index.html
- Login screen (shown before auth): username input, password input, Login button
- Dashboard (shown after auth, hidden initially):
  - Header with "Smart Home Controller" title and logout button
  - Device grid section
  - Activity log section (last 20 entries in a table)
- No hardcoded device names — all rendered dynamically from server messages

### app.js
```javascript
const WS_URL = `ws://${window.location.hostname}:8080`;
// If opened as file://, use ws://localhost:8080

// State
let ws = null;
let token = null;
let reconnectTimer = null;

function connect() { ... }          // Create WebSocket, attach handlers
function onOpen() { ... }           // Nothing — wait for user to login
function onMessage(event) { ... }   // Route by type field
function onClose() { ... }          // Schedule reconnect after 3 seconds
function login() { ... }            // Read inputs, send auth message
function sendCommand(deviceId, action) { ... }  // Send command message
function renderDevices(devices) { ... }         // Build device grid DOM
function renderLog(entries) { ... }             // Build log table DOM
function requestDevices() { ... }   // Send get_devices message
function requestLog() { ... }       // Send get_log message
```

Auto-reconnect: on close, `reconnectTimer = setTimeout(connect, 3000)`

**Verify:** Open index.html in browser, login, see devices, click toggle, see state change.

---

## TASK 12 — Device Simulator

**File:** `simulator/device_sim.py`

```python
import asyncio
import websockets
import json
import argparse
import time

# CLI args: --host, --port
# Simulates 3 devices as separate asyncio tasks
# Each task: connect, register, heartbeat loop, respond to commands
```

Devices:
- `light_1` (type: light, initial: off)
- `fan_1` (type: fan, initial: off)
- `camera_1` (type: camera, initial: inactive)

Each device task:
1. Connect to `ws://<host>:<port>`
2. Send register message
3. Wait for register_ack
4. Start heartbeat loop (every 10s)
5. Listen for command messages, update state, send state_update

**Verify:**
```bash
python3 simulator/device_sim.py --host localhost --port 8080
# Should show 3 connected devices in server logs
# Dashboard should show all 3 devices
```

---

## TASK 13 — README

**File:** `README.md`

Include:
- Project overview (2 sentences)
- Dependencies: `gcc`, `libsqlite3-dev`, `libssl-dev`, `python3`, `python3-websockets`
- Build: `cd server && make`
- Run server: `./server <port> [--db <path>]`
- Run simulator: `python3 simulator/device_sim.py --host <host> --port <port>`
- Run client: open `client/index.html` in browser
- Default credentials: admin / admin123
- Architecture summary (3-4 sentences)

---

## TASK 14 — Error Handling Pass

**All files**

Go through every .c file and verify:
- [ ] Every `socket()`, `bind()`, `listen()`, `accept()`, `recv()`, `send()`, `write()` return value checked
- [ ] Every `malloc()` / `cJSON_Print()` result checked for NULL before use
- [ ] Every `sqlite3_*` call return code checked, errors logged
- [ ] Partial reads: recv_buf accumulation logic handles split TCP segments
- [ ] Partial writes: ws_send_text retry loop handles EAGAIN
- [ ] All `cJSON_Delete()` calls present after every `cJSON_Parse()`
- [ ] No memory leak: every `malloc()` has a matching `free()` on all paths
- [ ] Malformed JSON (cJSON_Parse returns NULL): handled gracefully, send INVALID_MESSAGE error
- [ ] Empty payload (len == 0): handled
- [ ] Oversized payload (> 4096 bytes): send close frame with code 1009, disconnect

**Verify:** Run with valgrind:
```bash
valgrind --leak-check=full ./server 8080
# Connect simulator and web client, run through all operations
# Zero memory leaks reported
```

---

## TASK 15 — Final Integration Test

**All components running together**

Test matrix:

| # | Test | Expected |
|---|------|----------|
| 1 | Server starts on port 8080 | `[INFO] Server listening on port 8080` |
| 2 | Simulator connects 3 devices | 3 `[INFO] Device registered` messages |
| 3 | Browser opens dashboard | Login screen shown |
| 4 | Login with admin/admin123 | Dashboard shown |
| 5 | Dashboard shows 3 devices | light_1, fan_1, camera_1 visible |
| 6 | Click "Turn On" for light_1 | State changes to "on" in UI |
| 7 | Simulator receives command | Prints "light_1 received command: on" |
| 8 | Activity log shows the action | Row with admin, light_1, "on" |
| 9 | Login with wrong password | Error message shown, connection closed |
| 10 | Send command without token | INVALID_TOKEN error |
| 11 | Kill simulator | Devices show "offline" in dashboard |
| 12 | Restart simulator | Devices reconnect and show correct state |
| 13 | Ctrl+C server | Clean shutdown, no port reuse error on restart |
| 14 | Open 3 browser tabs | All 3 receive state updates simultaneously |
| 15 | Check smarthome.db | All tables populated correctly |
