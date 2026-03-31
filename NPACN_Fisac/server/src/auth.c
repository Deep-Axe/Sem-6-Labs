#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <openssl/sha.h>

#include "auth.h"
#include "db.h"
#include "logger.h"

/* Hash password with SHA-256; returns heap-allocated 64-char hex string, caller must free() */
char *auth_hash_password(const char *password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)password, strlen(password), hash);

    char *out = malloc(65);
    if (!out) {
        fprintf(stderr, "[ERROR] auth_hash_password: malloc failed\n");
        return NULL;
    }
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        snprintf(out + (i * 2), 3, "%02x", hash[i]);
    }
    out[64] = '\0';
    return out;
}

/* Generate 32-byte random session token as 64-char hex string into out_token */
void auth_generate_token(char *out_token) {
    unsigned char buf[32];
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "[ERROR] auth_generate_token: cannot open /dev/urandom: %s\n", strerror(errno));
        exit(1);
    }
    ssize_t n = read(fd, buf, sizeof(buf));
    close(fd);
    if (n != (ssize_t)sizeof(buf)) {
        fprintf(stderr, "[ERROR] auth_generate_token: short read from /dev/urandom\n");
        exit(1);
    }
    for (int i = 0; i < 32; i++) {
        snprintf(out_token + (i * 2), 3, "%02x", buf[i]);
    }
    out_token[64] = '\0';
}

/* Validate token string against the session token stored in conn; returns 1 if valid, 0 if not */
int auth_validate_token(const Connection *conn, const char *token) {
    if (!conn || !token) return 0;
    return (strncmp(conn->session_token, token, 64) == 0) ? 1 : 0;
}

/* Full login flow: lookup user, compare hash, generate token; returns 0 on success, -1 on failure */
int auth_login(Connection *conn, const char *username, const char *password) {
    char stored_hash[65];
    if (db_get_user(username, stored_hash) != 1) return -1;

    char *input_hash = auth_hash_password(password);
    if (!input_hash) return -1;

    int match = (strcmp(stored_hash, input_hash) == 0);
    free(input_hash);

    if (match) {
        auth_generate_token(conn->session_token);
        strncpy(conn->username, username, sizeof(conn->username) - 1);
        conn->username[sizeof(conn->username) - 1] = '\0';
        conn->type  = CLIENT_WEB;
        conn->state = STATE_AUTHENTICATED;
        return 0;
    }
    return -1;
}
