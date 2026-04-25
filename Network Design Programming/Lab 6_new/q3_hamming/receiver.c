#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1, addrlen = sizeof(address);
    char buffer[1024] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    printf("[Receiver] Waiting for codeword...\n");
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

    int n = recv(new_socket, buffer, 1024, 0);
    buffer[n] = '\0';
    printf("[Receiver] Received Codeword: %s\n", buffer);

    int code[n + 1];
    for (int i = 1; i <= n; i++) code[i] = buffer[i - 1] - '0';

    // Calculate Syndrome (error position)
    int syndrome = 0;
    int r = 0;
    while ((1 << r) < (n + 1)) r++;

    for (int i = 0; i < r; i++) {
        int pos = (1 << i);
        int parity_check = 0;
        for (int k = 1; k <= n; k++) {
            if (k & pos) {
                parity_check ^= code[k];
            }
        }
        if (parity_check) {
            syndrome += pos;
        }
    }

    if (syndrome == 0) {
        printf("[Receiver] No error detected.\n");
    } else {
        printf("[Receiver] Error detected at bit position: %d\n", syndrome);
        code[syndrome] = !code[syndrome];
        printf("[Receiver] Error corrected.\n");
    }

    printf("[Receiver] Corrected Codeword: ");
    for (int i = 1; i <= n; i++) printf("%d", code[i]);
    
    printf("\n[Receiver] Original Data: ");
    for (int i = 1; i <= n; i++) {
        if ((i & (i - 1)) != 0) {
            printf("%d", code[i]);
        }
    }
    printf("\n");

    close(new_socket);
    close(server_fd);
    return 0;
}
