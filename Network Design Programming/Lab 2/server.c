#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAXSIZE 1024
#define PORT 3388

int cmp(const void *a, const void *b) {
    return *(char*)a - *(char*)b;
}

void handle_request(char *buff, char *filename, char *response) {
    char option = buff[0];
    char *data = buff + 2;

    if (option == '1') {
        FILE *fp = fopen(filename, "r");
        if (!fp) {
            strcpy(response, "Error opening file");
            return;
        }
        char content[10000] = {0};
        fread(content, 1, sizeof(content), fp);
        fclose(fp);

        int count = 0;
        char *ptr = content;
        while ((ptr = strstr(ptr, data)) != NULL) {
            count++;
            ptr++;
        }
        if (count)
            sprintf(response, "%d", count);
        else
            strcpy(response, "String not found");
    }

    else if (option == '2') {
        char s1[256], s2[256];
        sscanf(data, "%s %s", s1, s2);

        FILE *fp = fopen(filename, "r");
        if (!fp) {
            strcpy(response, "Error opening file");
            return;
        }

        char content[10000] = {0};
        fread(content, 1, sizeof(content), fp);
        fclose(fp);

        char *pos = strstr(content, s1);
        if (!pos) {
            strcpy(response, "String not found");
            return;
        }

        char newc[10000] = {0};
        char *src = content, *dst = newc;
        int l1 = strlen(s1), l2 = strlen(s2);

        while (*src) {
            if (strncmp(src, s1, l1) == 0) {
                strcpy(dst, s2);
                dst += l2;
                src += l1;
            } else {
                *dst++ = *src++;
            }
        }
        *dst = 0;

        fp = fopen(filename, "w");
        fwrite(newc, 1, strlen(newc), fp);
        fclose(fp);

        strcpy(response, "String replaced");
    }

    else if (option == '3') {
        FILE *fp = fopen(filename, "r");
        if (!fp) {
            strcpy(response, "Error opening file");
            return;
        }
        char content[10000] = {0};
        int len = fread(content, 1, sizeof(content), fp);
        fclose(fp);

        qsort(content, len, 1, cmp);

        fp = fopen(filename, "w");
        fwrite(content, 1, len, fp);
        fclose(fp);

        strcpy(response, "File reordered");
    }

    else {
        strcpy(response, "Invalid option");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s tcp|udp\n", argv[0]);
        exit(1);
    }

    int sockfd;
    struct sockaddr_in addr, cliaddr;
    socklen_t len = sizeof(cliaddr);
    char buff[MAXSIZE], filename[256], response[MAXSIZE];

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    /* ---------------- TCP ---------------- */
    if (strcmp(argv[1], "tcp") == 0) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
        listen(sockfd, 1);

        int connfd = accept(sockfd, NULL, NULL);

        recv(connfd, filename, sizeof(filename), 0);

        if (access(filename, F_OK) == -1) {
            send(connfd, "File not present", 16, 0);
            close(connfd);
            return 0;
        }

        send(connfd, "File present", 12, 0);

        while (1) {
            memset(buff, 0, MAXSIZE);
            recv(connfd, buff, MAXSIZE, 0);

            if (strcmp(buff, "4") == 0) {
                send(connfd, "Exiting", 7, 0);
                break;
            }

            memset(response, 0, MAXSIZE);
            handle_request(buff, filename, response);
            send(connfd, response, strlen(response), 0);
        }

        close(connfd);
    }

    /* ---------------- UDP ---------------- */
    else {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));

        recvfrom(sockfd, filename, sizeof(filename), 0,
                 (struct sockaddr*)&cliaddr, &len);

        if (access(filename, F_OK) == -1) {
            sendto(sockfd, "File not present", 16, 0,
                   (struct sockaddr*)&cliaddr, len);
            return 0;
        }

        sendto(sockfd, "File present", 12, 0,
               (struct sockaddr*)&cliaddr, len);

        while (1) {
            recvfrom(sockfd, buff, MAXSIZE, 0,
                     (struct sockaddr*)&cliaddr, &len);

            if (strcmp(buff, "4") == 0) {
                sendto(sockfd, "Exiting", 7, 0,
                       (struct sockaddr*)&cliaddr, len);
                break;
            }

            memset(response, 0, MAXSIZE);
            handle_request(buff, filename, response);

            sendto(sockfd, response, strlen(response), 0,
                   (struct sockaddr*)&cliaddr, len);
        }
    }

    close(sockfd);
    return 0;
}
