// server.c - Banking Application Server
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define DB_FILE "bank_db.txt"

// Caesar Cipher encryption (shift of 3)
void encrypt(const char *input, char *output) {
    int i = 0;
    while (input[i] != '\0') {
        char c = input[i];
        if (c >= 'a' && c <= 'z')
            output[i] = ((c - 'a' + 3) % 26) + 'a';
        else if (c >= 'A' && c <= 'Z')
            output[i] = ((c - 'A' + 3) % 26) + 'A';
        else
            output[i] = c; // digits and special chars unchanged
        i++;
    }
    output[i] = '\0';
}

// Initialize DB with sample users if not exists
void init_db() {
    FILE *fp = fopen(DB_FILE, "r");
    if (fp) { fclose(fp); return; }

    fp = fopen(DB_FILE, "w");
    if (!fp) { perror("Cannot create DB"); exit(1); }

    // Format: username encrypted_password balance
    // Passwords: alice->pass123, bob->secret
    char enc[256];
    encrypt("pass123", enc);
    fprintf(fp, "alice %s 5000.00\n", enc);
    encrypt("secret", enc);
    fprintf(fp, "bob %s 3200.50\n", enc);
    fclose(fp);
    printf("[Server] Database initialized.\n");
}

// Find user in DB: returns 1 if found, sets enc_pass and balance
int find_user(const char *username, char *enc_pass, double *balance) {
    FILE *fp = fopen(DB_FILE, "r");
    if (!fp) return -1;

    char uname[64], epass[128];
    double bal;
    while (fscanf(fp, "%s %s %lf", uname, epass, &bal) == 3) {
        if (strcmp(uname, username) == 0) {
            strcpy(enc_pass, epass);
            *balance = bal;
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

// Update balance for a user in DB
int update_balance(const char *username, double new_balance) {
    FILE *fp = fopen(DB_FILE, "r");
    if (!fp) return 0;

    char temp_file[] = "bank_db_tmp.txt";
    FILE *tmp = fopen(temp_file, "w");
    if (!tmp) { fclose(fp); return 0; }

    char uname[64], epass[128];
    double bal;
    while (fscanf(fp, "%s %s %lf", uname, epass, &bal) == 3) {
        if (strcmp(uname, username) == 0)
            fprintf(tmp, "%s %s %.2f\n", uname, epass, new_balance);
        else
            fprintf(tmp, "%s %s %.2f\n", uname, epass, bal);
    }
    fclose(fp);
    fclose(tmp);
    remove(DB_FILE);
    rename(temp_file, DB_FILE);
    return 1;
}

void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];
    char username[64], password[64], enc_pass[128], stored_enc[128];
    double balance;

    // Step 1: Receive username
    memset(buffer, 0, BUFFER_SIZE);
    recv(client_fd, buffer, BUFFER_SIZE, 0);
    strncpy(username, buffer, 63);
    username[63] = '\0';

    // Step 2: Receive password
    memset(buffer, 0, BUFFER_SIZE);
    recv(client_fd, buffer, BUFFER_SIZE, 0);
    strncpy(password, buffer, 63);
    password[63] = '\0';

    printf("[Server] Login attempt - Username: %s\n", username);

    // Validate credentials
    int found = find_user(username, stored_enc, &balance);
    if (!found) {
        printf("[Server] Incorrect Username: %s\n", username);
        send(client_fd, "Incorrect Username", 18, 0);
        close(client_fd);
        return;
    }

    encrypt(password, enc_pass);
    if (strcmp(enc_pass, stored_enc) != 0) {
        printf("[Server] Incorrect Password for user: %s\n", username);
        send(client_fd, "Incorrect Password", 18, 0);
        close(client_fd);
        return;
    }

    printf("[Server] Login successful for user: %s\n", username);
    send(client_fd, "Login Successful", 16, 0);

    // Banking menu loop
    while (1) {
        char menu[512];
        snprintf(menu, sizeof(menu),
            "\n===== BANKING MENU =====\n"
            "a. Debit\n"
            "b. Credit\n"
            "c. View Balance\n"
            "d. EXIT\n"
            "Enter choice: ");
        send(client_fd, menu, strlen(menu), 0);

        memset(buffer, 0, BUFFER_SIZE);
        int n = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (n <= 0) break;
        char choice = buffer[0];

        if (choice == 'a' || choice == 'A') {
            send(client_fd, "Enter debit amount: ", 20, 0);
            memset(buffer, 0, BUFFER_SIZE);
            recv(client_fd, buffer, BUFFER_SIZE, 0);
            double amount = atof(buffer);
            if (amount <= 0) {
                send(client_fd, "Invalid amount.", 15, 0);
            } else if (amount > balance) {
                send(client_fd, "Insufficient balance.", 21, 0);
            } else {
                balance -= amount;
                update_balance(username, balance);
                snprintf(buffer, BUFFER_SIZE, "Debited %.2f. New Balance: %.2f", amount, balance);
                send(client_fd, buffer, strlen(buffer), 0);
                printf("[Server] Debited %.2f from %s. Balance: %.2f\n", amount, username, balance);
            }
        } else if (choice == 'b' || choice == 'B') {
            send(client_fd, "Enter credit amount: ", 21, 0);
            memset(buffer, 0, BUFFER_SIZE);
            recv(client_fd, buffer, BUFFER_SIZE, 0);
            double amount = atof(buffer);
            if (amount <= 0) {
                send(client_fd, "Invalid amount.", 15, 0);
            } else {
                balance += amount;
                update_balance(username, balance);
                snprintf(buffer, BUFFER_SIZE, "Credited %.2f. New Balance: %.2f", amount, balance);
                send(client_fd, buffer, strlen(buffer), 0);
                printf("[Server] Credited %.2f to %s. Balance: %.2f\n", amount, username, balance);
            }
        } else if (choice == 'c' || choice == 'C') {
            snprintf(buffer, BUFFER_SIZE, "Current Balance: %.2f", balance);
            send(client_fd, buffer, strlen(buffer), 0);
        } else if (choice == 'd' || choice == 'D') {
            send(client_fd, "Goodbye! Session ended.", 23, 0);
            printf("[Server] User %s logged out.\n", username);
            break;
        } else {
            send(client_fd, "Invalid choice. Try again.", 26, 0);
        }
    }

    close(client_fd);
}

int main() {
    init_db();

    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("Socket failed"); exit(1); }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed"); exit(1);
    }

    if (listen(server_fd, 5) < 0) { perror("Listen failed"); exit(1); }

    printf("[Server] Banking Server started on port %d\n", PORT);
    printf("[Server] Waiting for connections...\n");

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_fd < 0) { perror("Accept failed"); continue; }

        printf("[Server] Client connected: %s\n", inet_ntoa(client_addr.sin_addr));
        handle_client(client_fd);
        printf("[Server] Client disconnected.\n");
    }

    close(server_fd);
    return 0;
}
