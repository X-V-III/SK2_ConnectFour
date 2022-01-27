#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define PORT 8080
#define server_address localhost

// to pass socket descriptor to thread
struct thread_data_t
{
    int connectionSocket;
};

void *ThreadBehavior(void *t_data)
{
    pthread_detach(pthread_self());

    struct thread_data_t *th_data = (struct thread_data_t*)t_data;
    int client_socket = (*th_data).connectionSocket;

    char buf[1024];
    memset(buf, 0, 1024);
    read(client_socket, buf, sizeof(buf));

    printf("Connection established on thread %ld by %d\n", pthread_self(), client_socket);

    // exit condition
    while (1)
    {
        memset(buf, 0, 1024);
        read((*th_data).connectionSocket, buf, sizeof(buf));
        if (strcmp(buf, "exit") == 0)
        {
            printf("Client %d closes connection\n", client_socket);
            break;
        }
        
        printf("Message from %d: %s\n", client_socket, buf);
    }
    
    printf("Thread %ld exits\n", pthread_self());
    pthread_exit(NULL);
}

//funkcja obsługująca połączenie z nowym klientem
void handleConnection(int connection_socket_descriptor) 
{
    int create_result = 0;

    pthread_t thread1;
    
    struct thread_data_t *t_data = malloc(sizeof(struct thread_data_t));
    (*t_data).connectionSocket = connection_socket_descriptor;

    create_result = pthread_create(&thread1, NULL, ThreadBehavior, (void *)t_data);
    if (create_result){
       printf("Can't create thread, code: %d\n", create_result);
       exit(-1);
    }
}

int main(int argc, char *argv[])
{
	int server_socket_descriptor, connection_socket_descriptor;
	int opt = 1;
	int player1 = 0, player2 = 0;

	struct sockaddr_in server_address;
	int addrlen = sizeof(server_address);

	char buffer[1024] = {0};
	char *message = "Message from server";

	// Creating socket file descriptor
    if ((server_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        printf("\nSocket creation failed\n");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(struct sockaddr));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);
    if (setsockopt(server_socket_descriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        printf("\nSetsockopt failed\n");
        exit(EXIT_FAILURE);
    }

    
    // Binding socket
    if (bind(server_socket_descriptor, (struct sockaddr *)&server_address, sizeof(server_address))<0)
    {
        printf("\nBind failed\n");
        exit(EXIT_FAILURE);
    }

    // Waiting for connection
    if (listen(server_socket_descriptor, 5) < 0)
    {
        printf("\nListen failed\n");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        if (player1 == 0 || player2 == 0)
        {
            connection_socket_descriptor = accept(server_socket_descriptor, NULL, NULL);
            if (connection_socket_descriptor < 0)
            {
               fprintf(stderr, "%s: Błąd przy próbie utworzenia gniazda dla połączenia.\n", argv[0]);
               exit(1);
            }

            if (player1 == 0)
            {
                player1 = connection_socket_descriptor;
            }
            else
            {
                player2 = connection_socket_descriptor;
            }

            handleConnection(connection_socket_descriptor);
        }
    }

    close(server_socket_descriptor);
	return 0;
}