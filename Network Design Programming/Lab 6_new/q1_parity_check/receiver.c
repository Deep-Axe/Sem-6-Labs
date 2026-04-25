#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    int parity_type;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("[Receiver] Waiting for codeword...\n");

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Receive parity type and codeword
    recv(new_socket, &parity_type, sizeof(int), 0);
    int valread = recv(new_socket, buffer, BUFFER_SIZE, 0);
    buffer[valread] = '\0';

    int ones_count = 0;
    for (int i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] == '1') ones_count++;
    }

    printf("[Receiver] Received Codeword: %s\n", buffer);
    printf("[Receiver] Parity Type Used: %s\n", (parity_type == 0) ? "Even" : "Odd");
    printf("[Receiver] Number of 1s in Codeword: %d\n", ones_count);

    if (parity_type == 0) { // Even Parity Case
        if (ones_count % 2 == 0) {
            printf("[Receiver] Check Passed: No Error Detected (Even number of 1s).\n");
        } else {
            printf("[Receiver] Check Failed: Error Detected!\n");
        }
    } else { // Odd Parity Case
        if (ones_count % 2 != 0) {
            printf("[Receiver] Check Passed: No Error Detected (Odd number of 1s).\n");
        } else {
            printf("[Receiver] Check Failed: Error Detected!\n");
        }
    }

    close(new_socket);
    close(server_fd);
    return 0;
}
