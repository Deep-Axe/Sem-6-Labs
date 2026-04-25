#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to check if two strings are anagrams
int are_anagrams(const char *str1, const char *str2) {
    int freq[256] = {0};

    // Both strings must have the same length
    if (strlen(str1) != strlen(str2)) {
        return 0;
    }

    // Count character frequencies of str1
    for (int i = 0; str1[i]; i++) {
        freq[(unsigned char)str1[i]]++;
    }

    // Subtract character frequencies of str2
    for (int i = 0; str2[i]; i++) {
        freq[(unsigned char)str2[i]]--;
    }

    // If all frequencies are zero, strings are anagrams
    for (int i = 0; i < 256; i++) {
        if (freq[i] != 0) {
            return 0;
        }
    }

    return 1;
}

// Function to get current date and time as a formatted string
void get_datetime(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

void handle_client(int client_fd, struct sockaddr_in *client_addr) {
    char buffer[BUFFER_SIZE] = {0};
    char datetime[64];
    char client_ip[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &client_addr->sin_addr, client_ip, INET_ADDRSTRLEN);
    int client_port = ntohs(client_addr->sin_port);

    // Display date/time and client socket address
    get_datetime(datetime, sizeof(datetime));

    printf("\n================================================\n");
    printf("  CLIENT CONNECTED\n");
    printf("================================================\n");
    printf("  Date & Time  : %s\n", datetime);
    printf("  Client Addr  : %s:%d\n", client_ip, client_port);
    printf("================================================\n");

    // Receive the two strings from the client (sent as "string1\nstring2")
    int bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_read <= 0) {
        printf("[Server] Failed to receive data from client %s:%d\n",
               client_ip, client_port);
        return;
    }
    buffer[bytes_read] = '\0';

    // Parse the two strings separated by newline
    char *str1 = buffer;
    char *str2 = strchr(buffer, '\n');

    if (str2 == NULL) {
        const char *err_msg = "ERROR: Expected two strings separated by newline.";
        send(client_fd, err_msg, strlen(err_msg), 0);
        return;
    }

    *str2 = '\0';  // Null-terminate the first string
    str2++;         // Move to the start of the second string

    // Remove trailing newline from str2 if present
    str2[strcspn(str2, "\r\n")] = '\0';
    str1[strcspn(str1, "\r\n")] = '\0';

    printf("[Server] Received from %s:%d\n", client_ip, client_port);
    printf("         String 1: \"%s\"\n", str1);
    printf("         String 2: \"%s\"\n", str2);

    // Check for anagram
    char response[BUFFER_SIZE];

    if (are_anagrams(str1, str2)) {
        snprintf(response, BUFFER_SIZE,
                 "RESULT: \"%s\" and \"%s\" ARE anagrams of each other.",
                 str1, str2);
        printf("[Server] Result  : Anagrams ✓\n");
    } else {
        snprintf(response, BUFFER_SIZE,
                 "RESULT: \"%s\" and \"%s\" are NOT anagrams of each other.",
                 str1, str2);
        printf("[Server] Result  : Not Anagrams ✗\n");
    }

    // Send result back to client
    send(client_fd, response, strlen(response), 0);
    printf("[Server] Response sent to %s:%d\n", client_ip, client_port);

    // Display date/time when client processing is done
    get_datetime(datetime, sizeof(datetime));
    printf("[Server] Client %s:%d served at %s\n", client_ip, client_port, datetime);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

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

    // --- Bind ---
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // --- Listen ---
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("=============================================\n");
    printf("  ITERATIVE ANAGRAM SERVER\n");
    printf("=============================================\n");
    printf("  Listening on port %d\n", PORT);
    printf("  Serving clients one at a time (iterative)\n");
    printf("  Press Ctrl+C to stop the server\n");
    printf("=============================================\n");

    // --- Iterative server loop: handle one client at a time ---
    while (1) {
        printf("\n[Server] Waiting for next client connection...\n");

        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        // Handle this client completely before accepting the next one
        handle_client(client_fd, &client_addr);

        // Close the client connection
        close(client_fd);
        printf("[Server] Connection closed. Ready for next client.\n");
    }

    close(server_fd);
    return 0;
}