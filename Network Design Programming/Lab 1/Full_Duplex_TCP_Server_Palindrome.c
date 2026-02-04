#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/wait.h>
#include<sys/select.h>
#define MAXSIZE 100

int is_palindrome(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        if (str[i] != str[len - i - 1]) {
            return 0;
        }
    }
    return 1;
}

main() {
    int sockfd, newsockfd, retval, i;
    socklen_t actuallen;
    int recedbytes, sentbytes;
    struct sockaddr_in serveraddr, clientaddr;
    char buff[MAXSIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("\nSocket creation error");
        return 1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    retval = bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == -1) {
        printf("Binding error");
        close(sockfd);
        return 1;
    }

    retval = listen(sockfd, 1);
    if (retval == -1) {
        close(sockfd);
        return 1;
    }
    actuallen = sizeof(clientaddr);
    newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &actuallen);
    if (newsockfd == -1) {
        close(sockfd);
        return 1;
    }

    int pipefd[2];
    pipe(pipefd);
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        close(sockfd);
        close(newsockfd);
        return 1;
    }
    if (pid == 0) { // Child process: handle receiving
        close(pipefd[1]); // Close write end
        close(sockfd); // Child doesn't need listening socket
        fd_set readfds;
        int maxfd = newsockfd > pipefd[0] ? newsockfd : pipefd[0];
        while (1) {
            FD_ZERO(&readfds);
            FD_SET(newsockfd, &readfds);
            FD_SET(pipefd[0], &readfds);
            select(maxfd + 1, &readfds, NULL, NULL, NULL);
            if (FD_ISSET(pipefd[0], &readfds)) {
                break; // Stop signal from parent
            }
            if (FD_ISSET(newsockfd, &readfds)) {
                memset(buff, '\0', sizeof(buff));
                recedbytes = recv(newsockfd, buff, sizeof(buff), 0);
                if (recedbytes == -1) {
                    break;
                }
                if (strcmp(buff, "stop") == 0) {
                    write(pipefd[1], "stop", 4); // Signal parent to stop
                    break;
                }
                printf("Received: %s\n", buff);
                char response[MAXSIZE];
                if (is_palindrome(buff)) {
                    strcpy(response, "Palindrome");
                } else {
                    strcpy(response, "Not Palindrome");
                }
                sentbytes = send(newsockfd, response, strlen(response), 0);
                if (sentbytes == -1) {
                    break;
                }
            }
        }
        close(newsockfd);
        close(pipefd[0]);
        exit(0);
    } else { // Parent process: handle sending
        close(pipefd[0]); // Close read end
        close(sockfd); // Parent doesn't need listening socket
        while (1) {
            memset(buff, '\0', sizeof(buff));
            scanf("%s", buff);
            buff[strlen(buff)] = '\0';
            int s = strlen(buff) * sizeof(char);
            sentbytes = send(newsockfd, buff, s, 0);
            if (sentbytes == -1) {
                break;
            }
            if (strcmp(buff, "stop") == 0) {
                write(pipefd[1], "stop", 4); // Signal child to stop
                break;
            }
        }
        close(newsockfd);
        close(pipefd[1]);
        wait(NULL); // Wait for child
    }
    close(sockfd);
    return 0;
}
