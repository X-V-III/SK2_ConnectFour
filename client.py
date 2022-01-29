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
YELLOW = (255,255,0)
WHITE = (255,255,255)

ROW_COUNT = 6
COLUMN_COUNT = 7

# GAME METHODS ****************

def create_board():
    board = np.zeros((ROW_COUNT,COLUMN_COUNT))
    return board

def drop_piece(board, row, col, piece):
    board[row][col] = piece

def is_valid_location(board, col):
    return board[ROW_COUNT-1][col] == 0

def get_next_open_row(board, col):
    for r in range(ROW_COUNT):
        if board[r][col] == 0:
            return r

def print_board(board):
    print(np.flip(board, 0))

def winning_move(board, piece):
    # Check horizontal locations for win
    for c in range(COLUMN_COUNT-3):
        for r in range(ROW_COUNT):
            if board[r][c] == piece and board[r][c+1] == piece and board[r][c+2] == piece and board[r][c+3] == piece:
                return True

    # Check vertical locations for win
    for c in range(COLUMN_COUNT):
        for r in range(ROW_COUNT-3):
            if board[r][c] == piece and board[r+1][c] == piece and board[r+2][c] == piece and board[r+3][c] == piece:
                return True

    # Check positively sloped diaganols
    for c in range(COLUMN_COUNT-3):
        for r in range(ROW_COUNT-3):
            if board[r][c] == piece and board[r+1][c+1] == piece and board[r+2][c+2] == piece and board[r+3][c+3] == piece:
                return True

    # Check negatively sloped diaganols
    for c in range(COLUMN_COUNT-3):
        for r in range(3, ROW_COUNT):
            if board[r][c] == piece and board[r-1][c+1] == piece and board[r-2][c+2] == piece and board[r-3][c+3] == piece:
                return True

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
    if is_valid_location(board, col):
        row = get_next_open_row(board, col)
        drop_piece(board, row, col, number)


# *****************************

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

                draw_board(board)
    return command


def isGoodCommand(command):
    if (len(command) != 2):
        return 0
    pattern = re.compile("^[0-9][0-9]$")
    if (not pattern.match(command)):
        return 0
    return 1

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    print('Connected to server')

    code = ""

    code = s.recv(1)
    print("Match started as Player " + repr(code)[2])

    myPlayerNumber = code
    currentPlayerTurn = b'1'

    # GAME SETUP *******************

    board = create_board()
    print_board(board)
    game_over = False
    myColor = RED
    if (myPlayerNumber == b'2'):
        myColor = BLUE
    myNumber = 1
    if (myPlayerNumber == b'2'):
        myNumber = 2
    opponentNumber = 2
    if (myNumber == 2):
        opponentNumber = 1

    pygame.init()

    SQUARESIZE = 100

    width = COLUMN_COUNT * SQUARESIZE
    height = (ROW_COUNT+1) * SQUARESIZE

    size = (width, height)

    RADIUS = int(SQUARESIZE/2 - 5)

    screen = pygame.display.set_mode(size)
    draw_board(board)
    pygame.display.update()

    myfont = pygame.font.SysFont("monospace", 75)

    # ******************************

    if myPlayerNumber == currentPlayerTurn:
        print("[Your turn]")

        myCommand = getCommand(board)
        parseAndExecuteMove(board, myCommand, myNumber)
        draw_board(board)
        print(f"myCommand: {myCommand}")
        s.sendall(myCommand.encode())

    else:
        print("[Opponent turn]")

    while 1:
        code = s.recv(1).decode("utf-8")
        print("Server response code: " + repr(code))

        # next turn
        if code == '0':

            if currentPlayerTurn == b'1':
                currentPlayerTurn = b'2'
            else:
                currentPlayerTurn = b'1'

            if myPlayerNumber == currentPlayerTurn:
                opponentCommand = s.recv(2).decode("utf-8")
                print(f"opponentCommand: {opponentCommand}")
                parseAndExecuteMove(board, opponentCommand, opponentNumber)
                draw_board(board)

                print("[Your turn]")

                myCommand = getCommand(board)
                parseAndExecuteMove(board, myCommand, myNumber)
                draw_board(board)
                s.sendall(myCommand.encode())

            else:
                print("[Opponent turn]")

        # bad command
        if code == '1':
            print("Invalid command")
            myCommand = input("Enter new command: ").encode()
            s.sendall(myCommand)
    
        if code == '9':
            break