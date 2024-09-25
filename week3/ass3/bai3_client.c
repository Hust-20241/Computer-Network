#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char sendline[BUFFER_SIZE], recvline[BUFFER_SIZE];
    ssize_t n;

    // Creating socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IP addresses from text to binary
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("Invalid address or address not supported\n");
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Connection Failed\n");
        return -1;
    }

    // Send a message to the server
    printf("Enter message: ");
    fgets(sendline, BUFFER_SIZE, stdin);     // Read message from user
    write(sock, sendline, strlen(sendline)); // Send message to server

    // Read the server's response
    if ((n = read(sock, recvline, BUFFER_SIZE)) > 0)
    {
        recvline[n] = '\0'; // Null-terminate the received string
        printf("Server response: %s", recvline);
    }
    else
    {
        perror("Read failed");
    }

    // Close the socket
    close(sock);

    return 0;
}