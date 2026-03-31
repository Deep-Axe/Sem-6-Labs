#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

#include "websocket.h"
#include "logger.h"

#define WS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define RECV_BUF_SIZE 4096

/* Base64-encode `length` bytes of `input` using OpenSSL BIO; returns heap-allocated string, caller must free(). */
static char *base64_encode(const unsigned char *input, int length) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, input, length);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);

    char *b64text = malloc(bufferPtr->length + 1);
    if (b64text) {
        memcpy(b64text, bufferPtr->data, bufferPtr->length);
        b64text[bufferPtr->length] = '\0';
    }

    BIO_free_all(bio);
    return b64text;
}

/* Perform the RFC 6455 WebSocket upgrade handshake on fd; returns 0 on success, -1 on failure. */
int ws_handshake(int fd) {
    char buf[RECV_BUF_SIZE];
    ssize_t n = recv(fd, buf, sizeof(buf) - 1, 0);
    if (n <= 0) return -1;
    buf[n] = '\0';

    if (strstr(buf, "GET ") == NULL || strstr(buf, "Upgrade: websocket") == NULL) {
        return -1;
    }

    char *key_start = strstr(buf, "Sec-WebSocket-Key: ");
    if (!key_start) return -1;
    key_start += 19;
    char *key_end = strstr(key_start, "\r\n");
    if (!key_end) return -1;

    size_t key_len = key_end - key_start;
    char key[128];
    if (key_len >= sizeof(key)) return -1;
    strncpy(key, key_start, key_len);
    key[key_len] = '\0';

    char combined[256];
    snprintf(combined, sizeof(combined), "%s%s", key, WS_GUID);

    unsigned char sha1_res[20];
    SHA1((unsigned char *)combined, strlen(combined), sha1_res);

    char *accept_key = base64_encode(sha1_res, 20);
    if (!accept_key) return -1;

    char response[512];
    snprintf(response, sizeof(response),
             "HTTP/1.1 101 Switching Protocols\r\n"
             "Upgrade: websocket\r\n"
             "Connection: Upgrade\r\n"
             "Sec-WebSocket-Accept: %s\r\n\r\n",
             accept_key);

    free(accept_key);

    if (send(fd, response, strlen(response), 0) == -1) {
        LOG_SYSCALL_ERROR("send handshake response");
        return -1;
    }

    return 0;
}

/* Parse one WebSocket frame from buf; fills opcode, payload, and length.
 * Returns bytes consumed (advance buf by this amount), 0 if frame is incomplete, -1 on error. */
int ws_parse_frame(const unsigned char *buf, int buf_len,
                   unsigned char *out_payload, int *out_len, uint8_t *out_opcode) {
    if (buf_len < 2) return 0;

    uint8_t byte0 = buf[0];
    uint8_t byte1 = buf[1];

    *out_opcode = byte0 & 0x0F;
    int has_mask = (byte1 & 0x80) != 0;
    uint64_t payload_len = byte1 & 0x7F;
    int header_len = 2;

    if (payload_len == 126) {
        if (buf_len < 4) return 0;
        payload_len = (buf[2] << 8) | buf[3];
        header_len += 2;
    } else if (payload_len == 127) {
        if (buf_len < 10) return 0;
        payload_len = 0;
        for (int i = 0; i < 8; i++) {
            payload_len = (payload_len << 8) | buf[2 + i];
        }
        header_len += 8;
    }

    const unsigned char *masking_key = NULL;
    if (has_mask) {
        if (buf_len < header_len + 4) return 0;
        masking_key = &buf[header_len];
        header_len += 4;
    }

    if (buf_len < (int)(header_len + payload_len)) return 0;

    *out_len = (int)payload_len;
    const unsigned char *payload_start = &buf[header_len];

    if (has_mask) {
        for (uint64_t i = 0; i < payload_len; i++) {
            out_payload[i] = payload_start[i] ^ masking_key[i % 4];
        }
    } else {
        memcpy(out_payload, payload_start, (size_t)payload_len);
    }

    return header_len + (int)payload_len;
}

/* Build and send a single WebSocket frame with the given opcode; handles partial writes. */
static int ws_send_frame(int fd, uint8_t opcode, const unsigned char *payload, int len) {
    unsigned char frame[RECV_BUF_SIZE + 10]; /* Extra space for header */
    int header_len = 0;

    frame[header_len++] = 0x80 | (opcode & 0x0F);

    if (len <= 125) {
        frame[header_len++] = (uint8_t)len;
    } else if (len <= 65535) {
        frame[header_len++] = 126;
        frame[header_len++] = (len >> 8) & 0xFF;
        frame[header_len++] = len & 0xFF;
    } else {
        // 64-bit length not supported for simplicity in this project's scale
        return -1;
    }

    if (header_len + len > (int)sizeof(frame)) return -1;
    memcpy(frame + header_len, payload, len);

    int frame_len = header_len + len;
    int total = 0;
    while (total < frame_len) {
        ssize_t n = write(fd, frame + total, frame_len - total);
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                usleep(1000);
                continue;
            }
            LOG_SYSCALL_ERROR("write frame");
            return -1;
        }
        total += (int)n;
    }
    return 0;
}

/* Send a UTF-8 text frame (opcode 0x01) to fd; returns 0 on success, -1 on error. */
int ws_send_text(int fd, const char *payload, int len) {
    return ws_send_frame(fd, 0x01, (const unsigned char *)payload, len);
}

/* Send a close frame (opcode 0x08) with the given RFC 6455 status code. */
int ws_send_close(int fd, uint16_t code) {
    unsigned char payload[2];
    payload[0] = (code >> 8) & 0xFF;
    payload[1] = code & 0xFF;
    return ws_send_frame(fd, 0x08, payload, 2);
}

/* Send a pong frame (opcode 0x0A) echoing the given payload; used to respond to pings. */
int ws_send_pong(int fd, const char *payload, int len) {
    return ws_send_frame(fd, 0x0A, (const unsigned char *)payload, len);
}
