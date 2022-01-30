#!/usr/bin/env python3

import numpy as np
import pygame
import sys
import math
import socket
import re

HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 8080        # The port used by the server

BLUE = (0,0,255)
BLACK = (0,0,0)
RED = (255,0,0)
WHITE = (255,255,255)

ROW_COUNT = 6
COLUMN_COUNT = 7
SQUARESIZE = 60

# GAME METHODS ****************

def create_board():
    board = np.zeros((ROW_COUNT,COLUMN_COUNT))
    return board

def drop_piece(board, row, col, piece):
    board[row][col] = piece

def get_next_open_row(board, col):
    for r in range(ROW_COUNT):
        if board[r][col] == 0:
            return r

def draw_board(board):
    for c in range(COLUMN_COUNT):
        for r in range(ROW_COUNT):
            pygame.draw.rect(screen, BLACK, (c*SQUARESIZE, r*SQUARESIZE+SQUARESIZE, SQUARESIZE, SQUARESIZE))
            pygame.draw.circle(screen, WHITE, (int(c*SQUARESIZE+SQUARESIZE/2), int(r*SQUARESIZE+SQUARESIZE+SQUARESIZE/2)), RADIUS)
    
    for c in range(COLUMN_COUNT):
        for r in range(ROW_COUNT):      
            if board[r][c] == 1:
                pygame.draw.circle(screen, RED, (int(c*SQUARESIZE+SQUARESIZE/2), height-int(r*SQUARESIZE+SQUARESIZE/2)), RADIUS)
            elif board[r][c] == 2: 
                pygame.draw.circle(screen, BLUE, (int(c*SQUARESIZE+SQUARESIZE/2), height-int(r*SQUARESIZE+SQUARESIZE/2)), RADIUS)
    pygame.display.update()

def parseAndExecuteMove(board, move, number):
    col = int(move) - 1
    row = get_next_open_row(board, col)
    drop_piece(board, row, col, number)
    draw_board(board)

def getCommand(board):
    command = ""
    while(command == ""):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                command = "00"

            if event.type == pygame.MOUSEMOTION:
                pygame.draw.rect(screen, BLACK, (0,0, width, SQUARESIZE))
                posx = event.pos[0]
                pygame.draw.circle(screen, myColor, (posx, int(SQUARESIZE/2)), RADIUS)

            pygame.display.update()

            if event.type == pygame.MOUSEBUTTONDOWN:
                pygame.draw.rect(screen, BLACK, (0,0, width, SQUARESIZE))
                posx = event.pos[0]
                col = int(math.floor(posx/SQUARESIZE))
                command = "0" + str(col+1)
    return command

# *****************************

if __name__ == "__main__":

    if (len(sys.argv) != 3):
        print("Usage: python3 client.py \'SERVER_ADDRESS\' \'PORT\'")
        exit(1)

    HOST = sys.argv[1]
    PORT = int(sys.argv[2])

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        print('Connected to server')

        code = ""

        code = s.recv(1)
        print("Match started as Player " + repr(code)[2])

        myPlayerNumber = code.decode("utf-8")
        currentPlayerTurn = '1'

        # GAME SETUP *******************

        board = create_board()
        
        myColor = RED
        if (myPlayerNumber == '2'):
            myColor = BLUE
        opponentNumber = '2'
        if (myPlayerNumber == '2'):
            opponentNumber = '1'

        pygame.init()
        pygame.display.set_caption("Player " + myPlayerNumber)

        width = COLUMN_COUNT * SQUARESIZE
        height = (ROW_COUNT+1) * SQUARESIZE

        size = (width, height)

        RADIUS = int(SQUARESIZE/2 - 5)

        screen = pygame.display.set_mode(size)
        draw_board(board)
        pygame.display.update()

        myfont = pygame.font.SysFont("monospace", 35)

        # ******************************

        while(1):
            if (currentPlayerTurn == myPlayerNumber):
                print("[Your turn]")
                myCommand = getCommand(board)
                s.sendall(myCommand.encode())
                code = s.recv(1).decode("utf-8")

                if (code == '9'):
                    print("Exitting")
                    break;

                if (code == '1'):
                    print("Invalid move")
                    myCommand = getCommand(board)
                    s.sendall(myCommand.encode())
                    code = s.recv(1).decode("utf-8")

                if (code == '2'):
                    parseAndExecuteMove(board, myCommand, int(myPlayerNumber))
                    print("You Win!")
                    label = myfont.render("Player " + myPlayerNumber + " wins!", 1, WHITE)
                    screen.blit(label, (20,20))
                    draw_board(board)
                    pygame.time.wait(5000)
                    break;

                if (code == '0'):
                    parseAndExecuteMove(board, myCommand, int(myPlayerNumber))
                    currentPlayerTurn = opponentNumber

            else: # if (currentPlayerTurn == opponentNumber):
                print("[Opponent turn]")
                code = s.recv(1).decode("utf-8")

                if (code == '9'):
                    print("Exitting")
                    break;

                if (code == '3'):
                    opponentMove = s.recv(2).decode("utf-8")
                    parseAndExecuteMove(board, opponentMove, int(opponentNumber))
                    print("You Lost :(")
                    label = myfont.render("Player " + opponentNumber + " wins!", 1, WHITE)
                    screen.blit(label, (20,20))
                    draw_board(board)
                    pygame.time.wait(5000)
                    break;

                if (code == '0'):
                    opponentMove = s.recv(2).decode("utf-8")
                    parseAndExecuteMove(board, opponentMove, int(opponentNumber))
                    currentPlayerTurn = myPlayerNumber