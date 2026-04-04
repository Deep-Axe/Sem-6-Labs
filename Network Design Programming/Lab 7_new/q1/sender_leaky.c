#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define PACKET_SIZE 4

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    int arrival_times[] = {1, 2, 3, 5, 6, 8, 11, 12, 15, 16, 19};
    int n = sizeof(arrival_times) / sizeof(arrival_times[0]);
    int dummy_data = 1010; // 4-byte integer

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

    printf("[Sender] Starting bursty transmission...\n");

    int current_time = 0;
    for (int i = 0; i < n; i++) {
        int wait_time = arrival_times[i] - current_time;
        sleep(wait_time);
        current_time = arrival_times[i];

        printf("[Sender] Time %ds: Sending packet of %d bytes\n", current_time, PACKET_SIZE);
        send(sock, &dummy_data, sizeof(int), 0);
    }

    printf("[Sender] All packets sent. Closing.\n");
    close(sock);
    return 0;
}
