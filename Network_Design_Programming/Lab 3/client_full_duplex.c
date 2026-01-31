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
    char buff[MAX];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    printf("Connected to server\n");

    pid_t pid = fork();

    /* CHILD PROCESS → RECEIVER */
    if (pid == 0) {
        printf("\n[Client Child] PID=%d PPID=%d\n", getpid(), getppid());
        while (1) {
            memset(buff, 0, MAX);
            int n = read(sockfd, buff, MAX);
            if (n <= 0)
                break;
            printf("\nServer: %s\n", buff);
        }
        exit(0);
    }

    /* PARENT PROCESS → SENDER */
    else {
        printf("\n[Client Parent] PID=%d PPID=%d\n", getpid(), getppid());
        while (1) {
            memset(buff, 0, MAX);
            printf("Client: ");
            fgets(buff, MAX, stdin);
            buff[strcspn(buff, "\n")] = 0;
            write(sockfd, buff, strlen(buff));
        }
    }

    close(sockfd);
    return 0;
}
