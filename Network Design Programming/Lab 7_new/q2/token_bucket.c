#include <stdio.h>

int main() {
    double bucket_size = 50.0;    // b = 50 KB
    double replenishment_rate = 10.0; // r = 10 KBps
    double current_tokens = 50.0; // Starts full
    double packet_size = 15.0;    // 15 KB
    double interval = 0.5;        // 0.5 seconds
    double time = 0.0;

    printf("Token Bucket Simulation:\n");
    printf("- Bucket Size: %.1f KB\n", bucket_size);
    printf("- Replenishment Rate: %.1f KBps\n", replenishment_rate);
    printf("- Packet Size: %.1f KB arriving every %.1f seconds\n\n", packet_size, interval);

    printf("%-10s | %-15s | %-15s | %-15s\n", "Time (s)", "Tokens Before", "Tokens After", "Status");
    printf("--------------------------------------------------------------------------\n");

    for (int i = 1; i <= 6; i++) {
        time += interval;
        
        // Add tokens
        current_tokens += interval * replenishment_rate;
        if (current_tokens > bucket_size) current_tokens = bucket_size;

        double before = current_tokens;
        char *status;

        if (current_tokens >= packet_size) {
            current_tokens -= packet_size;
            status = "SENT";
        } else {
            status = "QUEUED";
        }

        printf("%-10.1f | %-15.1f | %-15.1f | %-15s\n", time, before, current_tokens, status);
        
        if (time == 1.5) {
            printf(">> [Q1] Tokens left after 1.5s: %.1f KB\n", current_tokens);
        }
    }

    printf("\n[Q2] Packets start to be queued at t = 2.5s because 10.0 < 15.0 tokens.\n");
    
    // Q3 calculation
    double R = 20.0;
    double r = 10.0;
    double b = 50.0;
    double S = b / (R - r);
    double burst_size = R * S;
    printf("[Q3] Max Rate R = 20 KBps. Burst duration S = b/(R-r) = 50/(20-10) = 5.0s.\n");
    printf("     Maximum burst size = R * S = 20 * 5 = %.1f KB.\n", burst_size);

    return 0;
}
