// server.cpp
#include <iostream>
#include <string>
#include <thread>
#include <map>
#include <mutex>
#include <memory>       
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
#include <cstring>
#include <cerrno>
#include "Board.h"

using namespace std;

mutex clientMutex;
map<int, unique_ptr<Board>> clientBoards;                   // Stores pointers to boards

void ServerFunction(int clientSocket);
string GetColorCode(const string color);

int main(int argc, char **argv) {
    if (argc != 2) {                                        // If command line arguments are not 2, then print and error and return
        cerr << "Usage: " << argv[0] << " <port>\n";
        return 1;
    }

    int port = atoi(argv[1]);                               // TCP port number to listen on
    int listenSock = socket(AF_INET, SOCK_STREAM, 0);       // Create a new socket
    if (listenSock == -1) { 
        cerr << "Socket: " << strerror(errno) << "\n";
        return 1; 
    }

    sockaddr_in serverAddr {};                              // Setting up server structure
    serverAddr.sin_family = AF_INET;                        // IPv4 
    serverAddr.sin_addr.s_addr = INADDR_ANY;                // Any valid address 
    serverAddr.sin_port = htons(port);                      // Host to network byte order

    if (bind(listenSock, (sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {  // Bind socket to port
        cerr << "Bind: " << strerror(errno) << "\n";
        return 1;
    }
    
    if (listen(listenSock, 5) == -1) {                      // Listen for incoming connections
        cerr << "Listen: " << strerror(errno) << "\n";
        return 1;
    }

    cout << "Server is running on port " << port << "...\n";
    while (true) {                                                                  // Accept clients and create a thread for each one
        sockaddr_in clientAddr {};
        socklen_t clientLen = sizeof(clientAddr);
        int socket = accept(listenSock, (sockaddr *)&clientAddr, &clientLen);
        if (socket < 0) { 
            cerr << "Accept: " << strerror(errno) << "\n";
            continue; 
        
        }
        thread thread(ServerFunction, socket);
        thread.detach();
    }
    close(listenSock);                                                              // Close the socket 
    return 0;
}

void ServerFunction(int clientSocket) {
    char buffer[1024];

    // Read initial configuration
    int bytes = read(clientSocket, buffer, sizeof(buffer) - 1);
    if (bytes <= 0) {
        cerr << "Failed to read board configuration\n";
        close(clientSocket);
        return;
    }
    buffer[bytes] = '\0';
    cout << "Server Received: " << buffer << "\n";

    // Parse board config recieved from the client into variables that will create the board object for that client 
    int boardWidth, boardHeight, mineCount;
    string color;
    
    istringstream in(buffer);
    in >> boardWidth >> boardHeight >> mineCount >> color;
    
    if (boardWidth <= 0 || boardHeight <= 0 || mineCount <= 0 || mineCount > boardWidth * boardHeight) {
        cerr << "Invalid board configuration\n";
        close(clientSocket);
        return;
    }
    
    string colorCode = GetColorCode(color);                             // This sets the color of the board for that game

    // Make the Board on the heap and stash it. Creates the board object 
    {
        auto b = make_unique<Board>(boardHeight, boardWidth, mineCount);
        lock_guard<mutex> lock(clientMutex);
        clientBoards[clientSocket] = move(b);
    }

    cout << "Board created for socket " << clientSocket << "\n";

    // Send the very first (all‑hidden) board so client unblocks immediately
    
    ostringstream oss;                                                  // This sends the created board string to the client 
    clientBoards[clientSocket]->RevealedBoard(oss, colorCode);         // to print the board in its intitial state
    string init = oss.str();
    send(clientSocket, init.c_str(), init.size(), 0);
    

    // 5) Now enter the play loop
    bool firstMove = true;
    while (true) {
        cout << "Waiting for client move...\n";
        bytes = read(clientSocket, buffer, sizeof(buffer) - 1);     // Reads in the input from the client 
        if (bytes <= 0) {
            cout << "Client disconnected\n";
            break;
        }
        buffer[bytes] = '\0';
        cout << "Got command: " << buffer << "\n";

        // parse “Dig x y” or “Flag x y”
        istringstream cmd(buffer);
        string word;
        int x, y;
        cmd >> word >> x >> y;

        auto &board = *clientBoards[clientSocket];
        if (word == "Dig") {
            if (firstMove) {
                board.PlaceMines(x,y);
                board.UpdateCounts();
                firstMove = false;
            }
            board.PlayBoard(x, y);
        } else if (word == "Flag") {
            board.ToggleFlag(x, y);
        }

        // send updated board
        
        ostringstream oss;
        board.RevealedBoard(oss, colorCode);
        string state = oss.str();

        // if game over, send final message and break
        if (!board.CheckGameStatus()) {
            state += board.CheckWin() ? "YOU WIN!!!\n" : "YOU LOSE!!!\n";
            send(clientSocket, state.c_str(), state.size(), 0);
            break;
        } else {
            send(clientSocket, state.c_str(), state.size(), 0);
        }
    }

    // 6) teardown
    cout << "Client disconnected\n";
    close(clientSocket);
    {
        lock_guard<mutex> lock(clientMutex);
        clientBoards.erase(clientSocket);
    }
}

// Function to determine the color entered in by the player, returns ANSI string
string GetColorCode(const string color) {
    const string RESET = "\033[0m",
                 BLUE = "\033[34m",
                 RED = "\033[91m",
                 YELLOW = "\033[33m",
                 GREEN = "\033[32m";

    if (color == "Red" || color == "red")        return RED;
    if (color =="Blue" || color == "blue")       return BLUE;
    if (color == "Yellow" || color == "yellow")  return YELLOW;
    if (color == "Green" || color == "green")    return GREEN;
    return RESET;
}
