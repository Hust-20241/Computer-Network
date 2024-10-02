// client.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_QUESTIONS 10

int main()
{
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int n;

    // Create a socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setup the server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Convert IPv4 address from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0)
    {
        perror("Invalid address or address not supported");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server. Type a message and press enter:\n");

    for (int i = 0; i < MAX_QUESTIONS; i++)
    {
        memset(buffer, 0, BUFFER_SIZE);
        read(client_socket, buffer, sizeof(buffer));
        printf("Question %d: %s\n", i + 1, buffer);

        while (1)
        {
            printf("Enter your answer (A, B, C, or D): ");
            // Get input from the user
            fgets(buffer, BUFFER_SIZE, stdin);

            // Remove the newline character if it's present (fgets adds it)
            if (buffer[0] != '\0' && buffer[strlen(buffer) - 1] == '\n')
            {
                buffer[strlen(buffer) - 1] = '\0';
            }

            // Check if the string is empty. If it is, stop the program
            if (buffer[0] == 'A' || buffer[0] == 'B' || buffer[0] == 'C' || buffer[0] == 'D')
            {
                send(client_socket, buffer, strlen(buffer), 0);
                break;
            }
            else
            {
                printf("Invalid answer. Please enter A, B, C, or D.\n");
            }
        }
    }

    // Receive and print the final score
    memset(buffer, 0, BUFFER_SIZE);
    read(client_socket, buffer, sizeof(buffer));
    printf("%s", buffer);

    close(client_socket);

    return 0;
}