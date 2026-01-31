#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6000
#define MAX 1024

/* Function to swap characters */
void swap(char *a, char *b) {
    char t = *a;
    *a = *b;
    *b = t;
}

/* Function to generate permutations */
void permute(char *str, int l, int r) {
    if (l == r) {
        printf("%s\n", str);
        return;
    }
    for (int i = l; i <= r; i++) {
        swap(&str[l], &str[i]);
        permute(str, l + 1, r);
        swap(&str[l], &str[i]);  // backtrack
    }
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    char buffer[MAX];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    printf("UDP Server waiting...\n");

    len = sizeof(cliaddr);
    recvfrom(sockfd, buffer, MAX, 0,
             (struct sockaddr*)&cliaddr, &len);

    printf("\nString received from client: %s\n", buffer);
    printf("\nPermutations:\n");

    permute(buffer, 0, strlen(buffer) - 1);

    strcpy(buffer, "Permutations printed at server");
    sendto(sockfd, buffer, strlen(buffer), 0,
           (struct sockaddr*)&cliaddr, len);

    close(sockfd);
    return 0;
}
