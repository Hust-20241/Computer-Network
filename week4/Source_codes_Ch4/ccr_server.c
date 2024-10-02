// concurrent_server.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/wait.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_QUESTIONS 10

char *questions[MAX_QUESTIONS] = {
    "7 + 3 = ?\nA. 8\nB. 9\nC. 10\nD. 11",
    "15 - 6 = ?\nA. 7\nB. 8\nC. 9\nD. 10",
    "6 * 4 = ?\nA. 20\nB. 22\nC. 24\nD. 26",
    "20 / 5 = ?\nA. 3\nB. 4\nC. 5\nD. 6",
    "12 + 7 = ?\nA. 17\nB. 18\nC. 19\nD. 20",
    "50 - 25 = ?\nA. 23\nB. 24\nC. 25\nD. 26",
    "9 * 3 = ?\nA. 25\nB. 26\nC. 27\nD. 28",
    "18 / 2 = ?\nA. 7\nB. 8\nC. 9\nD. 10",
    "25 + 15 = ?\nA. 40\nB. 41\nC. 42\nD. 43",
    "30 - 12 = ?\nA. 17\nB. 18\nC. 19\nD. 20"};

char correct_answers[MAX_QUESTIONS] = {'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'A', 'B'};

// Signal handler to prevent zombie processes
void sigchld_handler(int sig)
{
    (void)sig; // Ignore unused parameter warning
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
}

void handle_client(int connfd)
{
    char buffer[BUFFER_SIZE];
    int n = 0;

    for (int i = 0; i < MAX_QUESTIONS; i++)
    {
        if (questions[i] != NULL)
        {
            send(connfd, questions[i], strlen(questions[i]), 0);
        }
        else
        {
            snprintf(buffer, sizeof(buffer), "Question %d does not exist.\n", i + 1);
            send(connfd, buffer, strlen(buffer), 0);
        }

        memset(buffer, 0, BUFFER_SIZE);
        read(connfd, buffer, sizeof(buffer));

        if (buffer[0] == correct_answers[i])
        {
            n++;
        }
    }
    snprintf(buffer, sizeof(buffer), "Your final score is: %d/%d\n", n, MAX_QUESTIONS);
    send(connfd, buffer, strlen(buffer), 0);

    printf("Client's score: %d\n", n);

    close(connfd);
    exit(0);
}

int main()
{
    int listenfd, connfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    pid_t pid;

    // Create the listening socket
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setup the server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the listening socket to the specified port
    if (bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(listenfd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(listenfd, 5) < 0)
    {
        perror("Listen failed");
        close(listenfd);
        exit(EXIT_FAILURE);
    }

    // Handle SIGCHLD to prevent zombie processes
    signal(SIGCHLD, sigchld_handler);

    printf("Server is listening on port %d...\n", PORT);

    // Server loop to accept multiple clients
    while (1)
    {
        // Accept an incoming connection
        connfd = accept(listenfd, (struct sockaddr *)&client_addr, &addr_len);
        if (connfd < 0)
        {
            perror("Accept failed");
            continue;
        }

        // Fork a child process to handle the client
        pid = fork();
        if (pid < 0)
        {
            perror("Fork failed");
            close(connfd);
        }
        else if (pid == 0)
        {
            // Child process: handle the client
            close(listenfd); // Close the listening socket in the child process
            handle_client(connfd);
            close(connfd);
            exit(0);
        }
        else
        {
            // Parent process: continue accepting new clients
            close(connfd); // Close the client socket in the parent process
        }
    }

    return 0;
}
