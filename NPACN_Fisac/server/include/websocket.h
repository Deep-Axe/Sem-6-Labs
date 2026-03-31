#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <stdint.h>
#include <stddef.h>

int ws_handshake(int fd);
int ws_parse_frame(const unsigned char *buf, int buf_len,
                   unsigned char *out_payload, int *out_len, uint8_t *out_opcode);
int ws_send_text(int fd, const char *payload, int len);
int ws_send_close(int fd, uint16_t code);
int ws_send_pong(int fd, const char *payload, int len);

#endif
