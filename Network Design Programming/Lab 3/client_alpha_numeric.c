#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 5000
#define MAX 1024

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[MAX];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    printf("Input string: ");
    scanf("%s", buffer);

    send(sockfd, buffer, strlen(buffer), 0);

    /* Receive two messages (order may vary) */
    for (int i = 0; i < 2; i++) {
        memset(buffer, 0, MAX);
        recv(sockfd, buffer, MAX, 0);
        printf("\n%s\n", buffer);
    }

    close(sockfd);
    return 0;
}
