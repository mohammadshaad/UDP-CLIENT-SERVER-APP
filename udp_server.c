#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024
#define CHECKSUM_BITS 5

// Function to calculate the checksum value and return as binary string
char* calculateChecksum(const char* binaryCode)
{
    unsigned int sum = 0;
    unsigned int i;

    for (i = 0; i < strlen(binaryCode); i++)
    {
        sum += binaryCode[i] - '0';  // Convert character to integer
        sum %= (1 << CHECKSUM_BITS); // Keep the sum within the bit range
    }

    // Convert checksum to binary string
    char* checksumBinary = malloc(CHECKSUM_BITS + 1);
    for (i = 0; i < CHECKSUM_BITS; i++)
    {
        checksumBinary[CHECKSUM_BITS - i - 1] = (sum & (1 << i)) ? '1' : '0';
    }
    checksumBinary[CHECKSUM_BITS] = '\0';

    return checksumBinary;
}

int main()
{
    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    char buffer[MAX_BUFFER_SIZE];

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    memset(&clientAddr, 0, sizeof(clientAddr));

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(12345); // Choose a suitable port number

    // Bind the socket to the server address
    if (bind(sockfd, (const struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        char* checksumBinary;
        unsigned int clientAddrLen = sizeof(clientAddr);

        memset(buffer, 0, MAX_BUFFER_SIZE);

        // Receive binary code from client
        int len = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, MSG_WAITALL,
                           (struct sockaddr*)&clientAddr, &clientAddrLen);

        if (len < 0)
        {
            perror("Error in receiving message");
            exit(EXIT_FAILURE);
        }

        // Calculate checksum and get as binary string
        checksumBinary = calculateChecksum(buffer);

        printf("Received Binary Code: %s\n", buffer);
        printf("Calculated Checksum: %s\n", checksumBinary);

        // Send the checksum back to the client
        if (sendto(sockfd, checksumBinary, strlen(checksumBinary), 0,
                   (struct sockaddr*)&clientAddr, clientAddrLen) < 0)
        {
            perror("Error in sending message");
            exit(EXIT_FAILURE);
        }

        free(checksumBinary);
    }

    close(sockfd);

    return 0;
}
