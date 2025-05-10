Minesweeper Game with Server/Client

=========================================================================================================================

Project Summary
This project implements a concurrent thread-based client-server model using C++ for a textbased Minesweeper game in a 
console. The project's goal is to allow multiple clients the ability to play the game off a server which handles 
the game logic and processing. Each client is independent of each other using the thread library from C++ and the server
creates a board object for each client using Object-Oriented Programming.

=========================================================================================================================

Installation and Running the Program

To run the program, you must use Jupyter Hub which is a unix environment. If you are on Windows, this will not work
natively as it uses incompatible libraries.

1. Download and install the files onto Jupyter Hub

2. In the directory, run make to compile the files into server and client executable

3. Start the server with the ./server <port> command. Ex: ./server 8080

4. Connect a client with the ./client <server_ip> <port> command. Ex: ./server localhost 8080

5. Client connection can be ended with a win or lose condition, or ctrl + c

6. To terminate the server, do ctrl + c in the console

=========================================================================================================================

YouTube Link: https://youtu.be/SLLX0Z70cJs