#ifndef DB_H
#define DB_H

#include <sqlite3.h>
#include <time.h>

int db_init(const char *db_path);
void db_close(void);
int db_get_user(const char *username, char *out_hash);
int db_upsert_device(const char *device_id, const char *device_type,
                     const char *state, time_t last_seen);
int db_update_device_state(const char *device_id, const char *state, time_t last_seen);
int db_get_all_devices(char **out_json);
int db_log_activity(const char *username, const char *device_id,
                    const char *action, const char *source_ip);
int db_get_activity_log(int limit, char **out_json);

#endif
