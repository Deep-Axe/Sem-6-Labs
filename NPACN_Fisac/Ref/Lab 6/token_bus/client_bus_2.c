/*
 * Token Bus Protocol - Client 2
 *
 * Client 2 acts as an intermediate node. It waits for the token from Client 1,
 * holds it for 40 seconds (simulating processing), and then forwards it to
 * Client 3. If Client 3 is disconnected, it reports the failure.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/time.h>


#define LISTEN_PORT   5001
#define CLIENT3_PORT  5002
#define BUFFER_SIZE   256
#define HOLD_TIME_SEC 10
#define TIMEOUT_SEC   5

int main(int argc, char *argv[])
{
    int server_fd, client_fd, sockfd;
    struct sockaddr_in server_addr, client_addr, dest_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    struct timeval tv;
    const char *client3_ip;

    if (argc < 2) {
        printf("Usage: %s <Client3_IP>\n", argv[0]);
        printf("Example: %s 192.168.1.3\n", argv[0]);
        printf("  Use 127.0.0.1 if testing on a single machine.\n");
        return EXIT_FAILURE;
    }

    client3_ip = argv[1];

    printf("=============================================\n");
    printf("  Token Bus Protocol — Client 2 (Relay)\n");
    printf("=============================================\n");

    /* --- Set up server socket to receive token from Client 1 --- */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(LISTEN_PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        return EXIT_FAILURE;
    }

    if (listen(server_fd, 1) < 0) {
        perror("listen");
        close(server_fd);
        return EXIT_FAILURE;
    }

    printf("[Client 2] Listening on port %d...\n", LISTEN_PORT);
    printf("[Client 2] Waiting for token from Client 1...\n\n");

    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd < 0) {
        perror("accept");
        close(server_fd);
        return EXIT_FAILURE;
    }

    printf("[Client 2] Connected with Client 1 (%s).\n",
           inet_ntoa(client_addr.sin_addr));

    memset(buffer, 0, BUFFER_SIZE);
    bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_read <= 0) {
        printf("[Client 2] Failed to receive token.\n");
        close(client_fd);
        close(server_fd);
        return EXIT_FAILURE;
    }

    printf("[Client 2] Token received: \"%s\"\n", buffer);
    close(client_fd);
    close(server_fd);

    /* --- Hold the token for HOLD_TIME_SEC seconds --- */
    printf("[Client 2] Holding token for %d seconds...\n", HOLD_TIME_SEC);
    sleep(HOLD_TIME_SEC);
    printf("[Client 2] Hold time elapsed. Forwarding token to Client 3...\n\n");

    /* --- Forward token to Client 3 --- */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    tv.tv_sec  = TIMEOUT_SEC;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family      = AF_INET;
    dest_addr.sin_port        = htons(CLIENT3_PORT);
    dest_addr.sin_addr.s_addr = inet_addr(client3_ip);

    printf("[Client 2] Attempting to connect to Client 3 (%s:%d)...\n",
           client3_ip, CLIENT3_PORT);

    if (connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        printf("[Client 2] Client 3 is DISCONNECTED from the LAN.\n");
        printf("[Client 2] Token could not be forwarded. Terminating.\n");
        close(sockfd);
        return EXIT_FAILURE;
    }

    printf("[Client 2] Connected to Client 3.\n");

    const char *fwd_msg = "TOKEN_FROM_CLIENT2";
    send(sockfd, fwd_msg, strlen(fwd_msg), 0);
    printf("[Client 2] Token forwarded to Client 3.\n");
    printf("[Client 2] Done.\n");
    close(sockfd);

    return EXIT_SUCCESS;
}