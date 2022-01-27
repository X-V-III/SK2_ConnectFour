#!/usr/bin/env python3

import socket

HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 8080        # The port used by the server

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    mess = b'Hello, server'
    s.sendall(mess)
    #mess = s.recv(1024)
    #print(repr(mess))

    while 1:
        mess = input("Enter message: ").encode()
        s.sendall(mess)
        if (mess == b'exit'):
            break