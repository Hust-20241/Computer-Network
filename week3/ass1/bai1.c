#include <stdio.h>
#include <string.h>
#include <arpa/inet.h> // For inet_pton and sockaddr_in

int main()
{

    char *ip_address = "192.168.1.1";
    struct sockaddr_in server_addr;

    // Zero out the sockaddr_in structure
    memset(&server_addr, 0, sizeof(server_addr));

    // Set the address family to IPv4
    server_addr.sin_family = AF_INET;

    // Set the port number (use htons to convert to network byte order)
    server_addr.sin_port = htons(8080);

    // Set the IP address (use inet_pton to convert from text to binary form)
    if (inet_pton(AF_INET, ip_address, &server_addr.sin_addr) <= 0)
    {
        printf("Invalid address/ Address not supported\n");
        return -1;
    }

    // Print the initialized structure
    printf("sockaddr_in structure initialized:\n");
    printf("  Family      : AF_INET (IPv4)\n");
    printf("  Port        : %d\n", ntohs(server_addr.sin_port)); // convert back to host byte order for printing
    printf("  IP Address  : %s\n", ip_address);

    return 0;
}
