#!/usr/bin/env python3

import socket

HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 8080        # The port used by the server

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    print('Connected to server')
    #mess = s.recv(1024)
    #print(repr(mess))

    code = ""

    code = s.recv(1)
    print("Match started as Player " + repr(code)[2])

    myPlayerNumber = code
    currentPlayerTurn = b'1'

    if myPlayerNumber == currentPlayerTurn:
        print("[Your turn]")

        myCommand = input("Enter command: ").encode()
        s.sendall(myCommand)

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
                opponentCommand = s.recv(1024)
                print(repr(opponentCommand))

                print("[Your turn]")

                myCommand = input("Enter command: ").encode()
                s.sendall(myCommand)

            else:
                print("[Opponent turn]")

        # bad command
        if code == b'1':
            pass
    
        if code == b'9':
            break

        