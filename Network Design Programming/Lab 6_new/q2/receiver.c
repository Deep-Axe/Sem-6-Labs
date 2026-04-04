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
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char codeword[MAX] = {0}, gen[MAX] = {0}, rem[MAX] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    printf("[Receiver] Waiting for codeword...\n");

    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

    // Receive generator and codeword
    int n1 = recv(new_socket, gen, MAX, 0);
    gen[n1] = '\0';
    int n2 = recv(new_socket, codeword, MAX, 0);
    codeword[n2] = '\0';

    printf("[Receiver] Generator: %s\n", gen);
    printf("[Receiver] Codeword:  %s\n", codeword);

    xor_division(codeword, gen, rem);

    int error = 0;
    for (int i = 0; rem[i] != '\0'; i++) {
        if (rem[i] == '1') error = 1;
    }

    if (!error) {
        printf("[Receiver] Remainder is Zero: No error detected. Data Accepted.\n");
    } else {
        printf("[Receiver] Remainder (%s) is Non-Zero: Error detected! Rejecting and requesting retransmission.\n", rem);
    }

    close(new_socket);
    close(server_fd);
    return 0;
}
