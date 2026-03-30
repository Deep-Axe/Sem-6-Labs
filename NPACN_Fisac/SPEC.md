# SPEC.md — Smart Home Controller: Master Specification

## Project Goal
Build a C-based IoT Smart Home Controller where a WebSocket server (written in pure C using POSIX sockets) manages multiple IoT device connections (lights, fans, security cameras) and serves a web dashboard. Users log in via the web interface, send commands to devices in real time, and all activity is persisted in a SQLite database.

## Submission Context
This is an academic project demonstrating socket programming, concurrency, WebSocket protocol implementation, and database integration in C. Code must be clean, commented, and industry-standard. No shortcuts. No hardcoded values.

---

## Folder Structure (FINAL — do not deviate)
```
SmartHomeController/
├── SPEC.md
├── ARCHITECTURE.md
├── PROTOCOL.md
├── DB_SCHEMA.md
├── TASKS.md
├── CONSTRAINTS.md
├── server/
│   ├── main.c
│   ├── websocket.c
│   ├── websocket.h
│   ├── device_manager.c
│   ├── device_manager.h
│   ├── auth.c
│   ├── auth.h
│   ├── db.c
│   ├── db.h
│   ├── logger.c
│   ├── logger.h
│   ├── utils.c
│   ├── utils.h
│   └── Makefile
├── client/
│   ├── index.html
│   ├── app.js
│   └── style.css
├── simulator/
│   └── device_sim.py
└── README.md
```

---

## Feature Checklist

### Server Core
- [ ] TCP server socket with SO_REUSEADDR and TCP_NODELAY
- [ ] Accepts connections on configurable port (default 8080), read from config or argv
- [ ] fork()-based connection handling — one child process per accepted connection
- [ ] pthreads for heartbeat monitor thread and DB write queue thread
- [ ] POSIX shared memory (shm_open + mmap) for device registry shared across processes
- [ ] Named semaphores (sem_open) for cross-process synchronization on shared memory
- [ ] SIGCHLD handler with waitpid() to reap zombie child processes
- [ ] Distinguishes between IoT device clients and web browser clients by first message type
- [ ] Graceful shutdown on SIGINT/SIGTERM with proper socket cleanup

### WebSocket
- [ ] RFC 6455 compliant HTTP Upgrade handshake
- [ ] SHA-1 + Base64 for Sec-WebSocket-Accept key generation
- [ ] Text frame parsing (opcode 0x1)
- [ ] Binary frame rejection with proper close frame response
- [ ] Ping/Pong control frame handling (opcode 0x9 / 0xA)
- [ ] Close frame handling (opcode 0x8)
- [ ] Partial read handling — reassemble fragmented TCP segments
- [ ] Partial write handling — retry until all bytes sent
- [ ] Client-to-server masking support (unmask incoming frames)
- [ ] Server-to-client unmasked frames

### Authentication
- [ ] POST-style login over WebSocket (first message after handshake)
- [ ] Password stored as SHA-256 hex digest in DB
- [ ] Session token = 32-byte random hex string, stored in memory per connection
- [ ] All subsequent messages validated against session token
- [ ] Failed auth closes connection after sending error response

### Device Management
- [ ] Register device on first connect (device sends identity message)
- [ ] Track device state: on/off for lights and fans, active/inactive for cameras
- [ ] Heartbeat from device updates last_seen timestamp in DB
- [ ] Detect device disconnect via select() EOF, update state to offline
- [ ] Web client can request list of all devices and their current states
- [ ] Web client can send command to specific device by device_id

### Database (SQLite)
- [ ] Auto-create DB file and tables on first run
- [ ] Users table: id, username, password_hash, created_at
- [ ] Devices table: device_id, type, state, last_seen
- [ ] Activity log table: id, timestamp, username, device_id, action, source_ip
- [ ] All DB access via sqlite3_bind_* (no string interpolation)
- [ ] DB initialized once at startup, handle passed globally via pointer

### Web Client
- [ ] Single-page dashboard (index.html + app.js + style.css)
- [ ] Login form — sends auth message over WebSocket
- [ ] Device grid showing all devices with current state
- [ ] Toggle button per device — sends command message
- [ ] Activity log panel showing last 20 actions
- [ ] Auto-reconnect on WebSocket disconnect
- [ ] All values dynamic — no hardcoded device names or states

### Device Simulator
- [ ] Python script (device_sim.py) simulating 3 devices: light_1, fan_1, camera_1
- [ ] Each device connects as a separate WebSocket client
- [ ] Sends identity message on connect
- [ ] Sends heartbeat every 10 seconds
- [ ] Responds to on/off commands with state_update confirmation
- [ ] Accepts device_id, server host, and port as CLI arguments

---

## What Done Looks Like
1. `make` in server/ compiles with zero warnings
2. `./server 8080` starts the server
3. Running `ps aux` shows multiple child processes when devices are connected
4. `python3 simulator/device_sim.py --host localhost --port 8080` connects 3 simulated devices
5. Opening `client/index.html` in browser shows login screen
6. Login with admin/admin123 succeeds
7. Dashboard shows light_1, fan_1, camera_1 with their states
8. Clicking toggle on light_1 sends command, simulator receives it, state updates on dashboard
9. All actions appear in activity log panel
10. Database file `smarthome.db` contains all records
11. Ctrl+C on server shuts down cleanly

---

## Code Style Reference

The `Ref/` folder in the project root contains C code written by the author of this project. It is organized as:

```
Ref/
├── lab0/
├── lab1/
├── lab2/
├── lab3/
├── lab4/
├── lab5/
├── lab6/
└── lab7/
```

Each lab folder contains C source files for practice questions. Some lab folders may also contain subdirectories — those subdirectories also contain C source files and must be read as well.

**Before writing any code for this project, recursively read all .c and .h files found anywhere inside `Ref/` and extract the author's coding style.**

Specifically learn:
- Variable and function naming conventions
- Brace style, indentation width, spacing around operators
- How functions are structured (early return vs nested if, where error handling sits)
- Comment style — density, placement, what gets a comment and what doesn't
- How structs and typedefs are written
- How includes are ordered in .c and .h files
- Preferred idioms for loops, string ops, and pointer usage

**Rules:**
- Apply this style uniformly across all files you generate for this project
- Do not copy any logic from Ref/ — extract style only
- If a style pattern from Ref/ conflicts with CONSTRAINTS.md, CONSTRAINTS.md wins
