/*
 * Token Ring Protocol - Client 3
 *
 * Client 3 is the final node in the ring. It can ONLY receive the token
 * from Client 2 (its direct neighbor). It has NO knowledge of Client 1.
 * If Client 2 is down, Client 3 simply never receives the token —
 * there is no bypass path.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#define LISTEN_PORT  5002
#define BUFFER_SIZE  256

int main(void)
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    printf("=============================================\n");
    printf(" Token Ring Protocol — Client 3 (Receiver)\n");
    printf("=============================================\n");

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

    printf("[Client 3] Listening on port %d...\n", LISTEN_PORT);
    printf("[Client 3] Waiting for token from Client 2 (only neighbor)...\n\n");

    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd < 0) {
        perror("accept");
        close(server_fd);
        return EXIT_FAILURE;
    }

    printf("[Client 3] Connection accepted from %s:%d\n",
           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    memset(buffer, 0, BUFFER_SIZE);
    bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_read <= 0) {
        printf("[Client 3] Failed to receive token.\n");
        close(client_fd);
        close(server_fd);
        return EXIT_FAILURE;
    }

    /*
     * TOKEN RING: No sender identification needed.
     * The token can ONLY come from Client 2 (direct neighbor).
     * There is no bypass path from Client 1.
     */
    printf("[Client 3] Token received from Client 2: \"%s\"\n", buffer);
    printf("[Client 3] Token ring cycle complete.\n");
    printf("[Client 3] Done.\n");

    close(client_fd);
    close(server_fd);

    return EXIT_SUCCESS;
}