#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define PACKET_SIZE 15 // KB

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    int dummy_data = 1234;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }

    printf("[Sender] Starting transmission of %d KB packets every 0.5s...\n", PACKET_SIZE);

    for (int i = 1; i <= 6; i++) {
        usleep(500000); // Wait 0.5 seconds
        printf("[Sender] Time %.1fs: Sending packet %d (%d KB)\n", i * 0.5, i, PACKET_SIZE);
        send(sock, &dummy_data, sizeof(int), 0);
    }

    printf("[Sender] All packets sent. Closing.\n");
    close(sock);
    return 0;
}
