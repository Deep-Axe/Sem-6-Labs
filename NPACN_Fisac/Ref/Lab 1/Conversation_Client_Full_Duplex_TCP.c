#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/select.h>

#define MAXSIZE 1024
#define SERVER_IP "172.18.175.104"  // change to server IP
#define SERVER_PORT 3388

int main() {
    char buff[MAXSIZE];
    int sockfd, sentbytes, recedbytes;
    struct sockaddr_in serveraddr;

    // 1️⃣ Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        return 1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERVER_PORT);
    serveraddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // 2️⃣ Connect to server
    if (connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("Connection failed");
        close(sockfd);
        return 1;
    }
    printf("[INFO] Connected to server %s:%d\n", SERVER_IP, SERVER_PORT);

    int pipefd[2];
    pipe(pipefd);

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        close(sockfd);
        return 1;
    }

    if (pid == 0) { // Child: receive messages
        close(pipefd[1]); // close write end
        fd_set readfds;
        int maxfd = sockfd > pipefd[0] ? sockfd : pipefd[0];

        while (1) {
            FD_ZERO(&readfds);
            FD_SET(sockfd, &readfds);
            FD_SET(pipefd[0], &readfds);

            select(maxfd + 1, &readfds, NULL, NULL, NULL);

            if (FD_ISSET(pipefd[0], &readfds)) break;

            if (FD_ISSET(sockfd, &readfds)) {
                memset(buff, 0, sizeof(buff));
                recedbytes = recv(sockfd, buff, sizeof(buff) - 1, 0);
                if (recedbytes <= 0) break; // disconnected
                buff[recedbytes] = '\0';
                printf("Server: %s", buff);
                fflush(stdout);

                if (strncmp(buff, "stop", 4) == 0) break;
            }
        }
        close(sockfd);
        close(pipefd[0]);
        exit(0);
    } else { // Parent: send messages
        close(pipefd[0]); // close read end
        while (1) {
            printf("Client: ");
            if (!fgets(buff, sizeof(buff), stdin)) break;

            size_t len = strlen(buff);
            if (buff[len - 1] != '\n') {
                buff[len] = '\n';
                buff[len + 1] = '\0';
                len++;
            }

            size_t total = 0;
            while (total < len) {
                int n = send(sockfd, buff + total, len - total, 0);
                if (n <= 0) {
                    perror("Send failed");
                    goto end;
                }
                total += n;
            }

            if (strncmp(buff, "stop", 4) == 0) break;
        }

end:
        close(sockfd);
        close(pipefd[1]);
        wait(NULL);
    }

    return 0;
}
