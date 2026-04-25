#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 4096
#define DB_FILE "books.txt"

typedef struct
{
    int book_id;
    char book_name[100];
    char title[100];
    char publisher_name[100];
    char genre[50];
} Book;

// ── File helpers ─────────────────────────────────────────────────────────────

int load_books(Book *db)
{
    FILE *f = fopen(DB_FILE, "r");
    if (!f)
        return 0;
    int count = 0;
    while (fscanf(f, "%d|%99[^|]|%99[^|]|%99[^|]|%49[^\n]\n",
                  &db[count].book_id,
                  db[count].book_name,
                  db[count].title,
                  db[count].publisher_name,
                  db[count].genre) == 5)
    {
        count++;
    }
    fclose(f);
    return count;
}

void save_books(Book *db, int count)
{
    FILE *f = fopen(DB_FILE, "w");
    if (!f)
    {
        perror("fopen");
        return;
    }
    for (int i = 0; i < count; i++)
    {
        fprintf(f, "%d|%s|%s|%s|%s\n",
                db[i].book_id,
                db[i].book_name,
                db[i].title,
                db[i].publisher_name,
                db[i].genre);
    }
    fclose(f);
}

void init_database()
{
    // Only seed if file doesn't exist yet
    FILE *f = fopen(DB_FILE, "r");
    if (f)
    {
        fclose(f);
        return;
    }

    Book db[100];
    int n = 0;

    db[n].book_id = 1;
    strcpy(db[n].book_name, "J.K. Rowling");
    strcpy(db[n].title, "Harry Potter");
    strcpy(db[n].publisher_name, "Bloomsbury");
    strcpy(db[n].genre, "Comic");
    n++;

    db[n].book_id = 2;
    strcpy(db[n].book_name, "J.K. Rowling");
    strcpy(db[n].title, "Harry Potter and the Chamber of Secrets");
    strcpy(db[n].publisher_name, "Bloomsbury");
    strcpy(db[n].genre, "Fantasy");
    n++;

    db[n].book_id = 3;
    strcpy(db[n].book_name, "George Orwell");
    strcpy(db[n].title, "1984");
    strcpy(db[n].publisher_name, "Secker and Warburg");
    strcpy(db[n].genre, "Dystopian");
    n++;

    db[n].book_id = 4;
    strcpy(db[n].book_name, "George Orwell");
    strcpy(db[n].title, "Animal Farm");
    strcpy(db[n].publisher_name, "Secker and Warburg");
    strcpy(db[n].genre, "Political Satire");
    n++;

    db[n].book_id = 5;
    strcpy(db[n].book_name, "Agatha Christie");
    strcpy(db[n].title, "Murder on the Orient Express");
    strcpy(db[n].publisher_name, "Collins Crime Club");
    strcpy(db[n].genre, "Mystery");
    n++;

    save_books(db, n);
    printf("[Server] Seeded %d books to %s\n", n, DB_FILE);
}

// ── Handlers ─────────────────────────────────────────────────────────────────

void handle_insert(int client_fd)
{
    char buffer[BUFFER_SIZE] = {0};
    char response[BUFFER_SIZE] = {0};

    send(client_fd, "ENTER_BOOK_DETAILS", 18, 0);

    int bytes = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes <= 0)
        return;
    buffer[bytes] = '\0';

    // Load current books
    Book db[100];
    int count = load_books(db);

    if (count >= 100)
    {
        snprintf(response, BUFFER_SIZE, "ERROR: Database is full.\n");
        send(client_fd, response, strlen(response), 0);
        return;
    }

    Book *b = &db[count];
    char *token = strtok(buffer, "|");
    if (token)
        b->book_id = atoi(token);
    token = strtok(NULL, "|");
    if (token)
        strncpy(b->book_name, token, 99);
    token = strtok(NULL, "|");
    if (token)
        strncpy(b->title, token, 99);
    token = strtok(NULL, "|");
    if (token)
        strncpy(b->publisher_name, token, 99);
    token = strtok(NULL, "|");
    if (token)
        strncpy(b->genre, token, 49);

    count++;
    save_books(db, count); // Persist to file

    // Build response table
    snprintf(response, BUFFER_SIZE,
             "INSERT OK — saved to %s\n"
             "============================================================\n"
             " %-5s %-20s %-35s %-20s %-15s\n"
             "------------------------------------------------------------\n",
             DB_FILE, "ID", "Author", "Title", "Publisher", "Genre");

    for (int i = 0; i < count; i++)
    {
        char row[256];
        snprintf(row, sizeof(row), " %-5d %-20s %-35s %-20s %-15s\n",
                 db[i].book_id, db[i].book_name, db[i].title,
                 db[i].publisher_name, db[i].genre);
        strncat(response, row, BUFFER_SIZE - strlen(response) - 1);
    }
    strncat(response, "============================================================\n",
            BUFFER_SIZE - strlen(response) - 1);

    send(client_fd, response, strlen(response), 0);
    printf("[Server] Book inserted. Total books in file: %d\n", count);
}

void handle_update(int client_fd)
{
    char response[BUFFER_SIZE] = {0};

    Book db[100];
    int count = load_books(db);
    int found = 0;

    for (int i = 0; i < count; i++)
    {
        if (strcmp(db[i].title, "Harry Potter") == 0)
        {
            strcpy(db[i].genre, "Fantasy");
            found = 1;
            save_books(db, count); // Persist update
            snprintf(response, BUFFER_SIZE,
                     "UPDATE OK — changes saved to %s\n"
                     "============================================================\n"
                     " UPDATED RECORD\n"
                     "============================================================\n"
                     " ID        : %d\n"
                     " Author    : %s\n"
                     " Title     : %s\n"
                     " Publisher : %s\n"
                     " Genre     : %s  <-- Updated from Comic to Fantasy\n"
                     "============================================================\n",
                     DB_FILE,
                     db[i].book_id, db[i].book_name,
                     db[i].title, db[i].publisher_name, db[i].genre);
            break;
        }
    }

    if (!found)
    {
        snprintf(response, BUFFER_SIZE,
                 "UPDATE FAILED: 'Harry Potter' not found in %s\n", DB_FILE);
    }

    send(client_fd, response, strlen(response), 0);
    printf("[Server] Update done. Found: %s\n", found ? "Yes" : "No");
}

void handle_search(int client_fd)
{
    char response[BUFFER_SIZE] = {0};

    Book db[100];
    int count = load_books(db);

    if (count == 0)
    {
        snprintf(response, BUFFER_SIZE, "SEARCH FAILED: %s is empty.\n", DB_FILE);
        send(client_fd, response, strlen(response), 0);
        return;
    }

    char authors[100][100];
    int counts[100];
    int num_authors = 0;

    for (int i = 0; i < count; i++)
    {
        int found = 0;
        for (int j = 0; j < num_authors; j++)
        {
            if (strcmp(authors[j], db[i].book_name) == 0)
            {
                counts[j]++;
                found = 1;
                break;
            }
        }
        if (!found)
        {
            strncpy(authors[num_authors], db[i].book_name, 99);
            counts[num_authors] = 1;
            num_authors++;
        }
    }

    int max_idx = 0;
    for (int i = 1; i < num_authors; i++)
    {
        if (counts[i] > counts[max_idx])
            max_idx = i;
    }

    snprintf(response, BUFFER_SIZE,
             "SEARCH RESULT — read from %s\n"
             "============================================================\n"
             " Author with Maximum Publications\n"
             "============================================================\n"
             " Author Name  : %s\n"
             " Publications : %d book(s)\n"
             "============================================================\n",
             DB_FILE, authors[max_idx], counts[max_idx]);

    send(client_fd, response, strlen(response), 0);
    printf("[Server] Search done. Max author: %s (%d books)\n",
           authors[max_idx], counts[max_idx]);
}

void handle_client(int client_fd)
{
    char buffer[BUFFER_SIZE] = {0};
    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes <= 0)
            break;
        buffer[bytes] = '\0';

        printf("[Server] Command: %s\n", buffer);

        if (strncmp(buffer, "INSERT", 6) == 0)
            handle_insert(client_fd);
        else if (strncmp(buffer, "UPDATE", 6) == 0)
            handle_update(client_fd);
        else if (strncmp(buffer, "SEARCH", 6) == 0)
            handle_search(client_fd);
        else if (strncmp(buffer, "EXIT", 4) == 0)
        {
            send(client_fd, "Bye", 3, 0);
            printf("[Server] Client exited.\n");
            break;
        }
        else
        {
            send(client_fd, "ERROR: Unknown command.\n", 23, 0);
        }
    }
}

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    init_database(); // Seed file if it doesn't exist

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 5) < 0)
    {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("=============================================\n");
    printf("  BOOK DATABASE SERVER (file-based)\n");
    printf("  Storage : %s\n", DB_FILE);
    printf("  Port    : %d\n", PORT);
    printf("  Press Ctrl+C to stop\n");
    printf("=============================================\n");

    while (1)
    {
        printf("\n[Server] Waiting for client...\n");
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0)
        {
            perror("accept");
            continue;
        }

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, ip, INET_ADDRSTRLEN);
        printf("[Server] Client connected: %s:%d\n", ip, ntohs(client_addr.sin_port));

        handle_client(client_fd);
        close(client_fd);
        printf("[Server] Client disconnected.\n");
    }

    close(server_fd);
    return 0;
}