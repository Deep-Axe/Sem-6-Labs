#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 4096
#define MAX_BOOKS 100

typedef struct
{
    int book_id;
    char book_name[100];
    char title[100];
    char publisher_name[100];
    char genre[50];
} Book;

Book database[MAX_BOOKS];
int book_count = 0;

void init_database()
{
    // Book 1
    database[0].book_id = 1;
    strcpy(database[0].book_name, "J.K. Rowling");
    strcpy(database[0].title, "Harry Potter");
    strcpy(database[0].publisher_name, "Bloomsbury");
    strcpy(database[0].genre, "Comic");

    // Book 2
    database[1].book_id = 2;
    strcpy(database[1].book_name, "J.K. Rowling");
    strcpy(database[1].title, "Harry Potter and the Chamber of Secrets");
    strcpy(database[1].publisher_name, "Bloomsbury");
    strcpy(database[1].genre, "Fantasy");

    // Book 3
    database[2].book_id = 3;
    strcpy(database[2].book_name, "George Orwell");
    strcpy(database[2].title, "1984");
    strcpy(database[2].publisher_name, "Secker and Warburg");
    strcpy(database[2].genre, "Dystopian");

    // Book 4
    database[3].book_id = 4;
    strcpy(database[3].book_name, "George Orwell");
    strcpy(database[3].title, "Animal Farm");
    strcpy(database[3].publisher_name, "Secker and Warburg");
    strcpy(database[3].genre, "Political Satire");

    // Book 5
    database[4].book_id = 5;
    strcpy(database[4].book_name, "Agatha Christie");
    strcpy(database[4].title, "Murder on the Orient Express");
    strcpy(database[4].publisher_name, "Collins Crime Club");
    strcpy(database[4].genre, "Mystery");

    book_count = 5;
}

void handle_insert(int client_fd)
{
    char buffer[BUFFER_SIZE] = {0};
    char response[BUFFER_SIZE] = {0};

    // Ask client for book details
    send(client_fd, "ENTER_BOOK_DETAILS", 18, 0);

    int bytes = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes <= 0)
        return;
    buffer[bytes] = '\0';

    if (book_count >= MAX_BOOKS)
    {
        snprintf(response, BUFFER_SIZE, "ERROR: Database is full.\n");
        send(client_fd, response, strlen(response), 0);
        return;
    }

    Book *b = &database[book_count];

    // Parse: book_id|book_name|title|publisher|genre
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

    book_count++;

    // Build display of all books
    snprintf(response, BUFFER_SIZE,
             "INSERT OK\n"
             "============================================================\n"
             " BOOK DATABASE\n"
             "============================================================\n"
             " %-5s %-20s %-35s %-20s %-15s\n"
             "------------------------------------------------------------\n",
             "ID", "Author", "Title", "Publisher", "Genre");

    for (int i = 0; i < book_count; i++)
    {
        char row[256];
        snprintf(row, sizeof(row), " %-5d %-20s %-35s %-20s %-15s\n",
                 database[i].book_id,
                 database[i].book_name,
                 database[i].title,
                 database[i].publisher_name,
                 database[i].genre);
        strncat(response, row, BUFFER_SIZE - strlen(response) - 1);
    }
    strncat(response, "============================================================\n",
            BUFFER_SIZE - strlen(response) - 1);

    send(client_fd, response, strlen(response), 0);
    printf("[Server] Book inserted. Total books: %d\n", book_count);
}

void handle_update(int client_fd)
{
    char response[BUFFER_SIZE] = {0};
    int found = 0;

    for (int i = 0; i < book_count; i++)
    {
        if (strcmp(database[i].title, "Harry Potter") == 0)
        {
            strcpy(database[i].genre, "Fantasy");
            found = 1;
            snprintf(response, BUFFER_SIZE,
                     "UPDATE OK\n"
                     "============================================================\n"
                     " UPDATED RECORD\n"
                     "============================================================\n"
                     " ID        : %d\n"
                     " Author    : %s\n"
                     " Title     : %s\n"
                     " Publisher : %s\n"
                     " Genre     : %s  <-- Updated from Comic to Fantasy\n"
                     "============================================================\n",
                     database[i].book_id,
                     database[i].book_name,
                     database[i].title,
                     database[i].publisher_name,
                     database[i].genre);
            break;
        }
    }

    if (!found)
    {
        snprintf(response, BUFFER_SIZE,
                 "UPDATE FAILED: Book titled 'Harry Potter' not found in database.\n");
    }

    send(client_fd, response, strlen(response), 0);
    printf("[Server] Update operation done. Found: %s\n", found ? "Yes" : "No");
}

void handle_search(int client_fd)
{
    char response[BUFFER_SIZE] = {0};

    if (book_count == 0)
    {
        snprintf(response, BUFFER_SIZE, "SEARCH FAILED: Database is empty.\n");
        send(client_fd, response, strlen(response), 0);
        return;
    }

    // Count publications per author
    char authors[MAX_BOOKS][100];
    int counts[MAX_BOOKS];
    int num_authors = 0;

    for (int i = 0; i < book_count; i++)
    {
        int found = 0;
        for (int j = 0; j < num_authors; j++)
        {
            if (strcmp(authors[j], database[i].book_name) == 0)
            {
                counts[j]++;
                found = 1;
                break;
            }
        }
        if (!found)
        {
            strncpy(authors[num_authors], database[i].book_name, 99);
            counts[num_authors] = 1;
            num_authors++;
        }
    }

    // Find max
    int max_idx = 0;
    for (int i = 1; i < num_authors; i++)
    {
        if (counts[i] > counts[max_idx])
        {
            max_idx = i;
        }
    }

    snprintf(response, BUFFER_SIZE,
             "SEARCH RESULT\n"
             "============================================================\n"
             " Author with Maximum Publications\n"
             "============================================================\n"
             " Author Name  : %s\n"
             " Publications : %d book(s)\n"
             "============================================================\n",
             authors[max_idx], counts[max_idx]);

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

        printf("[Server] Received command: %s\n", buffer);

        if (strncmp(buffer, "INSERT", 6) == 0)
        {
            handle_insert(client_fd);
        }
        else if (strncmp(buffer, "UPDATE", 6) == 0)
        {
            handle_update(client_fd);
        }
        else if (strncmp(buffer, "SEARCH", 6) == 0)
        {
            handle_search(client_fd);
        }
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

    init_database();

    printf("=============================================\n");
    printf("  BOOK DATABASE SERVER\n");
    printf("=============================================\n");
    printf("  Listening on port %d\n", PORT);
    printf("  Initial database loaded: %d books\n", book_count);
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

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("[Server] Client connected: %s:%d\n",
               client_ip, ntohs(client_addr.sin_port));

        handle_client(client_fd);
        close(client_fd);
        printf("[Server] Client disconnected.\n");
    }

    close(server_fd);
    return 0;
}