#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAXLINE 1024

// Simple DNS database structure
typedef struct {
    char hostname[256];
    char ip[16];
} DNSRecord;

// Function to load database from file
int loadDatabase(DNSRecord records[], int maxRecords) {
    FILE *fp = fopen("database.txt", "r");
    if (!fp) {
        perror("Failed to open database.txt");
        return 0;
    }

    int count = 0;
    while (fscanf(fp, "%255s %15s", records[count].hostname, records[count].ip) == 2 && count < maxRecords) {
        count++;
    }
    fclose(fp);
    return count;
}

// Function to search IP by hostname
char* lookupIP(DNSRecord records[], int count, const char *hostname) {
    for (int i = 0; i < count; i++) {
        if (strcmp(records[i].hostname, hostname) == 0) {
            return records[i].ip;
        }
    }
    return NULL;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[MAXLINE] = {0};

    // Load DNS database
    DNSRecord records[100];
    int recordCount = loadDatabase(records, 100);
    if (recordCount == 0) {
        printf("No records found in database.txt\n");
        return 1;
    }

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("DNS Server listening on port %d...\n", PORT);

    while (1) {
        // Accept client connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        memset(buffer, 0, MAXLINE);
        read(new_socket, buffer, MAXLINE);
        printf("Received request for hostname: %s\n", buffer);

        char *ip = lookupIP(records, recordCount, buffer);
        if (ip == NULL) {
            strcpy(buffer, "Hostname not found");
        } else {
            strcpy(buffer, ip);
        }

        send(new_socket, buffer, strlen(buffer), 0);
        close(new_socket);
    }

    return 0;
}
