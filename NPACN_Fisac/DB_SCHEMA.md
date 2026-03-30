# DB_SCHEMA.md — Database Design (SQLite)

## Database File
- Filename: `smarthome.db`
- Location: same directory as server binary (i.e., `server/smarthome.db`)
- Created automatically on first run via `sqlite3_open()`
- Path configurable via command-line argument `--db <path>`

---

## Initialization
Run these statements at startup in this exact order:

```sql
PRAGMA journal_mode=WAL;
PRAGMA foreign_keys=ON;
PRAGMA synchronous=NORMAL;
```

`WAL` mode allows concurrent reads during writes — important for a server that reads and writes frequently.

---

## Table Definitions (run at startup, IF NOT EXISTS)

### users
```sql
CREATE TABLE IF NOT EXISTS users (
    id           INTEGER PRIMARY KEY AUTOINCREMENT,
    username     TEXT    UNIQUE NOT NULL,
    password_hash TEXT   NOT NULL,
    created_at   INTEGER NOT NULL
);
```

### devices
```sql
CREATE TABLE IF NOT EXISTS devices (
    device_id    TEXT    PRIMARY KEY,
    device_type  TEXT    NOT NULL,
    state        TEXT    NOT NULL DEFAULT 'offline',
    last_seen    INTEGER NOT NULL DEFAULT 0
);
```

### activity_log
```sql
CREATE TABLE IF NOT EXISTS activity_log (
    id           INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp    INTEGER NOT NULL,
    username     TEXT    NOT NULL DEFAULT '',
    device_id    TEXT    NOT NULL DEFAULT '',
    action       TEXT    NOT NULL,
    source_ip    TEXT    NOT NULL DEFAULT ''
);
```

### sessions (in-memory only — do NOT persist to DB)
Session tokens live only in the `Connection` struct in RAM.
They are intentionally not stored in DB — they expire when connection closes.

---

## Seed Data (insert at startup if users table is empty)

```sql
-- Check: SELECT COUNT(*) FROM users
-- If 0, insert default admin:
INSERT INTO users (username, password_hash, created_at)
VALUES (
    'admin',
    '240be518fabd2724ddb6f04eeb1da5967448d7e831c08c8fa822809f74c720a9',
    strftime('%s', 'now')
);
```
*(Hash above is SHA-256 of "admin123")*

---

## Prepared Statements (compile once at startup)

Define all of these in `db.h` as `sqlite3_stmt*` globals, compile in `db_init()`:

```c
// User lookup
"SELECT password_hash FROM users WHERE username = ?"

// Device upsert (insert or replace)
"INSERT OR REPLACE INTO devices (device_id, device_type, state, last_seen) VALUES (?, ?, ?, ?)"

// Device state update
"UPDATE devices SET state = ?, last_seen = ? WHERE device_id = ?"

// Device list (all)
"SELECT device_id, device_type, state, last_seen FROM devices ORDER BY device_id"

// Activity log insert
"INSERT INTO activity_log (timestamp, username, device_id, action, source_ip) VALUES (?, ?, ?, ?, ?)"

// Activity log fetch (last N)
"SELECT timestamp, username, device_id, action, source_ip FROM activity_log ORDER BY id DESC LIMIT ?"
```

---

## Access Functions in db.c

```c
// Initialize DB: open file, run PRAGMAs, create tables, seed data, compile statements
int db_init(const char *db_path);

// Shutdown: finalize all prepared statements, close DB
void db_close(void);

// Lookup user — returns 1 if found, 0 if not found, -1 on error
// Fills out_hash (must be char[65])
int db_get_user(const char *username, char *out_hash);

// Upsert device — returns 0 on success, -1 on error
int db_upsert_device(const char *device_id, const char *device_type,
                     const char *state, time_t last_seen);

// Update device state only — returns 0 on success, -1 on error
int db_update_device_state(const char *device_id, const char *state, time_t last_seen);

// Get all devices — fills out_json with JSON array string, caller must free()
// Returns number of devices, -1 on error
int db_get_all_devices(char **out_json);

// Log activity — returns 0 on success, -1 on error
int db_log_activity(const char *username, const char *device_id,
                    const char *action, const char *source_ip);

// Get activity log — fills out_json with JSON array string, caller must free()
// Returns number of entries, -1 on error
int db_get_activity_log(int limit, char **out_json);
```

---

## Synchronization Analysis

### Problem
SQLite with WAL mode allows one writer at a time. Since the server is single-threaded (select() loop), there is **no concurrent write contention** — writes are always sequential. This is an architectural advantage of the single-threaded model.

### DB Write Latency Impact
- `db_log_activity()` is called on every command — must be fast
- SQLite WAL write for a single row: ~0.1–0.5ms on local disk
- This is acceptable; real-time responsiveness is dominated by network RTT (~1–50ms)
- If DB write fails: log error to stderr, do not block or retry in hot path

### What NOT to do
- Do not call `db_get_all_devices()` on every heartbeat (expensive SELECT)
- Use in-memory device_registry as authoritative source for real-time queries
- Only write to DB for persistence; read from DB only at startup and for log queries
