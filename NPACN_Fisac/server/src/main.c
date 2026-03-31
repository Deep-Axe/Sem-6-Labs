#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>        /* TCP_NODELAY */
#include <arpa/inet.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>

#include "logger.h"
#include "websocket.h"
#include "device_manager.h"
#include "db.h"
#include "auth.h"
#include "cJSON.h"

#define DEFAULT_PORT 8080
#define DEFAULT_DB_PATH "smarthome.db"
#define BACKLOG 10
#define HEARTBEAT_INTERVAL 5
#define HEARTBEAT_TIMEOUT 30

volatile sig_atomic_t running = 1;
volatile sig_atomic_t command_received = 0;
SharedRegistry *shared_reg = NULL;
sem_t *shared_sem = NULL;
int db_pipe[2];

typedef struct {
    char username[64];
    char device_id[64];
    char action[64];
    char source_ip[16];
} LogEntry;

/* Set the global running flag to 0 on SIGINT/SIGTERM to begin graceful shutdown. */
void sigint_handler(int sig) {
    (void)sig;
    running = 0;
}

/* Reap all terminated child processes to prevent zombies. */
void sigchld_handler(int sig) {
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

/* Signal a waiting device child that a command has been placed in pending_command. */
void sigusr1_handler(int sig) {
    (void)sig;
    command_received = 1;
}

void *db_write_thread(void *arg) {
    /* Drain the log pipe and persist every entry; handles EINTR and pipe close. */
    (void)arg;
    LogEntry entry;
    while (running) {
        ssize_t n = read(db_pipe[0], &entry, sizeof(LogEntry));
        if (n == (ssize_t)sizeof(LogEntry)) {
            db_log_activity(entry.username, entry.device_id, entry.action, entry.source_ip);
        } else if (n == 0) {
            /* Pipe write-end closed — nothing more to read. */
            break;
        } else if (n < 0) {
            if (errno == EINTR) continue;   /* interrupted by signal, retry */
            LOG_SYSCALL_ERROR("db_write_thread read");
            break;
        }
    }
    return NULL;
}

/* Periodically scan shared registry and mark devices offline if last_seen exceeds HEARTBEAT_TIMEOUT. */
void *heartbeat_thread(void *arg) {
    (void)arg;
    while (running) {
        sleep(HEARTBEAT_INTERVAL);
        if (shared_sem && shared_reg) {
            sem_wait(shared_sem);
            time_t now = time(NULL);
            for (int i = 0; i < shared_reg->device_count; i++) {
                if (shared_reg->devices[i].connected && 
                    (now - shared_reg->devices[i].last_seen > HEARTBEAT_TIMEOUT)) {
                    shared_reg->devices[i].connected = 0;
                    char msg[128];
                    snprintf(msg, sizeof(msg), "Device %s timed out", shared_reg->devices[i].device_id);
                    LOG_INFO(msg);
                }
            }
            sem_post(shared_sem);
        }
    }
    return NULL;
}

/* Release all IPC resources: DB, semaphore, shared memory, and the log pipe. */
void cleanup() {
    LOG_INFO("Cleaning up and shutting down...");
    db_close();
    if (shared_sem) {
        sem_close(shared_sem);
        sem_unlink(SEM_NAME);
    }
    if (shared_reg) {
        munmap(shared_reg, sizeof(SharedRegistry));
        shm_unlink(SHM_NAME);
    }
    close(db_pipe[0]);
    close(db_pipe[1]);
}

/* Route an incoming JSON message from a connection to the appropriate handler based on type and auth state. */
void handle_message(Connection *conn, const char *payload, int len) {
    (void)len;
    cJSON *root = cJSON_Parse(payload);
    if (!root) {
        LOG_ERROR("Malformed JSON received");
        return;
    }

    cJSON *type = cJSON_GetObjectItem(root, "type");
    if (!cJSON_IsString(type)) {
        cJSON_Delete(root);
        return;
    }

    if (conn->state == STATE_WS_HANDSHAKE_DONE) {
        if (strcmp(type->valuestring, "auth") == 0) {
            cJSON *user = cJSON_GetObjectItem(root, "username");
            cJSON *pass = cJSON_GetObjectItem(root, "password");
            if (cJSON_IsString(user) && cJSON_IsString(pass)) {
                if (auth_login(conn, user->valuestring, pass->valuestring) == 0) {
                    conn->auth_failures = 0;
                    LOG_INFO("Auth successful");
                    cJSON *res = cJSON_CreateObject();
                    cJSON_AddStringToObject(res, "type", "auth_result");
                    cJSON_AddBoolToObject(res, "success", 1);
                    cJSON_AddStringToObject(res, "token", conn->session_token);
                    char *out = cJSON_PrintUnformatted(res);
                    if (out) { ws_send_text(conn->fd, out, strlen(out)); free(out); }
                    cJSON_Delete(res);
                } else {
                    conn->auth_failures++;
                    LOG_ERROR("Auth failed");
                    cJSON *res = cJSON_CreateObject();
                    cJSON_AddStringToObject(res, "type", "auth_result");
                    cJSON_AddBoolToObject(res, "success", 0);
                    if (conn->auth_failures >= 3) {
                        cJSON_AddStringToObject(res, "message", "Too many failed attempts — closing connection");
                        char *out = cJSON_PrintUnformatted(res);
                        if (out) { ws_send_text(conn->fd, out, strlen(out)); free(out); }
                        cJSON_Delete(res);
                        ws_send_close(conn->fd, 1008);
                        conn->close_conn = 1;   /* signal child loop to break */
                    } else {
                        cJSON_AddStringToObject(res, "message", "Invalid credentials");
                        char *out = cJSON_PrintUnformatted(res);
                        if (out) { ws_send_text(conn->fd, out, strlen(out)); free(out); }
                        cJSON_Delete(res);
                    }
                }
            }
        } else if (strcmp(type->valuestring, "register") == 0) {
            cJSON *dev_id = cJSON_GetObjectItem(root, "device_id");
            cJSON *dev_type = cJSON_GetObjectItem(root, "device_type");
            cJSON *init_state = cJSON_GetObjectItem(root, "initial_state");
            if (cJSON_IsString(dev_id) && cJSON_IsString(dev_type) && cJSON_IsString(init_state)) {
                if (dm_register_device(dev_id->valuestring, dev_type->valuestring, init_state->valuestring, getpid()) == 0) {
                    strncpy(conn->device_id, dev_id->valuestring, 63);
                    conn->type = CLIENT_DEVICE;
                    conn->state = STATE_AUTHENTICATED;
                    LOG_INFO("Device registered");
                    cJSON *res = cJSON_CreateObject();
                    cJSON_AddStringToObject(res, "type", "register_ack");
                    cJSON_AddBoolToObject(res, "success", 1);
                    cJSON_AddStringToObject(res, "device_id", dev_id->valuestring);
                    char *out = cJSON_PrintUnformatted(res);
                    ws_send_text(conn->fd, out, strlen(out));
                    free(out);
                    cJSON_Delete(res);
                }
            }
        }
    } else if (conn->state == STATE_AUTHENTICATED) {
        if (conn->type == CLIENT_WEB) {
            cJSON *token = cJSON_GetObjectItem(root, "token");
            if (!cJSON_IsString(token) || !auth_validate_token(conn, token->valuestring)) {
                LOG_ERROR("Invalid token");
                cJSON_Delete(root);
                return;
            }

            if (strcmp(type->valuestring, "get_devices") == 0) {
                char *json = dm_get_all_json();
                if (json) {
                    cJSON *res = cJSON_CreateObject();
                    cJSON_AddStringToObject(res, "type", "device_list");
                    cJSON_AddItemToObject(res, "devices", cJSON_Parse(json));
                    char *out = cJSON_PrintUnformatted(res);
                    ws_send_text(conn->fd, out, strlen(out));
                    free(out);
                    free(json);
                    cJSON_Delete(res);
                }
            } else if (strcmp(type->valuestring, "get_log") == 0) {
                char *json = NULL;
                db_get_activity_log(20, &json);
                if (json) {
                    cJSON *res = cJSON_CreateObject();
                    cJSON_AddStringToObject(res, "type", "activity_log");
                    cJSON_AddItemToObject(res, "entries", cJSON_Parse(json));
                    char *out = cJSON_PrintUnformatted(res);
                    ws_send_text(conn->fd, out, strlen(out));
                    free(out);
                    free(json);
                    cJSON_Delete(res);
                }
            } else if (strcmp(type->valuestring, "command") == 0) {
                cJSON *dev_id = cJSON_GetObjectItem(root, "device_id");
                cJSON *action = cJSON_GetObjectItem(root, "action");
                if (cJSON_IsString(dev_id) && cJSON_IsString(action)) {
                    LOG_INFO("Command received for device");
                    
                    LogEntry log;
                    strncpy(log.username, conn->username, 63);
                    log.username[63] = '\0';
                    strncpy(log.device_id, dev_id->valuestring, 63);
                    log.device_id[63] = '\0';
                    strncpy(log.action, action->valuestring, 63);
                    log.action[63] = '\0';
                    strncpy(log.source_ip, conn->remote_ip, 15);
                    log.source_ip[15] = '\0';
                    write(db_pipe[1], &log, sizeof(LogEntry));

                    sem_wait(shared_sem);
                    Device *dev = dm_find_device(dev_id->valuestring);
                    if (dev && dev->connected) {
                        strncpy(dev->pending_command, action->valuestring, 15);
                        dev->pending_command[15] = '\0';
                        kill(dev->child_pid, SIGUSR1);
                        sem_post(shared_sem);
                        
                        cJSON *res = cJSON_CreateObject();
                        cJSON_AddStringToObject(res, "type", "command_ack");
                        cJSON_AddBoolToObject(res, "success", 1);
                        cJSON_AddStringToObject(res, "device_id", dev_id->valuestring);
                        cJSON_AddStringToObject(res, "action", action->valuestring);
                        char *out = cJSON_PrintUnformatted(res);
                        ws_send_text(conn->fd, out, strlen(out));
                        free(out);
                        cJSON_Delete(res);
                    } else {
                        sem_post(shared_sem);
                        cJSON *res = cJSON_CreateObject();
                        cJSON_AddStringToObject(res, "type", "command_ack");
                        cJSON_AddBoolToObject(res, "success", 0);
                        cJSON_AddStringToObject(res, "message", "Device offline");
                        char *out = cJSON_PrintUnformatted(res);
                        ws_send_text(conn->fd, out, strlen(out));
                        free(out);
                        cJSON_Delete(res);
                    }
                }
            } else if (strcmp(type->valuestring, "pre_register_device") == 0) {
                /* Web admin pre-commissions a device; it shows as offline until it connects. */
                cJSON *dev_id   = cJSON_GetObjectItem(root, "device_id");
                cJSON *dev_type = cJSON_GetObjectItem(root, "device_type");
                cJSON *init_st  = cJSON_GetObjectItem(root, "initial_state");
                const char *state = (cJSON_IsString(init_st)) ? init_st->valuestring : "off";
                if (cJSON_IsString(dev_id) && cJSON_IsString(dev_type)) {
                    int rc = dm_register_device(dev_id->valuestring, dev_type->valuestring, state, 0);
                    cJSON *res = cJSON_CreateObject();
                    cJSON_AddStringToObject(res, "type", "register_ack");
                    cJSON_AddBoolToObject(res, "success", rc == 0);
                    cJSON_AddStringToObject(res, "device_id", dev_id->valuestring);
                    if (rc != 0) cJSON_AddStringToObject(res, "message", "Registration failed (registry full?)");
                    char *out = cJSON_PrintUnformatted(res);
                    if (out) { ws_send_text(conn->fd, out, strlen(out)); free(out); }
                    cJSON_Delete(res);
                }
            }
        } else if (conn->type == CLIENT_DEVICE) {
            if (strcmp(type->valuestring, "heartbeat") == 0) {
                cJSON *state = cJSON_GetObjectItem(root, "state");
                dm_heartbeat(conn->device_id, cJSON_IsString(state) ? state->valuestring : NULL);
            } else if (strcmp(type->valuestring, "state_update") == 0) {
                cJSON *state = cJSON_GetObjectItem(root, "state");
                if (cJSON_IsString(state)) {
                    dm_update_state(conn->device_id, state->valuestring);
                }
            }
        }
    }

    cJSON_Delete(root);
}

/* Entry point: parse args, initialise IPC/DB/socket, spawn threads, and run the accept loop. */
int main(int argc, char *argv[]) {
    int port = DEFAULT_PORT;
    const char *db_path = DEFAULT_DB_PATH;

    if (argc > 1) {
        port = atoi(argv[1]);
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--db") == 0 && i + 1 < argc) {
            db_path = argv[i + 1];
        }
    }

    signal(SIGINT, sigint_handler);
    signal(SIGTERM, sigint_handler);
    signal(SIGCHLD, sigchld_handler);

    // Shared memory setup
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        LOG_SYSCALL_ERROR("shm_open");
        exit(1);
    }
    if (ftruncate(shm_fd, sizeof(SharedRegistry)) == -1) {
        LOG_SYSCALL_ERROR("ftruncate");
        exit(1);
    }
    shared_reg = mmap(NULL, sizeof(SharedRegistry), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_reg == MAP_FAILED) {
        LOG_SYSCALL_ERROR("mmap");
        exit(1);
    }
    memset(shared_reg, 0, sizeof(SharedRegistry));

    // Pipe for DB logs
    if (pipe(db_pipe) == -1) {
        LOG_SYSCALL_ERROR("pipe");
        exit(1);
    }

    // Semaphore setup
    shared_sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (shared_sem == SEM_FAILED) {
        LOG_SYSCALL_ERROR("sem_open");
        exit(1);
    }

    // DB write thread
    pthread_t db_thread;
    if (pthread_create(&db_thread, NULL, db_write_thread, NULL) != 0) {
        LOG_SYSCALL_ERROR("pthread_create db_write");
        exit(1);
    }

    // Heartbeat thread setup
    pthread_t hb_thread;
    if (pthread_create(&hb_thread, NULL, heartbeat_thread, NULL) != 0) {
        LOG_SYSCALL_ERROR("pthread_create heartbeat");
        exit(1);
    }

    if (db_init(db_path) != 0) {
        LOG_ERROR("Failed to initialize database");
        exit(1);
    }

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        LOG_SYSCALL_ERROR("socket");
        exit(1);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        LOG_SYSCALL_ERROR("setsockopt SO_REUSEADDR");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        LOG_SYSCALL_ERROR("bind");
        close(server_fd);
        exit(1);
    }

    if (listen(server_fd, BACKLOG) == -1) {
        LOG_SYSCALL_ERROR("listen");
        close(server_fd);
        exit(1);
    }

    char info_msg[128];
    snprintf(info_msg, sizeof(info_msg), "Server listening on port %d using database %s", port, db_path);
    LOG_INFO(info_msg);

    while (running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

        if (client_fd == -1) {
            if (errno == EINTR) continue;
            LOG_SYSCALL_ERROR("accept");
            continue;
        }

        /* Apply socket options on the accepted client fd.
         * TCP_NODELAY: disable Nagle's algorithm for low-latency command delivery.
         * SO_KEEPALIVE: let the OS detect and clean up dead connections. */
        int flag = 1;
        setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY,  &flag, sizeof(flag));
        setsockopt(client_fd, SOL_SOCKET,  SO_KEEPALIVE, &flag, sizeof(flag));

        pid_t pid = fork();
        if (pid == -1) {
            LOG_SYSCALL_ERROR("fork");
            close(client_fd);
        } else if (pid == 0) {
            // Child process
            close(server_fd);
            signal(SIGUSR1, sigusr1_handler);

            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
            int client_port = ntohs(client_addr.sin_port);
            char conn_msg[128];
            snprintf(conn_msg, sizeof(conn_msg), "Child %d handling connection from %s:%d", getpid(), client_ip, client_port);
            LOG_INFO(conn_msg);

            Connection conn;
            memset(&conn, 0, sizeof(conn));
            conn.fd = client_fd;

            conn.state = STATE_TCP_CONNECTED;
            strncpy(conn.remote_ip, client_ip, 15);
            conn.remote_ip[15] = '\0';

            if (ws_handshake(client_fd) == 0) {
                conn.state = STATE_WS_HANDSHAKE_DONE;
                LOG_INFO("WebSocket handshake successful in child");
                
                unsigned char buf[4096];
                int recv_len = 0;
                int child_running = 1;
                while (running && child_running) {
                    if (command_received && conn.type == CLIENT_DEVICE) {
                        command_received = 0;
                        /* Copy pending_command out while holding the semaphore,
                         * then release before doing any I/O — avoids holding the
                         * semaphore across a potentially-blocking ws_send_text(). */
                        char local_cmd[16];
                        local_cmd[0] = '\0';
                        sem_wait(shared_sem);
                        Device *dev = dm_find_device(conn.device_id);
                        if (dev && dev->pending_command[0] != '\0') {
                            strncpy(local_cmd, dev->pending_command, 15);
                            local_cmd[15] = '\0';
                            dev->pending_command[0] = '\0'; /* clear under sem */
                        }
                        sem_post(shared_sem);
                        if (local_cmd[0] != '\0') {
                            cJSON *cmd = cJSON_CreateObject();
                            cJSON_AddStringToObject(cmd, "type", "command");
                            cJSON_AddStringToObject(cmd, "action", local_cmd);
                            char *out = cJSON_PrintUnformatted(cmd);
                            if (out) {
                                ws_send_text(conn.fd, out, strlen(out));
                                free(out);
                            }
                            cJSON_Delete(cmd);
                        }
                    }

                    /* Use select with 1-second timeout to allow SIGUSR1 to interrupt. */
                    struct timeval tv = {1, 0};
                    fd_set fds;
                    FD_ZERO(&fds);
                    FD_SET(client_fd, &fds);
                    int sel = select(client_fd + 1, &fds, NULL, NULL, &tv);

                    if (sel > 0) {
                        ssize_t n = recv(client_fd, buf + recv_len, sizeof(buf) - recv_len, 0);
                        if (n <= 0) break;
                        recv_len += (int)n;

                        unsigned char payload[4096];
                        int payload_len;
                        uint8_t opcode;
                        int consumed;
                        while ((consumed = ws_parse_frame(buf, recv_len, payload, &payload_len, &opcode)) > 0) {
                            /* Oversized payload — RFC 6455 §7.4.1 code 1009 */
                            if (payload_len > 4096) {
                                ws_send_close(conn.fd, 1009);
                                child_running = 0;
                                break;
                            }
                            if (opcode == 0x8) {
                                /* Close frame — echo back and exit child cleanly */
                                ws_send_close(conn.fd, 1000);
                                child_running = 0;
                                break;
                            } else if (opcode == 0x9) {
                                /* Ping — must respond with Pong carrying same payload */
                                ws_send_pong(conn.fd, (const char *)payload, payload_len);
                            } else if (opcode == 0xA) {
                                /* Pong — unsolicited, silently ignore */
                            } else if (opcode == 0x1) {
                                /* Text frame — dispatch to message router */
                                payload[payload_len] = '\0';
                                handle_message(&conn, (char *)payload, payload_len);
                                if (conn.close_conn) {
                                    child_running = 0;
                                    break;
                                }
                            } else {
                                /* Binary (0x2) or any unknown opcode — reject per RFC 6455 §7.4.1 code 1003 */
                                ws_send_close(conn.fd, 1003);
                                child_running = 0;
                                break;
                            }
                            memmove(buf, buf + consumed, recv_len - consumed);
                            recv_len -= consumed;
                        }
                    } else if (sel < 0 && errno != EINTR) {
                        break;
                    }
                    if (!running) break;
                }
            } else {
                LOG_ERROR("WebSocket handshake failed in child");
            }
            
            if (conn.type == CLIENT_DEVICE) {
                dm_set_offline(conn.device_id);
            }
            close(client_fd);
            exit(0);
        } else {
            // Parent process
            close(client_fd);
        }
    }

    cleanup();
    close(server_fd);
    return 0;
}
