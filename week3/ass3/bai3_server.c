#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Custom sock_ntop function to convert a socket address into a string (IP and port)
char *sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
    static char str[128];
    char portstr[8];

    if (sa->sa_family == AF_INET)
    {
        struct sockaddr_in *sin = (struct sockaddr_in *)sa;
        if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
            return NULL;
        snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
        strcat(str, portstr);
        return str;
    }

    return NULL;
}

// Function to read a line (up to \n) from a socket descriptor
// Function to read a line (up to \n) from a descriptor
ssize_t readline(int fd, void *vptr, size_t maxlen)
{
    ssize_t n, rc;
    char c, *ptr;

    ptr = vptr;
    for (n = 1; n < maxlen; n++)
    {
        if ((rc = read(fd, &c, 1)) == 1)
        {
            *ptr++ = c;
            if (c == '\n')
            {
                break; // Stop at newline
            }
        }
        else if (rc == 0)
        {
            if (n == 1)
            {
                return 0; // No data read
            }
            else
            {
                break; // Some data was read
            }
        }
        else
        {
            return -1; // Error in read
        }
    }
    *ptr = 0;
    return n;
}

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    const char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind to address and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 10) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept a connection from the client
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    printf("Client connected from %s\n", sock_ntop((struct sockaddr *)&address, (socklen_t)addrlen));

    // Read the message from the client
    ssize_t n;
    while ((n = readline(new_socket, buffer, BUFFER_SIZE)) > 0)
    {
        buffer[n] = '\0';
        printf("Received message: %s", buffer);

        // Send the received message back to the client
        if (write(new_socket, buffer, n) < 0)
        {
            perror("Write failed");
            exit(EXIT_FAILURE);
        }
    }

    if (n < 0)
    {
        perror("Readline failed");
        exit(EXIT_FAILURE);
    }

    close(new_socket);
    close(server_fd);

    return 0;
}