#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include <time.h>
#include <sys/types.h>
#include <semaphore.h>

#define MAX_DEVICES 32
#define SHM_NAME "/smarthome_devices"
#define SEM_NAME "/smarthome_sem"

typedef struct {
    char device_id[64];
    char type[16];
    char state[16];
    char pending_command[16];
    time_t last_seen;
    int connected;      // 1 = connected, 0 = offline
    pid_t child_pid;    // pid of child process handling this device
} Device;

typedef struct {
    Device devices[MAX_DEVICES];
    int device_count;
} SharedRegistry;

extern SharedRegistry *shared_reg;
extern sem_t *shared_sem;

int dm_register_device(const char *device_id, const char *device_type,
                       const char *initial_state, pid_t child_pid);
Device *dm_find_device(const char *device_id);
int dm_update_state(const char *device_id, const char *state);
void dm_set_offline(const char *device_id);
char *dm_get_all_json(void);
void dm_heartbeat(const char *device_id, const char *state);

#endif
