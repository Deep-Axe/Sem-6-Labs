/*
 * Token Bus Protocol - Client 3
 *
 * Client 3 is the final destination in the token ring. It listens for a token
 * from Client 2 (normal case) or directly from Client 1 (if Client 2 has failed).
 * It opens two listening ports to handle both scenarios.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>
#include <sys/time.h>

#define LISTEN_PORT   5002   /* Receives token from Client 1 (bypass) or Client 2 */
#define BUFFER_SIZE   256

int main(void)
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    printf("=============================================\n");
    printf(" Token Bus Protocol — Client 3 (Receiver)\n");
    printf("=============================================\n");

    /* --- Set up server socket --- */
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

    if (listen(server_fd, 2) < 0) {
        perror("listen");
        close(server_fd);
        return EXIT_FAILURE;
    }

    printf("[Client 3] Listening on port %d...\n", LISTEN_PORT);
    printf("[Client 3] Waiting for token from Client 2 (or Client 1 on bypass)...\n\n");

    /* --- Accept incoming connection --- */
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd < 0) {
        perror("accept");
        close(server_fd);
        return EXIT_FAILURE;
    }

    printf("[Client 3] Connection accepted from %s:%d\n",
           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    /* --- Receive the token --- */
    memset(buffer, 0, BUFFER_SIZE);
    bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_read <= 0) {
        printf("[Client 3] Failed to receive token.\n");
        close(client_fd);
        close(server_fd);
        return EXIT_FAILURE;
    }

    printf("[Client 3] Token received: \"%s\"\n", buffer);

    /* Identify the sender based on the token message */
    if (strstr(buffer, "CLIENT1") != NULL) {
        printf("[Client 3] Token came DIRECTLY from Client 1 (Client 2 was bypassed).\n");
    } else if (strstr(buffer, "CLIENT2") != NULL) {
        printf("[Client 3] Token came from Client 2 (normal relay path).\n");
    } else {
        printf("[Client 3] Token received from unknown source.\n");
    }

    printf("[Client 3] Token bus cycle complete.\n");
    printf("[Client 3] Done.\n");

    close(client_fd);
    close(server_fd);

    return EXIT_SUCCESS;
}