#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define TCP_PORT 6000
#define UDP_PORT 6001
#define MAX 1024

int main() {
    int choice, protocol;
    char buffer[MAX];
    char data[50];

    printf("Choose protocol (1-TCP, 2-UDP): ");
    scanf("%d",&protocol);

    printf("Enter option:\n1. Registration Number\n2. Name of Student\n3. Subject Code\n");
    scanf("%d",&choice);

    printf("Enter data: ");
    scanf("%s",data);

    snprintf(buffer,MAX,"%d %s",choice,data);

    if(protocol==1){
        // TCP
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in serv;
        serv.sin_family = AF_INET;
        serv.sin_port = htons(TCP_PORT);
        serv.sin_addr.s_addr = inet_addr("127.0.0.1");

        connect(sock, (struct sockaddr*)&serv, sizeof(serv));
        send(sock, buffer, strlen(buffer), 0);
        recv(sock, buffer, MAX, 0);
        printf("\nServer response:\n%s\n", buffer);
        close(sock);
    } else {
        // UDP
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        struct sockaddr_in serv;
        serv.sin_family = AF_INET;
        serv.sin_port = htons(UDP_PORT);
        serv.sin_addr.s_addr = inet_addr("127.0.0.1");

        sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&serv, sizeof(serv));
        socklen_t len = sizeof(serv);
        recvfrom(sock, buffer, MAX, 0, (struct sockaddr*)&serv, &len);
        printf("\nServer response:\n%s\n", buffer);
        close(sock);
    }

    return 0;
}
