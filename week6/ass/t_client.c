// udp_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

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
    struct sockaddr_in servaddr;
    fd_set readfds;
    struct timeval tv;
    char *server_ip = "127.0.0.1"; // Server IP address
    char key[] = "ComplexKey";     // Key used for XOR encryption
    int key_len = strlen(key);
    socklen_t len = sizeof(servaddr);

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Fill server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);

    // Convert IP address from text to binary form
    if (inet_pton(AF_INET, server_ip, &servaddr.sin_addr) <= 0)
    {
        perror("Invalid address or address not supported");
        exit(EXIT_FAILURE);
    }

    printf("Client is running....\n");

    while (1)
    {
        // Nhập tin nhắn từ bàn phím
        printf("Enter message: ");
        fgets(buffer, MAXLINE, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // Loại bỏ ký tự newline

        // Mã hóa tin nhắn trước khi gửi
        int msg_len = strlen(buffer);
        xor_cipher(buffer, key, key_len, msg_len);

        // Gửi tin nhắn đã mã hóa tới server
        sendto(sockfd, buffer, msg_len, 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
        printf("Message sent to server.\n");

        // Thiết lập select để chờ phản hồi từ server với timeout
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        tv.tv_sec = TIMEOUT;
        tv.tv_usec = 0;

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
            continue;
        }

        // Nhận phản hồi từ server
        int n = recvfrom(sockfd, buffer, MAXLINE, 0, NULL, NULL);
        buffer[n] = '\0';

        // Giải mã phản hồi từ server
        xor_cipher(buffer, key, key_len, n);
        printf("Decrypted message from server: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
