# CONSTRAINTS.md — Hard Rules (Non-Negotiable)

## Language and Standards
- Pure C99 only (`-std=c99` flag enforced in Makefile)
- POSIX.1-2008 socket APIs only
- No external networking libraries (no libev, libuv, libevent, boost, etc.)
- No threading libraries (no pthreads, no OpenMP)
- Allowed external libraries ONLY:
  - `libsqlite3` — database
  - `libssl` + `libcrypto` (OpenSSL) — SHA-1 for WebSocket handshake, SHA-256 for passwords
  - `cJSON` — JSON parsing (copy cJSON.c and cJSON.h directly into server/ folder)

## No Hardcoded Values
- Server port: must come from `argv[1]` or default constant `#define DEFAULT_PORT 8080`
- DB path: must come from `--db` argument or default `#define DEFAULT_DB_PATH "smarthome.db"`
- Max connections: `#define MAX_CONNECTIONS 64`
- Max devices: `#define MAX_DEVICES 32`
- Buffer size: `#define RECV_BUF_SIZE 4096`
- Heartbeat timeout: `#define HEARTBEAT_TIMEOUT_SEC 30`
- No hardcoded usernames, passwords, device IDs, or IP addresses anywhere in server code

## Memory Safety
- Every `malloc()` return value checked for NULL before use
- Every heap allocation has exactly one matching `free()` on all exit paths
- No stack-allocated buffers larger than 8192 bytes
- No `strcpy()` — use `strncpy()` with explicit size, or `snprintf()`
- No `sprintf()` — use `snprintf()` only
- No `gets()` — never
- All string buffers null-terminated explicitly after `strncpy()`

## System Call Discipline
- Every syscall return value checked:
  - `socket()`, `bind()`, `listen()`, `accept()` — exit(1) on failure
  - `recv()`, `send()`, `read()`, `write()` — handle -1 and 0 separately
  - `fcntl()`, `setsockopt()` — log warning on failure, continue
  - `open()` on /dev/urandom — exit(1) on failure
- EINTR handling: retry syscalls that return EINTR (use a loop)
- EAGAIN/EWOULDBLOCK: non-blocking socket operation — retry, do not treat as error

## Database Access
- No string interpolation into SQL queries — EVER
- Only `sqlite3_bind_text()`, `sqlite3_bind_int()`, `sqlite3_bind_int64()` for parameters
- `sqlite3_step()` return values checked: SQLITE_ROW, SQLITE_DONE, other = error
- `sqlite3_reset()` called after every prepared statement use
- `sqlite3_finalize()` called on all statements in db_close()

## Input Validation
- All JSON fields validated for presence and type before use
- device_id, username, action: validate length < 64, validate no SQL special chars (not needed if using bind, but still check length)
- Incoming payload size > 4096 bytes: disconnect client with close frame 1009
- Unknown message type: log, send INVALID_MESSAGE, do NOT disconnect (unless repeated)
- Malformed JSON: log, send INVALID_MESSAGE, disconnect

## Code Style
- Every function has a single-line comment explaining its purpose
- Every .h file has include guards: `#ifndef FILENAME_H` / `#define FILENAME_H` / `#endif`
- All error messages format: `fprintf(stderr, "[ERROR] %s: %s\n", __func__, strerror(errno));`
- All info messages format: `printf("[INFO] %s\n", message);`
- Max function length: 80 lines. If longer, split into helpers.
- No global mutable state except:
  - `SharedRegistry *shared_reg` — mmap'd shared memory pointer in main.c
  - `sem_t *shared_sem` — named semaphore pointer in main.c
  - `Device device_registry[MAX_DEVICES]` in device_manager.c
  - `sqlite3 *db` pointer in db.c
  - `volatile sig_atomic_t running` in main.c

## Compilation Requirements
- `make` must produce zero warnings with `-Wall -Wextra -Wpedantic`
- No `-Wno-*` flags to suppress warnings
- Debug symbols included: `-g` flag

## What Gemini CLI Must NOT Do
- Do not use epoll() or select() — concurrency is handled via fork() and pthreads as specified
- Do not add TLS/SSL to the WebSocket connection — plain WS only
