/*
 * Token Ring Protocol - Client 1
 *
 * Client 1 is the token originator. It can ONLY send the token to Client 2
 * (its direct neighbor in the ring). If Client 2 is down, the ring is broken
 * and the token is lost. There is NO bypass mechanism.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/time.h>
#define CLIENT2_PORT 5001
#define BUFFER_SIZE  256
#define TIMEOUT_SEC  5

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in addr;
    struct timeval tv;
    const char *client2_ip;
    const char *token_msg = "TOKEN_FROM_CLIENT1";

    if (argc < 2) {
        printf("Usage: %s <Client2_IP>\n", argv[0]);
        printf("Example: %s 192.168.1.2\n", argv[0]);
        printf("  Use 127.0.0.1 if testing on a single machine.\n");
        return EXIT_FAILURE;
    }

    client2_ip = argv[1];

    printf("=============================================\n");
    printf("  Token Ring Protocol — Client 1 (Sender)\n");
    printf("=============================================\n");
    printf("[Client 1] Token originated. Starting transmission...\n\n");

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    tv.tv_sec  = TIMEOUT_SEC;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(CLIENT2_PORT);
    addr.sin_addr.s_addr = inet_addr(client2_ip);

    printf("[Client 1] Attempting to connect to Client 2 (%s:%d)...\n",
           client2_ip, CLIENT2_PORT);

    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        /*
         * TOKEN RING: No bypass! If the next neighbor is down,
         * the ring is broken and the token is lost.
         */
        printf("[Client 1] Client 2 is DISCONNECTED.\n");
        printf("[Client 1] RING IS BROKEN — token cannot be forwarded.\n");
        printf("[Client 1] Token LOST.\n");
        close(sockfd);
        return EXIT_FAILURE;
    }

    printf("[Client 1] Connected to Client 2.\n");
    send(sockfd, token_msg, strlen(token_msg), 0);
    printf("[Client 1] Token sent to Client 2.\n");
    printf("[Client 1] Done.\n");
    close(sockfd);

    return EXIT_SUCCESS;
}
