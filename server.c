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
#define SERVER_IP "localhost"
#define EXIT_COMMADND "00"
#define EXIT_CODE "9"
#define NEXT_TURN_CODE "0"
#define WRONG_COMMAND_CODE "1"
#define YOU_WIN_CODE "2"
#define OPPONENT_WINS_CODE "3"
#define ROW_COUNT 6
#define COLUMN_COUNT 7

struct game_thread_data
{
    int player1_socket;
    int player2_socket;
};

int executeMove(int* board, char* move, int player)
{
    int col = atoi(move) - 1;
    int row;
    for (int i = 0; i < ROW_COUNT; i++)
    {
        if (board[col+i*COLUMN_COUNT] == 0)
            row = i;
    }
    board[col+row*COLUMN_COUNT] = player;
    return 1;
}

int validateMove(int* board, char* move)
{
    int col = atoi(move) - 1;
    if (board[col] != 0) return 0;
    return 1;
}

// returns one dimensional array representing game board
int* createBoard(int x, int y)
{
    int *board = malloc(sizeof(int) * x * y);
    for (int i = 0; i < x*y; i++)
    {
        board[i] = 0;
    }
    return board;
}

int getBoardElement(int* board, int y, int x)
{
    return board[x + y * COLUMN_COUNT];
}

int isWinningState(int* board, int player)
{
    // Check horizontal locations for win
    for (int y = 0; y < ROW_COUNT; ++y)
    {
        for (int x = 0; x < COLUMN_COUNT-3; ++x)
        {
            if (getBoardElement(board, y, x) == player &&
                    getBoardElement(board, y, x+1) == player &&
                    getBoardElement(board, y, x+2) == player &&
                    getBoardElement(board, y, x+3) == player)
            {
                return 1;
            }
        }
    }

    // Check vertical locations for win
    for (int y = 0; y < ROW_COUNT - 3; ++y)
    {
        for (int x = 0; x < COLUMN_COUNT; ++x)
        {
            if (getBoardElement(board, y, x) == player &&
                    getBoardElement(board, y+1, x) == player &&
                    getBoardElement(board, y+2, x) == player &&
                    getBoardElement(board, y+3, x) == player)
            {
                return 1;
            }
        }
    }

    // Check positively sloped diagonals
    for (int y = 0; y < ROW_COUNT-3; ++y)
    {
        for (int x = 0; x < COLUMN_COUNT-3; ++x)
        {
            if (getBoardElement(board, y, x) == player &&
                    getBoardElement(board, y+1, x+1) == player &&
                    getBoardElement(board, y+2, x+2) == player &&
                    getBoardElement(board, y+3, x+3) == player)
            {
                return 1;
            }
        }
    }

    // Check negatively sloped diagonals
    for (int y = 3; y < ROW_COUNT; ++y)
    {
        for (int x = 0; x < COLUMN_COUNT-3; ++x)
        {
            if (getBoardElement(board, y, x) == player &&
                    getBoardElement(board, y-1, x+1) == player &&
                    getBoardElement(board, y-2, x+2) == player &&
                    getBoardElement(board, y-3, x+3) == player)
            {
                return 1;
            }
        }
    }
    return 0;
}

void printBoard(int *board)
{
    for (int i = 0; i < ROW_COUNT; i++)
    {
        for (int j = 0; j < COLUMN_COUNT; j++)
        {
            printf("%d ", board[j+i*COLUMN_COUNT]);
        }
        printf("\n");
    }
}


void *GameThread(void *game_thread_data_t)
{
    pthread_detach(pthread_self());

    int* board = createBoard(COLUMN_COUNT, ROW_COUNT);
    
    // stores the current player's turn, 1 by default
    int player_turn = 1;

    struct game_thread_data *gthread_data = (struct game_thread_data*)game_thread_data_t;
    int player1_socket = (*gthread_data).player1_socket;
    int player2_socket = (*gthread_data).player2_socket;

    printf("Game thread started with socket descriptors: %d, %d\n", player1_socket, player2_socket);

    char command[3];

    char code[2];
    strcpy(code, WRONG_COMMAND_CODE);
    send(player1_socket, &code, strlen(code), 0);
    strcpy(code, "2");
    send(player2_socket, &code, strlen(code), 0);

    int temp_self = player1_socket;
    int temp_opponent = player2_socket;

    while(1)
    {
        memset(command, 0, 2);
        strcpy(code, NEXT_TURN_CODE);

        read(temp_self, command, sizeof(command));
        //printf("Command from player %d: %s\n", player_turn, command);

        if (strcmp(command, EXIT_COMMADND) == 0)
        {
            printf("Client %d aborts the match for descriptors %d and %d\n", player_turn, player1_socket, player2_socket);
            strcpy(code, EXIT_CODE);
            send(temp_self, &code, strlen(code), 0);
            send(temp_opponent, &code, strlen(code), 0);
            break;
        }
        else if (validateMove(board, command))
        {
            executeMove(board, command, player_turn);
            //printBoard(board);

            if (isWinningState(board, player_turn))
            {
                strcpy(code, YOU_WIN_CODE);
                send(temp_self, &code, strlen(code), 0);
                strcpy(code, OPPONENT_WINS_CODE);
                send(temp_opponent, &code, strlen(code), 0);
                send(temp_opponent, &command, strlen(command), 0);
                break;
            }

            strcpy(code, NEXT_TURN_CODE);
            send(temp_self, &code, strlen(code), 0);
            send(temp_opponent, &code, strlen(code), 0);
            send(temp_opponent, &command, strlen(command), 0);

            if (player_turn == 1) player_turn = 2;
            else player_turn = 1;

            if (player_turn == 1)
            {
                temp_self = player1_socket;
                temp_opponent = player2_socket;
            }
            else
            {
                temp_self = player2_socket;
                temp_opponent = player1_socket;
            }
        }
        else
        {
            printf("Client %d sent invalid command\n", player_turn);
            strcpy(code, WRONG_COMMAND_CODE);
            send(temp_self, &code, strlen(code), 0);
        }
    }

    printf("Game thread for descriptors %d and %d exits\n", player1_socket, player2_socket);
    close(player1_socket);
    close(player2_socket);
    pthread_exit(NULL);
}

//funkcja obs??uguj??ca po????czenie z nowym klientem
void handleConnections(int server_socket_descriptor) 
{
    pthread_t game_thread;
    int player1 = 0;
    int player2 = 0;
    int connection_socket_descriptor = 0;

    while(1)
    {
        player1 = 0;
        player2 = 0;
        connection_socket_descriptor = 0;

        while(player1 == 0 || player2 == 0)
        {
            connection_socket_descriptor = accept(server_socket_descriptor, NULL, NULL);

            if (connection_socket_descriptor < 0)
            {
               printf("A player tried to connect, but failed. Code: %d\n", connection_socket_descriptor);
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
            printf("Player connected to descriptor: %d\n", connection_socket_descriptor);
        }

        int create_result = 0;
        
        // prepare socket data for game thread
        struct game_thread_data *game_thread_data_t = malloc(sizeof(struct game_thread_data));
        (*game_thread_data_t).player1_socket = player1;
        (*game_thread_data_t).player2_socket = player2;

        create_result = pthread_create(&game_thread, NULL, GameThread, (void *)game_thread_data_t);
        if (create_result){
           printf("Can't create thread, code: %d\n", create_result);
           exit(-1);
        }

        //pthread_join(game_thread, NULL);
    }

}

int main(int argc, char *argv[])
{
	int server_socket_descriptor;
	int opt = 1;

	struct sockaddr_in server_address;

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

    printf("Server started at: %s:%d\n", SERVER_IP, PORT);

    handleConnections(server_socket_descriptor);

    close(server_socket_descriptor);
	return 0;
}