#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
#define BUCKET_SIZE 50.0      // b = 50 KB
#define REPLENISH_RATE 10.0   // r = 10 KBps
#define PACKET_SIZE 15.0      // 15 KB

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1, addrlen = sizeof(address);
    double current_tokens = BUCKET_SIZE; // Bucket starts full
    time_t last_time, current_time;
    int dummy_data;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    printf("[Receiver] Token Bucket Shaper Started...\n");
    printf("- Bucket Size: %.1f KB\n- Replenishment Rate: %.1f KBps\n\n", BUCKET_SIZE, REPLENISH_RATE);

    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    
    // Initial timestamp
    last_time = time(NULL);

    while (recv(new_socket, &dummy_data, sizeof(int), 0) > 0) {
        current_time = time(NULL);
        
        // 1. Calculate tokens added since last arrival
        double time_gap = difftime(current_time, last_time);
        double tokens_added = time_gap * REPLENISH_RATE;
        
        // 2. Add tokens to bucket (capped at BUCKET_SIZE)
        current_tokens += tokens_added;
        if (current_tokens > BUCKET_SIZE) current_tokens = BUCKET_SIZE;

        printf("----------------------------------------\n");
        printf("Packet Received! Time Gap: %.1fs | Tokens Added: %.1f\n", time_gap, tokens_added);
        printf("Tokens Before Consumption: %.1f\n", current_tokens);

        // 3. Check for sufficient tokens
        if (current_tokens >= PACKET_SIZE) {
            current_tokens -= PACKET_SIZE;
            printf("STATUS: SENT (Consumed %.1f tokens)\n", PACKET_SIZE);
            printf("Tokens Remaining: %.1f\n", current_tokens);
        } else {
            printf("STATUS: QUEUED (Insufficient tokens: %.1f < %.1f)\n", current_tokens, PACKET_SIZE);
            printf("Tokens Remaining: %.1f\n", current_tokens);
        }
        
        last_time = current_time;
    }

    printf("----------------------------------------\n");
    printf("[Receiver] Session closed. Final token count: %.1f KB.\n", current_tokens);

    close(new_socket);
    close(server_fd);
    return 0;
}
