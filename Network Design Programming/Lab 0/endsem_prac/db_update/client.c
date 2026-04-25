#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 4096

void print_menu()
{
    printf("\n=============================================\n");
    printf("  BOOK DATABASE CLIENT\n");
    printf("=============================================\n");
    printf("  1. Insert a new book\n");
    printf("  2. Update Harry Potter genre to Fantasy\n");
    printf("  3. Search author with most publications\n");
    printf("  4. Exit\n");
    printf("=============================================\n");
    printf("  Enter your choice: ");
}

void do_insert(int sock)
{
    char buffer[BUFFER_SIZE] = {0};
    char data[BUFFER_SIZE] = {0};

    // Send command
    send(sock, "INSERT", 6, 0);

    // Wait for server prompt
    int bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0);
    if (bytes <= 0)
        return;
    buffer[bytes] = '\0';

    if (strcmp(buffer, "ENTER_BOOK_DETAILS") != 0)
    {
        printf("%s\n", buffer);
        return;
    }

    // Collect book details from user
    int id;
    char name[100], title[100], publisher[100], genre[50];

    printf("\n--- Enter Book Details ---\n");
    printf("  Book ID       : ");
    scanf("%d", &id);
    getchar();
    printf("  Author Name   : ");
    fgets(name, 100, stdin);
    name[strcspn(name, "\n")] = '\0';
    printf("  Title         : ");
    fgets(title, 100, stdin);
    title[strcspn(title, "\n")] = '\0';
    printf("  Publisher     : ");
    fgets(publisher, 100, stdin);
    publisher[strcspn(publisher, "\n")] = '\0';
    printf("  Genre         : ");
    fgets(genre, 50, stdin);
    genre[strcspn(genre, "\n")] = '\0';

    // Format: id|name|title|publisher|genre
    snprintf(data, BUFFER_SIZE, "%d|%s|%s|%s|%s", id, name, title, publisher, genre);
    send(sock, data, strlen(data), 0);

    // Receive and display result
    memset(buffer, 0, BUFFER_SIZE);
    bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0);
    if (bytes > 0)
    {
        buffer[bytes] = '\0';
        printf("\n%s\n", buffer);
    }
}

void do_update(int sock)
{
    char buffer[BUFFER_SIZE] = {0};

    send(sock, "UPDATE", 6, 0);

    int bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0);
    if (bytes > 0)
    {
        buffer[bytes] = '\0';
        printf("\n%s\n", buffer);
    }
}

void do_search(int sock)
{
    char buffer[BUFFER_SIZE] = {0};

    send(sock, "SEARCH", 6, 0);

    int bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0);
    if (bytes > 0)
    {
        buffer[bytes] = '\0';
        printf("\n%s\n", buffer);
    }
}

void do_exit(int sock)
{
    char buffer[BUFFER_SIZE] = {0};

    send(sock, "EXIT", 4, 0);

    int bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0);
    if (bytes > 0)
    {
        buffer[bytes] = '\0';
        printf("\n  Server says: %s\n", buffer);
    }
    printf("  Closing connection. Goodbye!\n");
    printf("=============================================\n\n");
}

int main()
{
    int sock;
    struct sockaddr_in server_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("\n=============================================\n");
    printf("  Connected to Book Database Server\n");
    printf("=============================================\n");

    int choice;
    while (1)
    {
        print_menu();
        scanf("%d", &choice);
        getchar();

        switch (choice)
        {
        case 1:
            do_insert(sock);
            break;
        case 2:
            do_update(sock);
            break;
        case 3:
            do_search(sock);
            break;
        case 4:
            do_exit(sock);
            close(sock);
            return 0;
        default:
            printf("  Invalid choice. Please enter 1-4.\n");
        }
    }

    close(sock);
    return 0;
}