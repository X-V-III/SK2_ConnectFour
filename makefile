CC = gcc

all: server

server: server.c
	clear
	$(CC) -pthread -Wall server.c -o server