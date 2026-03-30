#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define NUM_ROUTES 2
#define TOTAL_SEATS 50

// ===================== Route Database =====================

typedef struct {
    int id;
    char source[64];
    char destination[64];
    int total_seats;
    int booked_seats;
} Route;

Route routes[NUM_ROUTES];
pthread_mutex_t route_locks[NUM_ROUTES];  // One mutex per route

// ===================== Initialization =====================

void init_routes() {
    // Route 1
    routes[0].id = 1;
    strcpy(routes[0].source, "Bangalore");
    strcpy(routes[0].destination, "Mumbai");
    routes[0].total_seats = TOTAL_SEATS;
    routes[0].booked_seats = 0;
    pthread_mutex_init(&route_locks[0], NULL);

    // Route 2
    routes[1].id = 2;
    strcpy(routes[1].source, "Delhi");
    strcpy(routes[1].destination, "Chennai");
    routes[1].total_seats = TOTAL_SEATS;
    routes[1].booked_seats = 0;
    pthread_mutex_init(&route_locks[1], NULL);
}

// ===================== Display Status =====================

void display_all_routes() {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║              TRAVEL TICKET RESERVATION DATABASE             ║\n");
    printf("╠════╦══════════════╦══════════════╦═══════════╦══════════════╣\n");
    printf("║ ID ║    Source    ║ Destination  ║ Available ║    Booked    ║\n");
    printf("╠════╬══════════════╬══════════════╬═══════════╬══════════════╣\n");

    for (int i = 0; i < NUM_ROUTES; i++) {
        pthread_mutex_lock(&route_locks[i]);
        int available = routes[i].total_seats - routes[i].booked_seats;
        printf("║ %2d ║ %-12s ║ %-12s ║    %3d    ║     %3d      ║\n",
               routes[i].id,
               routes[i].source,
               routes[i].destination,
               available,
               routes[i].booked_seats);
        pthread_mutex_unlock(&route_locks[i]);
    }

    printf("╚════╩══════════════╩══════════════╩═══════════╩══════════════╝\n\n");
}

// ===================== Get Date/Time =====================

void get_datetime(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

// ===================== Client Handler =====================

void *handle_client(void *arg) {
    int client_fd = *(int *)arg;
    free(arg);

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    getpeername(client_fd, (struct sockaddr *)&client_addr, &addr_len);

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    int client_port = ntohs(client_addr.sin_port);

    char datetime[64];
    get_datetime(datetime, sizeof(datetime));

    printf("[%s] Client connected: %s:%d\n", datetime, client_ip, client_port);

    // --- Step 1: Build and send the route menu to the client ---
    char menu[BUFFER_SIZE];
    int offset = 0;

    offset += snprintf(menu + offset, BUFFER_SIZE - offset,
                       "ROUTES_BEGIN\n");

    for (int i = 0; i < NUM_ROUTES; i++) {
        pthread_mutex_lock(&route_locks[i]);
        int available = routes[i].total_seats - routes[i].booked_seats;
        offset += snprintf(menu + offset, BUFFER_SIZE - offset,
                           "%d|%s|%s|%d|%d\n",
                           routes[i].id,
                           routes[i].source,
                           routes[i].destination,
                           available,
                           routes[i].booked_seats);
        pthread_mutex_unlock(&route_locks[i]);
    }

    offset += snprintf(menu + offset, BUFFER_SIZE - offset,
                       "ROUTES_END\n");

    send(client_fd, menu, strlen(menu), 0);

    // --- Step 2: Receive booking request from client ---
    // Format: "route_id|num_seats"
    char buffer[BUFFER_SIZE] = {0};
    int bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

    if (bytes_read <= 0) {
        printf("[Server] Client %s:%d disconnected before sending request.\n",
               client_ip, client_port);
        close(client_fd);
        return NULL;
    }

    buffer[bytes_read] = '\0';
    buffer[strcspn(buffer, "\r\n")] = '\0';

    int route_id, num_seats;
    if (sscanf(buffer, "%d|%d", &route_id, &num_seats) != 2) {
        const char *err = "ERROR: Invalid request format.";
        send(client_fd, err, strlen(err), 0);
        close(client_fd);
        return NULL;
    }

    get_datetime(datetime, sizeof(datetime));
    printf("[%s] Booking request from %s:%d -> Route %d, Seats: %d\n",
           datetime, client_ip, client_port, route_id, num_seats);

    // --- Step 3: Validate route ID ---
    if (route_id < 1 || route_id > NUM_ROUTES) {
        const char *err = "FAILURE: Invalid route ID selected.";
        send(client_fd, err, strlen(err), 0);
        printf("[Server] Invalid route ID %d from %s:%d\n",
               route_id, client_ip, client_port);
        close(client_fd);
        return NULL;
    }

    if (num_seats <= 0) {
        const char *err = "FAILURE: Number of seats must be positive.";
        send(client_fd, err, strlen(err), 0);
        close(client_fd);
        return NULL;
    }

    // --- Step 4: Attempt to book seats (thread-safe) ---
    int route_index = route_id - 1;
    char response[BUFFER_SIZE];

    pthread_mutex_lock(&route_locks[route_index]);

    int available = routes[route_index].total_seats - routes[route_index].booked_seats;

    if (num_seats > available) {
        snprintf(response, BUFFER_SIZE,
                 "FAILURE: Requested %d seats but only %d available on Route %d (%s -> %s). "
                 "Booking REJECTED.",
                 num_seats, available,
                 route_id,
                 routes[route_index].source,
                 routes[route_index].destination);

        pthread_mutex_unlock(&route_locks[route_index]);

        send(client_fd, response, strlen(response), 0);

        printf("[Server] REJECTED: %s:%d requested %d seats, only %d available on Route %d.\n",
               client_ip, client_port, num_seats, available, route_id);
    } else {
        // Book the seats
        routes[route_index].booked_seats += num_seats;
        int new_available = routes[route_index].total_seats - routes[route_index].booked_seats;
        int new_booked = routes[route_index].booked_seats;

        pthread_mutex_unlock(&route_locks[route_index]);

        snprintf(response, BUFFER_SIZE,
                 "SUCCESS: %d seat(s) booked on Route %d (%s -> %s). "
                 "Seats remaining: %d. Total booked: %d.",
                 num_seats,
                 route_id,
                 routes[route_index].source,
                 routes[route_index].destination,
                 new_available,
                 new_booked);

        send(client_fd, response, strlen(response), 0);

        printf("[Server] CONFIRMED: %s:%d booked %d seats on Route %d. "
               "Remaining: %d, Booked: %d\n",
               client_ip, client_port, num_seats, route_id,
               new_available, new_booked);
    }

    // --- Step 5: Display updated database ---
    display_all_routes();

    close(client_fd);

    get_datetime(datetime, sizeof(datetime));
    printf("[%s] Client %s:%d session ended.\n\n", datetime, client_ip, client_port);

    return NULL;
}

// ===================== Main =====================

int main() {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Initialize route database
    init_routes();

    // --- Create socket ---
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("╔══════════════════════════════════════════════╗\n");
    printf("║     TRAVEL TICKET RESERVATION SERVER        ║\n");
    printf("║     Listening on port %d                  ║\n", PORT);
    printf("║     Concurrent server (multi-threaded)      ║\n");
    printf("║     Press Ctrl+C to stop                    ║\n");
    printf("╚══════════════════════════════════════════════╝\n");

    // Display initial database
    display_all_routes();

    // --- Accept clients concurrently ---
    while (1) {
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        // Spawn a thread for each client
        int *client_fd_ptr = malloc(sizeof(int));
        *client_fd_ptr = client_fd;

        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, client_fd_ptr);
        pthread_detach(tid);  // Auto-cleanup when thread finishes
    }

    // Cleanup
    for (int i = 0; i < NUM_ROUTES; i++) {
        pthread_mutex_destroy(&route_locks[i]);
    }
    close(server_fd);

    return 0;
}