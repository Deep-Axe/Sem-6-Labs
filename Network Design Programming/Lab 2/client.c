
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAXSIZE 1024
#define PORT 3388

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s tcp|udp\n", argv[0]);
        exit(1);
    }

    int sockfd;
    struct sockaddr_in servaddr;
    socklen_t len = sizeof(servaddr);
    char buff[MAXSIZE], filename[256];

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Enter filename: ");
    scanf("%s", filename);

    /* ---------------- TCP ---------------- */
    if (strcmp(argv[1], "tcp") == 0) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

        send(sockfd, filename, strlen(filename), 0);
        recv(sockfd, buff, MAXSIZE, 0);
        printf("%s\n", buff);

        if (strcmp(buff, "File not present") == 0)
            exit(0);

        while (1) {
            int ch;
            char s1[256], s2[256];
            printf("\n1.Search 2.Replace 3.Reorder 4.Exit\n");
            scanf("%d", &ch);

            if (ch == 1) {
                scanf("%s", s1);
                sprintf(buff, "1 %s", s1);
            } else if (ch == 2) {
                scanf("%s %s", s1, s2);
                sprintf(buff, "2 %s %s", s1, s2);
            } else if (ch == 3) {
                strcpy(buff, "3");
            } else {
                strcpy(buff, "4");
            }

            send(sockfd, buff, strlen(buff), 0);
            recv(sockfd, buff, MAXSIZE, 0);
            printf("Server: %s\n", buff);

            if (ch == 4) break;
        }
    }

    /* ---------------- UDP ---------------- */
    else {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);

        sendto(sockfd, filename, strlen(filename), 0,
               (struct sockaddr*)&servaddr, len);

        recvfrom(sockfd, buff, MAXSIZE, 0,
                 (struct sockaddr*)&servaddr, &len);
        printf("%s\n", buff);

        if (strcmp(buff, "File not present") == 0)
            exit(0);

        while (1) {
            int ch;
            char s1[256], s2[256];
            printf("\n1.Search 2.Replace 3.Reorder 4.Exit\n");
            scanf("%d", &ch);

            if (ch == 1) {
                scanf("%s", s1);
                sprintf(buff, "1 %s", s1);
            } else if (ch == 2) {
                scanf("%s %s", s1, s2);
                sprintf(buff, "2 %s %s", s1, s2);
            } else if (ch == 3) {
                strcpy(buff, "3");
            } else {
                strcpy(buff, "4");
            }

            sendto(sockfd, buff, strlen(buff), 0,
                   (struct sockaddr*)&servaddr, len);

            recvfrom(sockfd, buff, MAXSIZE, 0,
                     (struct sockaddr*)&servaddr, &len);
            printf("Server: %s\n", buff);

            if (ch == 4) break;
        }
    }

    close(sockfd);
    return 0;
}
