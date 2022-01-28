#!/usr/bin/env python3

import socket
import re

HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 8080        # The port used by the server

def isGoodCommand(command):
    if (len(command) != 5):
        return 0
    pattern = re.compile("^[0-9][0-9];[0-9][0-9]$")
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

    if myPlayerNumber == currentPlayerTurn:
        print("[Your turn]")

        myCommand = input("Enter command: ")
        while (not isGoodCommand(myCommand)):
            myCommand = input("Invalid command, enter new command: ")
        s.sendall(myCommand.encode())

    else:
        print("[Opponent turn]")

    while 1:
        code = s.recv(1)
        print("Server response code: " + repr(code))

        # next turn
        if code == b'0':
            if currentPlayerTurn == b'1':
                currentPlayerTurn = b'2'
            else:
                currentPlayerTurn = b'1'

            if myPlayerNumber == currentPlayerTurn:
                opponentCommand = s.recv(5)
                print(repr(opponentCommand))

                print("[Your turn]")

                myCommand = input("Enter command: ")
                while (not isGoodCommand(myCommand)):
                    myCommand = input("Invalid command, enter new command: ")
                s.sendall(myCommand.encode())

            else:
                print("[Opponent turn]")

        # bad command
        if code == b'1':
            print("Invalid command")
            myCommand = input("Enter new command: ").encode()
            s.sendall(myCommand)
    
        if code == b'9':
            break

        