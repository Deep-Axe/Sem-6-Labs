#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX 2048

void xor_division(char *data, char *gen, char *rem) {
    int datalen = strlen(data);
    int genlen = strlen(gen);
    char temp[MAX];
    strcpy(temp, data);

    for (int i = 0; i <= datalen - genlen; i++) {
        if (temp[i] == '1') {
            for (int j = 0; j < genlen; j++) {
                temp[i + j] = (temp[i + j] == gen[j]) ? '0' : '1';
            }
        }
    }
    // Remainder is the last (genlen - 1) bits
    strncpy(rem, temp + datalen - (genlen - 1), genlen - 1);
    rem[genlen - 1] = '\0';
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char data[MAX], codeword[MAX], rem[MAX], gen[MAX];
    int choice;

    char *crc12 = "1100000001111";         // x^12 + x^11 + x^3 + x^2 + x + 1
    char *crc16 = "11000000000000101";     // x^16 + x^15 + x^2 + 1
    char *crcccitt = "10001000000100001";  // x^16 + x^12 + x^5 + 1

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    printf("Enter binary data: ");
    scanf("%s", data);

    printf("Select CRC Polynomial:\n1. CRC-12\n2. CRC-16\n3. CRC-CCITT\nChoice: ");
    scanf("%d", &choice);

    switch(choice) {
        case 1: strcpy(gen, crc12); break;
        case 2: strcpy(gen, crc16); break;
        case 3: strcpy(gen, crcccitt); break;
        default: printf("Invalid choice\n"); close(sock); return -1;
    }

    int n = strlen(gen) - 1;
    strcpy(codeword, data);
    for (int i = 0; i < n; i++) strcat(codeword, "0");

    xor_division(codeword, gen, rem);
    
    // Replace the appended 0s with the actual remainder
    strcpy(codeword + strlen(data), rem);

    printf("[Sender] Original Data: %s\n", data);
    printf("[Sender] Remainder (CRC): %s\n", rem);
    printf("[Sender] Codeword: %s\n", codeword);

    // Send generator first, then codeword
    send(sock, gen, strlen(gen), 0);
    usleep(10000); // Small delay
    send(sock, codeword, strlen(codeword), 0);

    close(sock);
    return 0;
}
