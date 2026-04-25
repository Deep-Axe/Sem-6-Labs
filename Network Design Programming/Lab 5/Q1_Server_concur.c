#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 2
#define BUFFER_SIZE 1024

// Shared state protected by mutex
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int client_count = 0;
int client_sockets[MAX_CLIENTS];
struct sockaddr_in client_addrs[MAX_CLIENTS];
char client_messages[MAX_CLIENTS][BUFFER_SIZE];
int messages_received = 0;

void send_terminate_to_all() {
    const char *term_msg = "terminate session";
    for (int i = 0; i < client_count; i++) {
        send(client_sockets[i], term_msg, strlen(term_msg), 0);
    }
}

void *handle_client(void *arg) {
    int index = *(int *)arg;
    free(arg);

    int client_fd = client_sockets[index];
    struct sockaddr_in addr = client_addrs[index];

    char buffer[BUFFER_SIZE] = {0};
    int bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

    if (bytes_read <= 0) {
        printf("[Server] Failed to receive data from client %d.\n", index + 1);
        close(client_fd);
        return NULL;
    }

    buffer[bytes_read] = '\0';

    char client_addr_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, client_addr_str, INET_ADDRSTRLEN);
    int client_port = ntohs(addr.sin_port);

    printf("[Server] Received from Client %d (%s:%d): \"%s\"\n",
           index + 1, client_addr_str, client_port, buffer);

    pthread_mutex_lock(&lock);
    strncpy(client_messages[index], buffer, BUFFER_SIZE - 1);
    messages_received++;

    // Once both messages are received, build the final string
    if (messages_received == MAX_CLIENTS) {
        pthread_mutex_unlock(&lock);

        // --- Read keyword from file ---
        FILE *fp = fopen("keyword.txt", "r");
        if (!fp) {
            printf("[Server] Error: Could not open keyword.txt\n");
            return NULL;
        }

        char keyword[BUFFER_SIZE] = {0};
        fgets(keyword, BUFFER_SIZE, fp);
        fclose(fp);

        // Remove trailing newline if present
        keyword[strcspn(keyword, "\r\n")] = '\0';

        // --- Append received strings to file ---
        fp = fopen("keyword.txt", "w");
        if (!fp) {
            printf("[Server] Error: Could not open keyword.txt for writing\n");
            return NULL;
        }

        // Build final string: "Manipal" + " " + "Institute Of" + " " + "Technology"
        char final_string[BUFFER_SIZE];
        snprintf(final_string, BUFFER_SIZE, "%s %s %s",
                 keyword, client_messages[0], client_messages[1]);

        fprintf(fp, "%s", final_string);
        fclose(fp);

        // --- Display result ---
        printf("\n========================================\n");
        printf("  COMBINED RESULT\n");
        printf("========================================\n");
        printf("  Final String : %s\n", final_string);
        printf("----------------------------------------\n");

        for (int i = 0; i < MAX_CLIENTS; i++) {
            char addr_str[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addrs[i].sin_addr, addr_str, INET_ADDRSTRLEN);
            int port = ntohs(client_addrs[i].sin_port);
            printf("  Client %d Addr : %s:%d\n", i + 1, addr_str, port);
        }

        printf("========================================\n\n");

        // Send success acknowledgment to both clients
        for (int i = 0; i < MAX_CLIENTS; i++) {
            char ack[BUFFER_SIZE];
            snprintf(ack, BUFFER_SIZE, "SUCCESS: %s", final_string);
            send(client_sockets[i], ack, strlen(ack), 0);
        }
    } else {
        pthread_mutex_unlock(&lock);
    }

    return NULL;
}

int main() {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // --- Create the keyword file with "Manipal" ---
    FILE *fp = fopen("keyword.txt", "w");
    if (!fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "Manipal");
    fclose(fp);
    printf("[Server] Created keyword.txt with keyword \"Manipal\"\n");

    // --- Create socket ---
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Allow address reuse
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

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("[Server] Listening on port %d...\n", PORT);
    printf("[Server] Waiting for %d clients to connect...\n\n", MAX_CLIENTS);

    pthread_t threads[MAX_CLIENTS];

    // --- Accept clients in a loop ---
    while (1) {
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        pthread_mutex_lock(&lock);
        int current_count = client_count;

        // If already MAX_CLIENTS connected, reject this one (exceeds 2)
        if (current_count >= MAX_CLIENTS) {
            pthread_mutex_unlock(&lock);

            printf("[Server] Client limit exceeded! Sending terminate to all.\n");

            const char *term_msg = "terminate session";
            send(client_fd, term_msg, strlen(term_msg), 0);
            close(client_fd);

            // Send terminate to previously connected clients
            send_terminate_to_all();

            // Close all client sockets
            for (int i = 0; i < MAX_CLIENTS; i++) {
                close(client_sockets[i]);
            }

            printf("[Server] All clients notified. Shutting down.\n");
            close(server_fd);
            exit(0);
        }

        client_sockets[current_count] = client_fd;
        client_addrs[current_count] = client_addr;
        client_count++;
        pthread_mutex_unlock(&lock);

        char addr_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, addr_str, INET_ADDRSTRLEN);
        printf("[Server] Client %d connected from %s:%d\n",
               current_count + 1, addr_str, ntohs(client_addr.sin_port));

        // Spawn a thread to handle this client
        int *index = malloc(sizeof(int));
        *index = current_count;
        pthread_create(&threads[current_count], NULL, handle_client, index);
    }

    // Cleanup (unreachable in normal flow, but good practice)
    for (int i = 0; i < MAX_CLIENTS; i++) {
        pthread_join(threads[i], NULL);
        close(client_sockets[i]);
    }
    close(server_fd);

    return 0;
}
