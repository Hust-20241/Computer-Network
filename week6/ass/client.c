#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 8080
#define MAXLINE 1024
#define TIMEOUT 5

// Updated XOR cipher for encryption/decryption based on data length
void improved_xor_cipher(char *data, const char *key, int key_len, int data_len)
{
    for (int i = 0; i < data_len; i++)
    {
        data[i] ^= key[i % key_len]; // Use key in a cyclic manner
    }
}

int main()
{
    int sockfd;
    char buffer[MAXLINE];
    char *response = "Response from client";
    struct sockaddr_in servaddr;
    char *server_ip = "127.0.0.1"; // Server IP address

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Fill server information (IP and port)
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    inet_pton(AF_INET, server_ip, &servaddr.sin_addr);

    // Connect to the server to optimize UDP communication
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Connect failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Send initial message to server
    send(sockfd, response, strlen(response), 0);
    printf("Message sent to server.\n");

    // Use select() to handle timeout
    fd_set readfds;
    struct timeval tv;
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);

    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;

    // Wait for server response
    int activity = select(sockfd + 1, &readfds, NULL, NULL, &tv);
    if (activity == -1)
    {
        perror("select error");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    else if (activity == 0)
    {
        printf("Timeout: No response from server.\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Receive encrypted message from server
    int n = recv(sockfd, buffer, MAXLINE, 0);
    buffer[n] = '\0';

    // XOR decryption with correct data length
    char key[] = "ComplexKey";
    int key_len = strlen(key);
    improved_xor_cipher(buffer, key, key_len, n);

    printf("Decrypted message from server: %s\n", buffer);

    while (1)
    {
        char response[MAXLINE];

        // Get user input for the message
        printf("Enter message to send to server: ");
        fgets(response, MAXLINE, stdin);
        response[strcspn(response, "\n")] = '\0';

        // Send the message to the server
        send(sockfd, response, strlen(response), 0);
        printf("Message sent to server.\n");

        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        tv.tv_sec = TIMEOUT;
        tv.tv_usec = 0;

        activity = select(sockfd + 1, &readfds, NULL, NULL, &tv);
        if (activity == -1)
        {
            perror("select error");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        else if (activity == 0)
        {
            printf("Timeout: No response from server.\n");
            continue;
        }

        n = recv(sockfd, buffer, MAXLINE, 0);
        buffer[n] = '\0';

        // Decrypt response from server
        improved_xor_cipher(buffer, key, key_len, n);
        printf("Decrypted message from server: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}