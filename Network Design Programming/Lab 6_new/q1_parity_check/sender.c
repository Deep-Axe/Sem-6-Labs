#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char binary_data[BUFFER_SIZE];
    char codeword[BUFFER_SIZE];
    int parity_type; // 0 for Even, 1 for Odd
    int ones_count = 0;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    printf("Enter binary data (e.g., 1011): ");
    scanf("%s", binary_data);
    printf("Select Parity Type (0 for Even, 1 for Odd): ");
    scanf("%d", &parity_type);

    // Count number of 1s
    for (int i = 0; binary_data[i] != '\0'; i++) {
        if (binary_data[i] == '1') ones_count++;
    }

    char parity_bit;
    if (parity_type == 0) { // Even Parity
        parity_bit = (ones_count % 2 == 0) ? '0' : '1';
    } else { // Odd Parity
        parity_bit = (ones_count % 2 != 0) ? '0' : '1';
    }

    // Form codeword: Data + Parity Bit
    strcpy(codeword, binary_data);
    int len = strlen(codeword);
    codeword[len] = parity_bit;
    codeword[len + 1] = '\0';

    printf("[Sender] Original Data: %s\n", binary_data);
    printf("[Sender] Parity Bit Added: %c\n", parity_bit);
    printf("[Sender] Transmitting Codeword: %s\n", codeword);

    // Send parity type first, then codeword
    send(sock, &parity_type, sizeof(int), 0);
    send(sock, codeword, strlen(codeword), 0);

    close(sock);
    return 0;
}
