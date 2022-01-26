#include<stdio.h>
#include<pthread.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#define PORT 8080

int main(int argc, char const *argv[])
{
	int client_fd = 0;
	int readed_value;

	struct sockaddr_in serv_addr;

	char buffer[1024] = {0};
	char *message = "Message from client";

	if (argc < 2)
	{
		printf("\nClient usage: ./client addr\n");
        exit(EXIT_FAILURE);
	}
	char* client_addr = argv[1];

	// Creating socket file descriptor
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        printf("\nSocket creation failed\n");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

     // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, client_addr, &serv_addr.sin_addr) <= 0) 
    {
        printf("\nInvalid address / Address not supported\n");
        exit(EXIT_FAILURE);
    }

    // Send connection request
    if (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed\n");
       	exit(EXIT_FAILURE);
    }

    send(client_fd, message, strlen(message) , 0 );
    printf("Hello message sent\n");
    readed_value = read(client_fd , buffer, 1024);
    printf("Message accepted: %s\n", buffer);

    return 0;
}