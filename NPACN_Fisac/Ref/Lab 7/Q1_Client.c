// client.c - Banking Application Client
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char input[256];

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) { perror("Socket failed"); exit(1); }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address"); exit(1);
    }

    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed"); exit(1);
    }

    printf("===== Welcome to Banking Application =====\n");

    // Send username
    printf("Enter Username: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0'; // Remove newline
    send(sock_fd, input, strlen(input), 0);

    // Send password
    printf("Enter Password: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';
    send(sock_fd, input, strlen(input), 0);

    // Receive login response
    memset(buffer, 0, BUFFER_SIZE);
    recv(sock_fd, buffer, BUFFER_SIZE, 0);
    printf("\n[Server]: %s\n", buffer);

    if (strcmp(buffer, "Login Successful") != 0) {
        close(sock_fd);
        return 0;
    }

    // Banking menu loop
    while (1) {
        // Receive menu from server
        memset(buffer, 0, BUFFER_SIZE);
        recv(sock_fd, buffer, BUFFER_SIZE, 0);
        printf("%s", buffer);

        // Send choice
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';
        send(sock_fd, input, strlen(input), 0);

        char choice = input[0];

        if (choice == 'd' || choice == 'D') {
            // Receive goodbye message
            memset(buffer, 0, BUFFER_SIZE);
            recv(sock_fd, buffer, BUFFER_SIZE, 0);
            printf("[Server]: %s\n", buffer);
            break;
        }

        if (choice == 'a' || choice == 'A' || choice == 'b' || choice == 'B') {
            // Receive prompt for amount
            memset(buffer, 0, BUFFER_SIZE);
            recv(sock_fd, buffer, BUFFER_SIZE, 0);
            printf("[Server]: %s", buffer);

            // Send amount
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = '\0';
            send(sock_fd, input, strlen(input), 0);
        }

        // Receive transaction result
        memset(buffer, 0, BUFFER_SIZE);
        recv(sock_fd, buffer, BUFFER_SIZE, 0);
        printf("[Server]: %s\n", buffer);
    }

    close(sock_fd);
    printf("\nThank you for using our Banking Application!\n");
    return 0;
}
