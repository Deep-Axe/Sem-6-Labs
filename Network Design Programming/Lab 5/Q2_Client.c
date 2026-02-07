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

    printf("=============================================\n");
    printf("  ANAGRAM CHECKER CLIENT\n");
    printf("=============================================\n");
    printf("  Connected to server successfully!\n");
    printf("  Client socket address: %s:%d\n", local_ip, local_port);
    printf("=============================================\n\n");

    // --- Prompt user for two strings ---
    char str1[BUFFER_SIZE / 2], str2[BUFFER_SIZE / 2];

    printf("Enter first string  : ");
    fflush(stdout);
    fgets(str1, sizeof(str1), stdin);
    str1[strcspn(str1, "\r\n")] = '\0';  // Remove newline

    printf("Enter second string : ");
    fflush(stdout);
    fgets(str2, sizeof(str2), stdin);
    str2[strcspn(str2, "\r\n")] = '\0';  // Remove newline

    // --- Send both strings separated by newline, along with socket address ---
    char message[BUFFER_SIZE];
    snprintf(message, BUFFER_SIZE, "%s\n%s", str1, str2);

    send(sock_fd, message, strlen(message), 0);

    printf("\n[Client] Sent to server:\n");
    printf("         String 1       : \"%s\"\n", str1);
    printf("         String 2       : \"%s\"\n", str2);
    printf("         Socket Address : %s:%d\n", local_ip, local_port);
    printf("[Client] Waiting for server response...\n\n");

    // --- Receive response from server ---
    char buffer[BUFFER_SIZE] = {0};
    int bytes_read = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);

    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';

        printf("=============================================\n");
        printf("  SERVER RESPONSE\n");
        printf("=============================================\n");
        printf("  %s\n", buffer);
        printf("=============================================\n");
    } else {
        printf("[Client] Server disconnected without response.\n");
    }

    close(sock_fd);
    printf("\n[Client] Connection closed.\n");

    return 0;
}