#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024

int main()
{
    int sockfd;
    struct sockaddr_in serverAddr;

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345); // Server port number
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP address

    char binaryCode[16];

    printf("Enter a 15-digit binary code: ");
    fgets(binaryCode, sizeof(binaryCode), stdin);

    // Send the binary code to the server
    if (sendto(sockfd, binaryCode, strlen(binaryCode), 0,
               (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Error in sending message");
        exit(EXIT_FAILURE);
    }

    char checksumBinary[6]; // 5 bits + null terminator
    memset(checksumBinary, 0, sizeof(checksumBinary));

    // Receive the checksum from the server
    if (recvfrom(sockfd, checksumBinary, sizeof(checksumBinary), MSG_WAITALL,
                 NULL, NULL) < 0)
    {
        perror("Error in receiving message");
        exit(EXIT_FAILURE);
    }

    printf("Received Checksum: %s\n", checksumBinary);

    close(sockfd);

    return 0;
}
