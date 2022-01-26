CC = gcc

all: server client

server: server.c
	clear
	$(CC) -pthread -Wall server.c -o server

client: client.c
	clear
	$(CC) -pthread -Wall client.c -o client