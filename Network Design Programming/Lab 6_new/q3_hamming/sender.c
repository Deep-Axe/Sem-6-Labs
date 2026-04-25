#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char data[1024], codeword[1024];
    int m, r = 0, n;

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

    printf("Enter binary data bits: ");
    scanf("%s", data);
    m = strlen(data);

    // Calculate redundant bits r: 2^r >= m + r + 1
    while ((1 << r) < (m + r + 1)) {
        r++;
    }

    n = m + r;
    int code[n + 1];
    memset(code, 0, sizeof(code));

    // Place data bits in non-power-of-2 positions
    int j = 0;
    for (int i = 1; i <= n; i++) {
        if ((i & (i - 1)) == 0) {
            code[i] = 0; // Placeholder for parity
        } else {
            code[i] = data[j++] - '0';
        }
    }

    // Calculate parity bits
    for (int i = 0; i < r; i++) {
        int pos = (1 << i);
        int parity = 0;
        for (int k = 1; k <= n; k++) {
            if (k & pos) {
                if (k != pos) parity ^= code[k];
            }
        }
        code[pos] = parity;
    }

    printf("[Sender] Generated Codeword: ");
    for (int i = 1; i <= n; i++) printf("%d", code[i]);
    printf("\n");

    // Optional: Simulate Error
    int flip;
    printf("Enter bit position to flip (1-%d) or 0 for no error: ", n);
    scanf("%d", &flip);
    if (flip > 0 && flip <= n) {
        code[flip] = !code[flip];
        printf("[Sender] Error injected at position %d\n", flip);
    }

    // Convert to string to send
    char buffer[1024];
    for (int i = 1; i <= n; i++) buffer[i - 1] = code[i] + '0';
    buffer[n] = '\0';

    send(sock, buffer, strlen(buffer), 0);
    printf("[Sender] Transmitted Codeword: %s\n", buffer);

    close(sock);
    return 0;
}
