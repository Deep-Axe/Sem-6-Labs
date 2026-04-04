#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
#define BUCKET_SIZE 10
#define OUTGOING_RATE 1
#define PACKET_SIZE 4

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1, addrlen = sizeof(address);
    int current_level = 0;
    time_t last_time, current_time;
    int dummy_data;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    printf("[Receiver] Leaky Bucket Policer Started...\n");
    printf("- Bucket Size: %d bytes\n- Drain Rate: %d byte/sec\n\n", BUCKET_SIZE, OUTGOING_RATE);

    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    
    // Initialize the last time to current time
    last_time = time(NULL);

    while (recv(new_socket, &dummy_data, sizeof(int), 0) > 0) {
        current_time = time(NULL);
        
        // 1. Calculate leaked data
        int time_gap = (int)(current_time - last_time);
        int leaked = time_gap * OUTGOING_RATE;
        
        // 2. Leak the bucket
        current_level -= leaked;
        if (current_level < 0) current_level = 0;

        printf("----------------------------------------\n");
        printf("Packet Received! Time Gap: %ds | Leaked: %dB\n", time_gap, leaked);

        // 3. Conformity Check
        if (current_level + PACKET_SIZE <= BUCKET_SIZE) {
            current_level += PACKET_SIZE;
            printf("STATUS: CONFORMING\n");
            printf("Bucket Level updated: %d bytes\n", current_level);
        } else {
            printf("STATUS: NON-CONFORMING (Dropped!)\n");
            printf("Bucket Level remains: %d bytes\n", current_level);
        }
        
        last_time = current_time;
    }

    printf("----------------------------------------\n");
    printf("[Receiver] Sender disconnected. Final level: %d bytes.\n", current_level);

    close(new_socket);
    close(server_fd);
    return 0;
}
