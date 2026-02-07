#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock_fd;
    struct sockaddr_in server_addr, local_addr;
    socklen_t addr_len = sizeof(local_addr);

    // --- Create socket ---
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // --- Connect to server ---
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    // Get own socket address
    getsockname(sock_fd, (struct sockaddr *)&local_addr, &addr_len);
    char local_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &local_addr.sin_addr, local_ip, INET_ADDRSTRLEN);
    int local_port = ntohs(local_addr.sin_port);

    printf("╔══════════════════════════════════════════════╗\n");
    printf("║     TRAVEL TICKET RESERVATION CLIENT        ║\n");
    printf("╚══════════════════════════════════════════════╝\n");
    printf("  Connected to server!\n");
    printf("  Client socket address: %s:%d\n\n", local_ip, local_port);

    // --- Step 1: Receive route menu from server ---
    char buffer[BUFFER_SIZE] = {0};
    int bytes_read = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);

    if (bytes_read <= 0) {
        printf("[Client] Server disconnected.\n");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    buffer[bytes_read] = '\0';

    // --- Step 2: Parse and display routes ---
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                    AVAILABLE ROUTES                         ║\n");
    printf("╠════╦══════════════╦══════════════╦═══════════╦══════════════╣\n");
    printf("║ ID ║    Source    ║ Destination  ║ Available ║    Booked    ║\n");
    printf("╠════╬══════════════╬══════════════╬═══════════╬══════════════╣\n");

    // Parse line by line: format "id|source|destination|available|booked"
    char *line = strtok(buffer, "\n");
    while (line != NULL) {
        if (strcmp(line, "ROUTES_BEGIN") == 0 || strcmp(line, "ROUTES_END") == 0) {
            line = strtok(NULL, "\n");
            continue;
        }

        int id, available, booked;
        char source[64], destination[64];

        if (sscanf(line, "%d|%[^|]|%[^|]|%d|%d",
                   &id, source, destination, &available, &booked) == 5) {
            printf("║ %2d ║ %-12s ║ %-12s ║    %3d    ║     %3d      ║\n",
                   id, source, destination, available, booked);
        }

        line = strtok(NULL, "\n");
    }

    printf("╚════╩══════════════╩══════════════╩═══════════╩══════════════╝\n\n");

    // --- Step 3: Get user input ---
    int route_choice, num_seats;

    printf("Enter Route ID to book  : ");
    fflush(stdout);
    scanf("%d", &route_choice);

    printf("Enter number of seats   : ");
    fflush(stdout);
    scanf("%d", &num_seats);

    // --- Step 4: Send booking request ---
    char request[BUFFER_SIZE];
    snprintf(request, BUFFER_SIZE, "%d|%d", route_choice, num_seats);

    send(sock_fd, request, strlen(request), 0);

    printf("\n[Client] Booking request sent.\n");
    printf("         Route          : %d\n", route_choice);
    printf("         Seats requested: %d\n", num_seats);
    printf("         Socket address : %s:%d\n", local_ip, local_port);
    printf("[Client] Waiting for server response...\n\n");

    // --- Step 5: Receive and display result ---
    memset(buffer, 0, BUFFER_SIZE);
    bytes_read = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);

    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';

        // Check if booking was successful or failed
        if (strstr(buffer, "SUCCESS") != NULL) {
            printf("╔══════════════════════════════════════════════════════════════╗\n");
            printf("║                  ✓ BOOKING CONFIRMED                        ║\n");
            printf("╚══════════════════════════════════════════════════════════════╝\n");
            printf("  %s\n", buffer);
        } else {
            printf("╔══════════════════════════════════════════════════════════════╗\n");
            printf("║                  ✗ BOOKING FAILED                           ║\n");
            printf("╚══════════════════════════════════════════════════════════════╝\n");
            printf("  %s\n", buffer);
        }
    } else {
        printf("[Client] Server disconnected without response.\n");
    }

    // --- Step 6: Terminate session ---
    close(sock_fd);
    printf("\n[Client] Session terminated. Connection closed.\n");

    return 0;
}