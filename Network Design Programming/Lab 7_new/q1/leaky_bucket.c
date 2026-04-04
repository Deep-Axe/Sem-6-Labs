#include <stdio.h>
#include <stdlib.h>

#define BUCKET_SIZE 10
#define OUTGOING_RATE 1
#define PACKET_SIZE 4

int main() {
    int arrival_times[] = {1, 2, 3, 5, 6, 8, 11, 12, 15, 16, 19};
    int n = sizeof(arrival_times) / sizeof(arrival_times[0]);

    int current_level = 0;
    int last_time = 0;

    printf("Scenario:\n");
    printf("- Bucket Size: %d bytes\n", BUCKET_SIZE);
    printf("- Outgoing Rate: %d byte/sec\n", OUTGOING_RATE);
    printf("- Packet Size: %d bytes\n\n", PACKET_SIZE);

    printf("%-10s | %-12s | %-12s | %-15s | %-10s\n", 
           "Time (s)", "Arrival", "Leaked (B)", "Bucket Level", "Status");
    printf("--------------------------------------------------------------------------\n");

    for (int i = 0; i < n; i++) {
        int current_time = arrival_times[i];
        
        // 1. Calculate how much leaked since the last event
        int time_gap = current_time - last_time;
        int leaked = time_gap * OUTGOING_RATE;
        
        // 2. Update bucket level after leaking
        current_level -= leaked;
        if (current_level < 0) current_level = 0;

        // 3. Try to add the new packet
        char *status;
        if (current_level + PACKET_SIZE <= BUCKET_SIZE) {
            current_level += PACKET_SIZE;
            status = "CONFORMING";
        } else {
            status = "NON-CONFORMING";
        }

        printf("%-10d | %-12d | %-12d | %-15d | %-10s\n", 
               current_time, PACKET_SIZE, leaked, current_level, status);
        
        last_time = current_time;
    }

    // Final drain
    printf("--------------------------------------------------------------------------\n");
    printf("Remaining data in bucket: %d bytes. Will drain in %d seconds.\n", 
           current_level, current_level / OUTGOING_RATE);

    return 0;
}
