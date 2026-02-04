#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 5000
#define MAX 1024

/* Sort characters ascending */
void sort_asc(char *s) {
    int n = strlen(s);
    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (s[i] > s[j]) {
                char t = s[i];
                s[i] = s[j];
                s[j] = t;
            }
}

/* Sort characters descending */
void sort_desc(char *s) {
    int n = strlen(s);
    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (s[i] < s[j]) {
                char t = s[i];
                s[i] = s[j];
                s[j] = t;
            }
}

int main() {
    int sockfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    char buffer[MAX];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    listen(sockfd, 1);

    printf("Server waiting for client...\n");

    len = sizeof(cliaddr);
    connfd = accept(sockfd, (struct sockaddr*)&cliaddr, &len);

    recv(connfd, buffer, MAX, 0);
    printf("Received string from client: %s\n", buffer);

    pid_t pid = fork();

    /* -------- CHILD PROCESS -------- */
    if (pid == 0) {
        char digits[MAX] = {0};
        int k = 0;

        for (int i = 0; buffer[i]; i++)
            if (isdigit(buffer[i]))
                digits[k++] = buffer[i];

        sort_asc(digits);

        char msg[MAX];
        sprintf(msg,
                "Child Process PID=%d\nSorted digits (ascending): %s",
                getpid(), digits);

        send(connfd, msg, strlen(msg), 0);
        exit(0);
    }

    /* -------- PARENT PROCESS -------- */
    else {
        char chars[MAX] = {0};
        int k = 0;

        for (int i = 0; buffer[i]; i++)
            if (isalpha(buffer[i]))
                chars[k++] = buffer[i];

        sort_desc(chars);

        char msg[MAX];
        sprintf(msg,
                "Parent Process PID=%d\nSorted characters (descending): %s",
                getpid(), chars);

        send(connfd, msg, strlen(msg), 0);
    }

    close(connfd);
    close(sockfd);
    return 0;
}
