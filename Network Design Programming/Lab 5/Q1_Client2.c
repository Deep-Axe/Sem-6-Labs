#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock_fd;
    struct sockaddr_in server_addr, local_addr;
    socklen_t addr_len = sizeof(local_addr);

    // --- Create socket ---
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // --- Connect to server ---
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    // Get own socket address
    getsockname(sock_fd, (struct sockaddr *)&local_addr, &addr_len);
    char local_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &local_addr.sin_addr, local_ip, INET_ADDRSTRLEN);
    int local_port = ntohs(local_addr.sin_port);

    printf("[Client2] Connected to server.\n");
    printf("[Client2] Local socket address: %s:%d\n", local_ip, local_port);

    // --- Send message with socket address ---
    const char *keyword = "Technology";
    char message[BUFFER_SIZE];
    snprintf(message, BUFFER_SIZE, "%s", keyword);

    send(sock_fd, message, strlen(message), 0);
    printf("[Client2] Sent: \"%s\"\n", message);

    // --- Wait for server response ---
    char buffer[BUFFER_SIZE] = {0};
    int bytes_read = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);

    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';

        if (strcmp(buffer, "terminate session") == 0) {
            printf("[Client2] Received TERMINATE from server. Closing.\n");
        } else {
            printf("[Client2] Server response: \"%s\"\n", buffer);
        }
    } else {
        printf("[Client2] Server disconnected.\n");
    }

    close(sock_fd);
    printf("[Client2] Connection closed.\n");

    return 0;
}