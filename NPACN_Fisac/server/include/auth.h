#ifndef AUTH_H
#define AUTH_H

#include "device_manager.h"
#include <arpa/inet.h>          /* INET_ADDRSTRLEN */

/* Connection type and state enumerations used by the message router. */
typedef enum { CLIENT_UNKNOWN, CLIENT_WEB, CLIENT_DEVICE } ClientType;
typedef enum { STATE_TCP_CONNECTED, STATE_WS_HANDSHAKE_DONE, STATE_AUTHENTICATED } ConnState;

typedef struct {
    int fd;
    ClientType type;
    ConnState state;
    char session_token[65];          /* 32-byte hex + null */
    char username[64];               /* for web clients */
    char device_id[64];              /* for iot devices */
    char recv_buf[4096];             /* partial read buffer */
    int  recv_len;                   /* bytes currently in recv_buf */
    char remote_ip[INET_ADDRSTRLEN]; /* dotted-decimal IPv4 address string */
    int  auth_failures;              /* consecutive failed logins — disconnect at 3 */
    int  close_conn;                 /* set to 1 by handle_message to request child teardown */
} Connection;

/* Hash password with SHA-256; returns heap-allocated 64-char hex string, caller must free() */
char *auth_hash_password(const char *password);

/* Generate 32-byte random session token as 64-char hex string into out_token (must be char[65]) */
void auth_generate_token(char *out_token);

/* Validate token string against the session token stored in conn; returns 1 if valid, 0 if not */
int auth_validate_token(const Connection *conn, const char *token);

/* Full login flow: lookup user, compare hash, generate token; returns 0 on success, -1 on failure */
int auth_login(Connection *conn, const char *username, const char *password);

#endif
