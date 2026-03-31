# Smart Home Controller

A C-based IoT Smart Home Controller with a WebSocket server, device management, and a web dashboard.

## Dependencies
- `gcc`
- `libsqlite3-dev`
- `libssl-dev`
- `python3`
- `python3-websockets`

## Build and Run

### 1. Build the server
```bash
cd server
make
```

### 2. Start the server
```bash
./server 8080
```

### 3. Start the device simulator
```bash
python3 simulator/device_sim.py --port 8080
```

### 4. Open the dashboard
Open `client/index.html` in your web browser.

## Credentials
- **Username:** `admin`
- **Password:** `admin123`

## Architecture
The server uses a hybrid concurrency model:
- `fork()` for handling each client connection in a separate process.
- `pthreads` for background tasks: heartbeat monitoring and database write queue.
- POSIX Shared Memory and Named Semaphores for cross-process state management.
- Custom WebSocket implementation (RFC 6455) using POSIX sockets.
