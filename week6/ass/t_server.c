// udp_server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <time.h>

#define PORT 8080
#define MAXLINE 1024
#define TIMEOUT 5

// XOR cipher for encryption/decryption
void xor_cipher(char *data, const char *key, int key_len, int data_len)
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
    char *messages[] = {
        "Message 1 from server",
        "Message 2 from server",
        "Message 3 from server",
        "Message 4 from server",
        "Message 5 from server"};
    int num_messages = sizeof(messages) / sizeof(messages[0]);
    struct sockaddr_in servaddr, cliaddr;
    fd_set readfds;
    struct timeval tv;
    char key[] = "ComplexKey"; // Key used for XOR encryption
    int key_len = strlen(key);

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Bind socket to IP and port
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server is running...\n");

    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        tv.tv_sec = TIMEOUT;
        tv.tv_usec = 0;

        // Wait for activity on the socket
        int activity = select(sockfd + 1, &readfds, NULL, NULL, &tv);

        if (activity == -1)
        {
            perror("select error");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        else if (activity == 0)
        {
            printf("Timeout: No activity from clients in the last %d seconds.\n", TIMEOUT);
            continue;
        }

        if (FD_ISSET(sockfd, &readfds))
        {
            socklen_t len = sizeof(cliaddr);
            int n = recvfrom(sockfd, buffer, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);
            buffer[n] = '\0';
            xor_cipher(buffer, key, key_len, n);

            printf("Received message from client: %s\n", buffer);

            // Select a random message to send to client
            int random_index = rand() % num_messages;
            char *selected_message = messages[random_index];

            // Encrypt the message using XOR cipher
            char msg_to_send[MAXLINE];
            strncpy(msg_to_send, selected_message, MAXLINE - 1);
            msg_to_send[MAXLINE - 1] = '\0'; // Ensure null-termination

            // Encrypt the message
            int msg_len = strlen(msg_to_send);
            xor_cipher(msg_to_send, key, key_len, msg_len);

            // Send encrypted message to the client
            sendto(sockfd, msg_to_send, msg_len, 0, (struct sockaddr *)&cliaddr, len);
            printf("Encrypted message sent to client: %s\n", selected_message);
        }
    }

    close(sockfd);
    return 0;
}
