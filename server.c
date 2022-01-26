#include<stdio.h>
#include<pthread.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#define PORT 8080
#define ADDRESS localhost
#define MAX_CLIENTS 10

int main(int argc, char const *argv[])
{
	int server_fd, new_socket, readed_value;
	int opt = 1;
	int clients_fds[MAX_CLIENTS] = {0};

	struct sockaddr_in address;
	int addrlen = sizeof(address);

	char buffer[1024] = {0};
	char *message = "Message from server";

	// Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        printf("\nSocket creation failed\n");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        printf("\nSetsockopt failed\n");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Binding socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        printf("\nBind failed\n");
        exit(EXIT_FAILURE);
    }

    // Waiting for connection
    if (listen(server_fd, 5) < 0)
    {
        printf("\nListen failed\n");
        exit(EXIT_FAILURE);
    }

    // Accepting first pending connection request 
    if ((clients_fds[0] = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
    {
        printf("\nAccept failed\n");
        exit(EXIT_FAILURE);
    }

    readed_value = read(clients_fds[0], buffer, 1024);
    printf("Message accepted: %s\n",buffer);

    send(clients_fds[0], message , strlen(message) , 0 );
    printf("Message sent\n");

	return 0;
}