#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>

#include "device_manager.h"
#include "logger.h"
#include "cJSON.h"
#include "db.h"

/* Register a device in shared memory; updates existing entry if already present, inserts new one otherwise. */
int dm_register_device(const char *device_id, const char *device_type, const char *initial_state, pid_t child_pid) {
    if (!shared_sem || !shared_reg) return -1;

    sem_wait(shared_sem);
    
    // Check if already exists
    Device *existing = NULL;
    for (int i = 0; i < shared_reg->device_count; i++) {
        if (strcmp(shared_reg->devices[i].device_id, device_id) == 0) {
            existing = &shared_reg->devices[i];
            break;
        }
    }

    if (existing) {
        strncpy(existing->state, initial_state, 15);
        existing->state[15] = '\0';
        existing->last_seen = time(NULL);
        existing->connected = 1;
        existing->child_pid = child_pid;
    } else {
        if (shared_reg->device_count >= MAX_DEVICES) {
            sem_post(shared_sem);
            return -1;
        }
        Device *new_dev = &shared_reg->devices[shared_reg->device_count++];
        strncpy(new_dev->device_id, device_id, 63);
        new_dev->device_id[63] = '\0';
        strncpy(new_dev->type, device_type, 15);
        new_dev->type[15] = '\0';
        strncpy(new_dev->state, initial_state, 15);
        new_dev->state[15] = '\0';
        new_dev->last_seen = time(NULL);
        new_dev->connected = 1;
        new_dev->child_pid = child_pid;
    }

    sem_post(shared_sem);
    
    // Persist to DB
    db_upsert_device(device_id, device_type, initial_state, time(NULL));
    
    return 0;
}

/* Linear search of shared registry for device_id; returns pointer into shared memory.
 * CALLER MUST HOLD shared_sem before calling — the returned pointer is only valid
 * while the semaphore is held. Never dereference or store the pointer after sem_post. */
Device *dm_find_device(const char *device_id) {
    if (!shared_reg) return NULL;
    for (int i = 0; i < shared_reg->device_count; i++) {
        if (strcmp(shared_reg->devices[i].device_id, device_id) == 0) {
            return &shared_reg->devices[i];
        }
    }
    return NULL;
}

/* Update device state and last_seen in shared memory and persist to DB; returns 0 on success. */
int dm_update_state(const char *device_id, const char *state) {
    if (!shared_sem || !shared_reg) return -1;
    sem_wait(shared_sem);
    Device *dev = dm_find_device(device_id);
    if (dev) {
        strncpy(dev->state, state, 15);
        dev->state[15] = '\0';
        dev->last_seen = time(NULL);
    }
    sem_post(shared_sem);
    
    if (dev) {
        db_update_device_state(device_id, state, time(NULL));
        return 0;
    }
    return -1;
}

void dm_set_offline(const char *device_id) {
    /* Mark device as disconnected in shared registry, then persist to DB. */
    if (!shared_sem || !shared_reg) return;
    int found = 0;
    sem_wait(shared_sem);
    Device *dev = dm_find_device(device_id);
    if (dev) {
        dev->connected = 0;
        found = 1;
    }
    sem_post(shared_sem);
    if (found) {
        db_update_device_state(device_id, "offline", time(NULL));
    }
}

/* Build a JSON array string of all registered devices from shared memory; caller must free(). */
char *dm_get_all_json(void) {
    if (!shared_sem || !shared_reg) return NULL;
    sem_wait(shared_sem);
    cJSON *root = cJSON_CreateArray();
    for (int i = 0; i < shared_reg->device_count; i++) {
        cJSON *item = cJSON_CreateObject();
        cJSON_AddStringToObject(item, "device_id", shared_reg->devices[i].device_id);
        cJSON_AddStringToObject(item, "device_type", shared_reg->devices[i].type);
        cJSON_AddStringToObject(item, "state", shared_reg->devices[i].connected ? shared_reg->devices[i].state : "offline");
        cJSON_AddNumberToObject(item, "last_seen", (double)shared_reg->devices[i].last_seen);
        cJSON_AddItemToArray(root, item);
    }
    sem_post(shared_sem);
    char *json = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return json;
}

void dm_heartbeat(const char *device_id, const char *state) {
    /* Update last_seen and state in shared registry; copy fields to locals before
     * releasing the semaphore so the DB call reads consistent stack data only. */
    if (!shared_sem || !shared_reg) return;
    char local_state[16];
    time_t local_last_seen = 0;
    int found = 0;
    local_state[0] = '\0';
    sem_wait(shared_sem);
    Device *dev = dm_find_device(device_id);
    if (dev) {
        dev->last_seen = time(NULL);
        if (state) {
            strncpy(dev->state, state, 15);
            dev->state[15] = '\0';
        }
        dev->connected = 1;
        /* Capture fields under semaphore before releasing. */
        strncpy(local_state, dev->state, 15);
        local_state[15] = '\0';
        local_last_seen = dev->last_seen;
        found = 1;
    }
    sem_post(shared_sem);
    if (found) {
        db_update_device_state(device_id, local_state, local_last_seen);
    }
}

