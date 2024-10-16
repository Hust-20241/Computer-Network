#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int main()
{
    int server_fd, new_socket, client_sockets[MAX_CLIENTS], max_sd, activity, i;
    struct sockaddr_in address;
    int opt = 1;
    fd_set readfds;
    char buffer[BUFFER_SIZE];
    sigset_t sigmask;

    // Initialize all client sockets to 0
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        client_sockets[i] = 0;
    }

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Define server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %d...\n", PORT);

    // Initialize the signal mask
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGINT);
    sigaddset(&sigmask, SIGTERM);

    while (1)
    {
        // Clear the socket set
        FD_ZERO(&readfds);

        // Add server socket to set
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        // Add child sockets to set
        for (i = 0; i < MAX_CLIENTS; i++)
        {
            int sd = client_sockets[i];
            if (sd > 0)
            {
                FD_SET(sd, &readfds);
            }
            if (sd > max_sd)
            {
                max_sd = sd;
            }
        }

        // Use pselect instead of select
        struct timespec timeout;
        timeout.tv_sec = 5; // 5 seconds timeout
        timeout.tv_nsec = 0;

        activity = pselect(max_sd + 1, &readfds, NULL, NULL, &timeout, &sigmask);

        if (activity < 0)
        {
            if (errno == EINTR)
            {
                // A signal was caught
                printf("pselect interrupted by signal\n");
                continue;
            }
            else
            {
                perror("pselect error");
                exit(EXIT_FAILURE);
            }
        }
        else if (activity == 0)
        {
            // Timeout occurred
            // printf("pselect timeout\n");
            continue;
        }

        // If something happened on the master socket, then it's an incoming connection
        if (FD_ISSET(server_fd, &readfds))
        {
            socklen_t addrlen = sizeof(address);
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("New connection, socket fd is %d, ip is : %s, port : %d\n",
                   new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // Add new socket to array of sockets
            for (i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_sockets[i] == 0)
                {
                    client_sockets[i] = new_socket;
                    printf("Adding to list of sockets as %d\n", i);
                    break;
                }
            }
        }

        // Check for IO operations on other sockets
        for (i = 0; i < MAX_CLIENTS; i++)
        {
            int sd = client_sockets[i];

            if (FD_ISSET(sd, &readfds))
            {
                int valread;
                if ((valread = read(sd, buffer, BUFFER_SIZE)) == 0)
                {
                    // Client disconnected
                    socklen_t addrlen = sizeof(address);
                    getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                    printf("Client disconnected, ip %s, port %d\n",
                           inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    close(sd);
                    client_sockets[i] = 0;
                }
                else
                {
                    // Process the incoming message
                    buffer[valread] = '\0';
                    printf("Message from client %d: %s\n", i, buffer);
                    for (int j = 0; j < MAX_CLIENTS; j++)
                    {
                        if (client_sockets[j] > 0 && client_sockets[j] != sd)
                        {
                            send(client_sockets[j], buffer, strlen(buffer), 0);
                        }
                    }
                    send(sd, "Message received", 16, 0);
                }
            }
        }
    }

    return 0;
}