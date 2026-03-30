#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAXLINE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[MAXLINE] = {0};
    char hostname[256];

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address / Address not supported \n");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    printf("Enter hostname to resolve: ");
    scanf("%255s", hostname);

    send(sock, hostname, strlen(hostname), 0);
    memset(buffer, 0, MAXLINE);
    read(sock, buffer, MAXLINE);

    printf("Response from server: %s\n", buffer);

    close(sock);
    return 0;
}
