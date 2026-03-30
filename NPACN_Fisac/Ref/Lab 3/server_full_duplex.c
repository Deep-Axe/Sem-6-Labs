#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 5000
#define MAX 1024

int main() {
    int sockfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    char buff[MAX];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    listen(sockfd, 1);

    printf("Server waiting for connection...\n");
    len = sizeof(cliaddr);
    connfd = accept(sockfd, (struct sockaddr*)&cliaddr, &len);

    printf("Client connected\n");

    pid_t pid = fork();

    /* CHILD PROCESS → RECEIVER */
    if (pid == 0) {
        printf("\n[Server Child] PID=%d PPID=%d\n", getpid(), getppid());
        while (1) {
            memset(buff, 0, MAX);
            int n = read(connfd, buff, MAX);
            if (n <= 0)
                break;
            printf("\nClient: %s\n", buff);
        }
        exit(0);
    }

    /* PARENT PROCESS → SENDER */
    else {
        printf("\n[Server Parent] PID=%d PPID=%d\n", getpid(), getppid());
        while (1) {
            memset(buff, 0, MAX);
            printf("Server: ");
            fgets(buff, MAX, stdin);
            buff[strcspn(buff, "\n")] = 0;
            write(connfd, buff, strlen(buff));
        }
    }

    close(connfd);
    close(sockfd);
    return 0;
}
