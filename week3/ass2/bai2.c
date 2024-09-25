#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

void print_binary(unsigned char byte)
{
    for (int i = 7; i >= 0; i--)
    {
        printf("%d", (byte >> i) & 1); // Extract each bit and print
    }
    printf(" ");
}

void convert_ipv4()
{
    char ip_str[INET_ADDRSTRLEN];
    printf("Scanf ip address: ");
    scanf("%s", ip_str);
    struct in_addr ipv4_addr; // Structure to hold the IPv4 address

    // Convert the IPv4 address from string to binary form
    if (inet_pton(AF_INET, ip_str, &ipv4_addr) == 1)
    {
        printf("inet_pton (IPv4): Successfully converted IP address: %s\n", ip_str);
    }
    else
    {
        printf("inet_pton (IPv4): Failed to convert IP address: %s\n", ip_str);
        exit(EXIT_FAILURE);
    }

    unsigned char *bytes = (unsigned char *)&ipv4_addr;
    printf("IP address in binary form: ");
    for (int i = 0; i < sizeof(ipv4_addr); i++)
    {
        print_binary(bytes[i]);
    }
    printf("\n");

    char ip_str_converted[INET_ADDRSTRLEN]; // Buffer to hold the converted IP address back to string

    // Convert the binary IPv4 address back to string form
    if (inet_ntop(AF_INET, &ipv4_addr, ip_str_converted, INET_ADDRSTRLEN))
    {
        printf("inet_ntop (IPv4): Converted back to string IP address: %s\n", ip_str_converted);
    }
    else
    {
        printf("inet_ntop (IPv4): Failed to convert IP address back to string\n");
        exit(EXIT_FAILURE);
    }
}

int main()
{
    const char *ipv4_str = "192.168.1.1"; // Example IPv4 address

    // Demonstrate IPv4 conversion
    printf("IPv4 Conversion:\n");
    convert_ipv4();

    printf("\n");

    return 0;
}
