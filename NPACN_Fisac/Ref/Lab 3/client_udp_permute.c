#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6000
#define MAX 1024

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    socklen_t len;
    char buffer[MAX];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    len = sizeof(servaddr);

    printf("Enter string: ");
    scanf("%s", buffer);

    /* Client sends first */
    sendto(sockfd, buffer, strlen(buffer), 0,
           (struct sockaddr*)&servaddr, len);

    /* Client waits for response */
    recvfrom(sockfd, buffer, MAX, 0,
             (struct sockaddr*)&servaddr, &len);

    printf("Server: %s\n", buffer);

    close(sockfd);
    return 0;
}
