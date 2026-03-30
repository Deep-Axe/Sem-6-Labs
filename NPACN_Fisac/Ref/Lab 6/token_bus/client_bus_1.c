/*
 * Token Bus Protocol - Client 1
 * 
 * Client 1 is the token originator. It attempts to send the token to Client 2.
 * If Client 2 is disconnected, it bypasses Client 2 and sends the token
 * directly to Client 3 (intermediate failure handling).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>

#define CLIENT2_PORT 5001
#define CLIENT3_PORT 5002
#define TOKEN_MSG    "TOKEN_FROM_CLIENT1"
#define BUFFER_SIZE  256
#define TIMEOUT_SEC  5

int try_connect_and_send(const char *ip, int port, const char *msg, const char *label)
{
    int sockfd;
    struct sockaddr_in addr;
    struct timeval tv;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    /* Set connection timeout */
    tv.tv_sec  = TIMEOUT_SEC;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    printf("[Client 1] Attempting to connect to %s (%s:%d)...\n", label, ip, port);

    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf("[Client 1] %s is DISCONNECTED from the LAN.\n", label);
        close(sockfd);
        return -1;
    }

    printf("[Client 1] Connected to %s.\n", label);
    send(sockfd, msg, strlen(msg), 0);
    printf("[Client 1] Token sent to %s.\n", label);
    close(sockfd);
    return 0;
}

int main(int argc, char *argv[])
{
    const char *client2_ip, *client3_ip;

    if (argc < 3) {
        printf("Usage: %s <Client2_IP> <Client3_IP>\n", argv[0]);
        printf("Example: %s 192.168.1.2 192.168.1.3\n", argv[0]);
        printf("  Use 127.0.0.1 for both if testing on a single machine.\n");
        return EXIT_FAILURE;
    }

    client2_ip = argv[1];
    client3_ip = argv[2];

    printf("=============================================\n");
    printf("   Token Bus Protocol — Client 1 (Sender)\n");
    printf("=============================================\n");
    printf("[Client 1] Token originated. Starting transmission...\n\n");

    /* Case 1 & 2: Try sending token to Client 2 first */
    if (try_connect_and_send(client2_ip, CLIENT2_PORT, TOKEN_MSG, "Client 2") == 0) {
        printf("[Client 1] Token successfully delivered to Client 2.\n");
        printf("[Client 1] Done.\n");
        return EXIT_SUCCESS;
    }

    /* Case 2: Client 2 failed — bypass and send directly to Client 3 */
    printf("\n[Client 1] Bypassing Client 2 (intermediate failure).\n");
    printf("[Client 1] Attempting to send token directly to Client 3...\n\n");

    if (try_connect_and_send(client3_ip, CLIENT3_PORT, TOKEN_MSG, "Client 3") == 0) {
        printf("[Client 1] Token successfully delivered to Client 3 (bypass).\n");
        printf("[Client 1] Done.\n");
        return EXIT_SUCCESS;
    }

    /* Case 4: Both Client 2 and Client 3 are disconnected */
    printf("\n[Client 1] Both Client 2 and Client 3 are DISCONNECTED.\n");
    printf("[Client 1] Token could not be delivered. Network segment failed.\n");
    return EXIT_FAILURE;
}
