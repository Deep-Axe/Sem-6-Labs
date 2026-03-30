#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
#include <arpa/inet.h>
#include<sys/wait.h>
#include<sys/select.h>
#define MAXSIZE 100

int main() {
    char buff[MAXSIZE];
    int sockfd, retval, i;
    int recedbytes, sentbytes;
    struct sockaddr_in serveraddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("\nSocket Creation Error");
        return 1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    retval = connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == -1) {
        printf("Connection error");
        return 1;
    }

    int pipefd[2];
    pipe(pipefd);
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        close(sockfd);
        return 1;
    }
    if (pid == 0) { // Child process: handle receiving
        close(pipefd[1]); // Close write end
        fd_set readfds;
        int maxfd = sockfd > pipefd[0] ? sockfd : pipefd[0];
        while (1) {
            FD_ZERO(&readfds);
            FD_SET(sockfd, &readfds);
            FD_SET(pipefd[0], &readfds);
            select(maxfd + 1, &readfds, NULL, NULL, NULL);
            if (FD_ISSET(pipefd[0], &readfds)) {
                break; // Stop signal from parent
            }
            if (FD_ISSET(sockfd, &readfds)) {
                memset(buff, '\0', sizeof(buff));
                recedbytes = recv(sockfd, buff, sizeof(buff), 0);
                if (recedbytes == -1) {
                    break;
                }
                if (strcmp(buff, "stop") == 0) {
                    write(pipefd[1], "stop", 4); // Signal parent to stop
                    break;
                }
                printf("Server response: %s\n", buff);
            }
        }
        close(sockfd);
        close(pipefd[0]);
        exit(0);
    } else { // Parent process: handle sending
        close(pipefd[0]); // Close read end
        while (1) {
            memset(buff, '\0', sizeof(buff));
            printf("Enter the text to check for palindrome: ");
            scanf("%s", buff);
            buff[strlen(buff)] = '\0';
            int s = strlen(buff) * sizeof(char);
            sentbytes = send(sockfd, buff, s, 0);
            if (sentbytes == -1) {
                break;
            }
            if (strcmp(buff, "stop") == 0) {
                write(pipefd[1], "stop", 4); // Signal child to stop
                break;
            }
        }
        close(sockfd);
        close(pipefd[1]);
        wait(NULL); // Wait for child
    }
    return 0;
}
