#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <time.h>

#include "db.h"
#include "logger.h"
#include "cJSON.h"

/* Module-level SQLite handle and compiled prepared statements (all private to db.c). */
static sqlite3 *db = NULL;
static sqlite3_stmt *stmt_user_lookup = NULL;
static sqlite3_stmt *stmt_device_upsert = NULL;
static sqlite3_stmt *stmt_device_update = NULL;
static sqlite3_stmt *stmt_device_list = NULL;
static sqlite3_stmt *stmt_log_insert = NULL;
static sqlite3_stmt *stmt_log_fetch = NULL;

/* Open the DB file, run PRAGMAs, create tables if absent, seed default admin, compile all statements. */
int db_init(const char *db_path) {
    int rc = sqlite3_open(db_path, &db);
    if (rc != SQLITE_OK) {
        LOG_ERROR("Cannot open database");
        return -1;
    }

    const char *pragmas = 
        "PRAGMA journal_mode=WAL;"
        "PRAGMA foreign_keys=ON;"
        "PRAGMA synchronous=NORMAL;";
    
    char *err_msg = NULL;
    rc = sqlite3_exec(db, pragmas, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        LOG_ERROR(err_msg);
        sqlite3_free(err_msg);
        return -1;
    }

    const char *schema = 
        "CREATE TABLE IF NOT EXISTS users ("
        "    id           INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    username     TEXT    UNIQUE NOT NULL,"
        "    password_hash TEXT   NOT NULL,"
        "    created_at   INTEGER NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS devices ("
        "    device_id    TEXT    PRIMARY KEY,"
        "    device_type  TEXT    NOT NULL,"
        "    state        TEXT    NOT NULL DEFAULT 'offline',"
        "    last_seen    INTEGER NOT NULL DEFAULT 0"
        ");"
        "CREATE TABLE IF NOT EXISTS activity_log ("
        "    id           INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    timestamp    INTEGER NOT NULL,"
        "    username     TEXT    NOT NULL DEFAULT '',"
        "    device_id    TEXT    NOT NULL DEFAULT '',"
        "    action       TEXT    NOT NULL,"
        "    source_ip    TEXT    NOT NULL DEFAULT ''"
        ");";

    rc = sqlite3_exec(db, schema, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        LOG_ERROR(err_msg);
        sqlite3_free(err_msg);
        return -1;
    }

    /* Seed admin user if the users table is empty.
     * Uses sqlite3_bind_text so no values are interpolated into the SQL string. */
    sqlite3_stmt *stmt_count = NULL;
    sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM users", -1, &stmt_count, NULL);
    if (sqlite3_step(stmt_count) == SQLITE_ROW) {
        if (sqlite3_column_int(stmt_count, 0) == 0) {
            sqlite3_stmt *stmt_seed = NULL;
            const char *seed_sql =
                "INSERT INTO users (username, password_hash, created_at) "
                "VALUES (?, ?, strftime('%s', 'now'))";
            if (sqlite3_prepare_v2(db, seed_sql, -1, &stmt_seed, NULL) == SQLITE_OK) {
                /* SHA-256 of "admin123" — see DB_SCHEMA.md */
                sqlite3_bind_text(stmt_seed, 1,
                    "admin",
                    -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt_seed, 2,
                    "240be518fabd2724ddb6f04eeb1da5967448d7e831c08c8fa822809f74c720a9",
                    -1, SQLITE_STATIC);
                sqlite3_step(stmt_seed);
                sqlite3_finalize(stmt_seed);
                LOG_INFO("Admin user seeded");
            }
        }
    }
    sqlite3_finalize(stmt_count);


    /* Compile all prepared statements once at startup for reuse across queries. */
    sqlite3_prepare_v2(db, "SELECT password_hash FROM users WHERE username = ?", -1, &stmt_user_lookup, NULL);
    sqlite3_prepare_v2(db, "INSERT OR REPLACE INTO devices (device_id, device_type, state, last_seen) VALUES (?, ?, ?, ?)", -1, &stmt_device_upsert, NULL);
    sqlite3_prepare_v2(db, "UPDATE devices SET state = ?, last_seen = ? WHERE device_id = ?", -1, &stmt_device_update, NULL);
    sqlite3_prepare_v2(db, "SELECT device_id, device_type, state, last_seen FROM devices ORDER BY device_id", -1, &stmt_device_list, NULL);
    sqlite3_prepare_v2(db, "INSERT INTO activity_log (timestamp, username, device_id, action, source_ip) VALUES (?, ?, ?, ?, ?)", -1, &stmt_log_insert, NULL);
    sqlite3_prepare_v2(db, "SELECT timestamp, username, device_id, action, source_ip FROM activity_log ORDER BY id DESC LIMIT ?", -1, &stmt_log_fetch, NULL);

    LOG_INFO("Database initialized");
    return 0;
}

/* Finalise all prepared statements and close the database handle. */
void db_close(void) {
    sqlite3_finalize(stmt_user_lookup);
    sqlite3_finalize(stmt_device_upsert);
    sqlite3_finalize(stmt_device_update);
    sqlite3_finalize(stmt_device_list);
    sqlite3_finalize(stmt_log_insert);
    sqlite3_finalize(stmt_log_fetch);
    sqlite3_close(db);
    LOG_INFO("Database closed");
}

/* Look up a user by username; fills out_hash (char[65]) with SHA-256 hex. Returns 1 if found, 0 if not. */
int db_get_user(const char *username, char *out_hash) {
    sqlite3_bind_text(stmt_user_lookup, 1, username, -1, SQLITE_STATIC);
    int found = 0;
    if (sqlite3_step(stmt_user_lookup) == SQLITE_ROW) {
        const char *hash = (const char *)sqlite3_column_text(stmt_user_lookup, 0);
        if (hash) {
            strncpy(out_hash, hash, 64);
            out_hash[64] = '\0';
            found = 1;
        }
    }
    sqlite3_reset(stmt_user_lookup);
    return found;
}

/* Insert or replace a device row; returns 0 on success, -1 on error. */
int db_upsert_device(const char *device_id, const char *device_type, const char *state, time_t last_seen) {
    sqlite3_bind_text(stmt_device_upsert, 1, device_id, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt_device_upsert, 2, device_type, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt_device_upsert, 3, state, -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt_device_upsert, 4, (sqlite3_int64)last_seen);
    
    int rc = sqlite3_step(stmt_device_upsert);
    sqlite3_reset(stmt_device_upsert);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

/* Update state and last_seen for an existing device row; returns 0 on success, -1 on error. */
int db_update_device_state(const char *device_id, const char *state, time_t last_seen) {
    sqlite3_bind_text(stmt_device_update, 1, state, -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt_device_update, 2, (sqlite3_int64)last_seen);
    sqlite3_bind_text(stmt_device_update, 3, device_id, -1, SQLITE_STATIC);
    
    int rc = sqlite3_step(stmt_device_update);
    sqlite3_reset(stmt_device_update);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

/* Fetch all device rows as a JSON array string; caller must free(). Returns device count or -1. */
int db_get_all_devices(char **out_json) {
    cJSON *root = cJSON_CreateArray();
    int count = 0;
    while (sqlite3_step(stmt_device_list) == SQLITE_ROW) {
        cJSON *item = cJSON_CreateObject();
        cJSON_AddStringToObject(item, "device_id", (const char *)sqlite3_column_text(stmt_device_list, 0));
        cJSON_AddStringToObject(item, "device_type", (const char *)sqlite3_column_text(stmt_device_list, 1));
        cJSON_AddStringToObject(item, "state", (const char *)sqlite3_column_text(stmt_device_list, 2));
        cJSON_AddNumberToObject(item, "last_seen", (double)sqlite3_column_int64(stmt_device_list, 3));
        cJSON_AddItemToArray(root, item);
        count++;
    }
    sqlite3_reset(stmt_device_list);
    *out_json = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return count;
}

/* Insert one activity log row; returns 0 on success, -1 on error. */
int db_log_activity(const char *username, const char *device_id, const char *action, const char *source_ip) {
    sqlite3_bind_int64(stmt_log_insert, 1, (sqlite3_int64)time(NULL));
    sqlite3_bind_text(stmt_log_insert, 2, username ? username : "", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt_log_insert, 3, device_id ? device_id : "", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt_log_insert, 4, action, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt_log_insert, 5, source_ip ? source_ip : "", -1, SQLITE_STATIC);
    
    int rc = sqlite3_step(stmt_log_insert);
    sqlite3_reset(stmt_log_insert);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

/* Fetch the most recent `limit` activity log rows as a JSON array string; caller must free(). */
int db_get_activity_log(int limit, char **out_json) {
    sqlite3_bind_int(stmt_log_fetch, 1, limit);
    cJSON *root = cJSON_CreateArray();
    int count = 0;
    while (sqlite3_step(stmt_log_fetch) == SQLITE_ROW) {
        cJSON *item = cJSON_CreateObject();
        cJSON_AddNumberToObject(item, "timestamp", (double)sqlite3_column_int64(stmt_log_fetch, 0));
        cJSON_AddStringToObject(item, "username", (const char *)sqlite3_column_text(stmt_log_fetch, 1));
        cJSON_AddStringToObject(item, "device_id", (const char *)sqlite3_column_text(stmt_log_fetch, 2));
        cJSON_AddStringToObject(item, "action", (const char *)sqlite3_column_text(stmt_log_fetch, 3));
        cJSON_AddStringToObject(item, "source_ip", (const char *)sqlite3_column_text(stmt_log_fetch, 4));
        cJSON_AddItemToArray(root, item);
        count++;
    }
    sqlite3_reset(stmt_log_fetch);
    *out_json = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return count;
}
