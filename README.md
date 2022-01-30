# SK2_ConnectFour

## Description 
Client-Server ConnectFour game for 2 players.  
The server is written in C using Socket.h and pthread.h  
The client has GUI and is implemented in Python using PyGame and Socket libraries  
All logic is handled by the server

## Protocol
### Server -> Clients
Server sends 2 byte "codes", with the second byte being EOL  
The first byte can be one of the following:  
* '0' - the server validated client's move and sends '0' to both players to signal that they can proceed to the next move.
* '1' - the server sends '1' as a response to invalid move.
* '2' - the server sends '2' when the game is won by a player
* '3' - the server sends '3' when the game is lost by a player
* '9' - if one player aborts the match, server sends '9' to the other

### Client -> Server
Clients send three byte messages, consisting of two digits and EOL  
Two digits are the number of column on which a player wants to drop their chip