#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 8080
#define BUFFER_SIZE 100

int main()
{
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    fd_set read_fds;

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Change this if needed

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server.\n");
    printf("Enter message to send to server. \n");

    while (1)
    {
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds); // Monitor user input
        FD_SET(sockfd, &read_fds);       // Monitor server messages

        // Wait for activity
        int activity = select(sockfd + 1, &read_fds, NULL, NULL, NULL);

        // Check for input from the server
        if (FD_ISSET(sockfd, &read_fds))
        {
            int valread = read(sockfd, buffer, BUFFER_SIZE);
            if (valread == 0)
            {
                printf("Server disconnected.\n");
                close(sockfd);
                break;
            }
            buffer[valread] = '\0';
            printf("Message from server: %s\n", buffer);
        }

        if (FD_ISSET(STDIN_FILENO, &read_fds))
        {
            // Read input from the user
            fgets(buffer, sizeof(buffer), stdin);
            // Send the input to the server
            send(sockfd, buffer, strlen(buffer), 0);
        }

        // Optionally, you can receive response from the server (if implemented)
    }

    // Clean up
    close(sockfd);
    return 0;
}
