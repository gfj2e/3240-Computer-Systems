// client.cpp
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <limits>
#include <cerrno>

using namespace std;

int main(int argc, char **argv) {
    if (argc != 3) {                                                // Error check command line arguments
        cerr << "Usage: " << argv[0] << " <server_ip> <port>\n";
        return 1;
    }

    const char *serverIp = argv[1];                                 // Server IP addess number 
    int port = atoi(argv[2]);                                       // server port number

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);             // Create TCP socket
    if (clientSocket == -1) {
        cerr << "Socket: " << strerror(errno) << "\n";
        return 1;
    }

    sockaddr_in serverAddress {};                                   // Set up server address structure
    serverAddress.sin_family = AF_INET;                             // IPv4
    serverAddress.sin_port = htons(port);                           // Convert to network byte order
    inet_pton(AF_INET, serverIp, &serverAddress.sin_addr);          // Convert IP string to binary

    // Connect to server with connect()
    if (connect(clientSocket, (sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        cerr << "Connect: " << strerror(errno) << "\n";
        close(clientSocket);
        return 1;
    }

    cout << "Connected to the server.\n";

    // 1) Send configuration which includes width, height, and minecount
    int width, height, mineCount;
    string color;
    cout << "Enter Color For Your Board (Red, Blue, Yellow, Green): ";
    cin >> color;
    cout << "Enter Rows, Cols, and Mines: ";
    cin >> height >> width >> mineCount;

    ostringstream cfg;
    cfg << width << " " << height << " " << mineCount << " " << color;
    string config = cfg.str();
    send(clientSocket, config.c_str(), config.size(), 0);

    // 2) Immediately read the INITIAL board
    char buffer[1024];
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer)-1, 0);
    if (bytesRead <= 0) {
        cerr << "Server closed the connection\n";
        return 1;
    }
    buffer[bytesRead] = '\0';
    cout << buffer;

    /*
    3) This is the main game loop which asks the user to either dig a mine or to place a flag
    If dig is entered, then the loop asks the user for what row and column they want to dig. 
    If flag is entered, the it asks the user for row and column to flag. A command string is created
    with the instructions and sent to the server to parse and update the game state which is then
    sent back to the client
    */ 
    while (true) {
        char option;
        int x, y;
        cout << "Dig or Place Flag (D or F): ";             
        cin >> option;

        ostringstream cmd;
        if (option == 'D' || option == 'd') {
            cout << "Enter a row and column to dig: ";
            cin >> x >> y;
            cmd << "Dig " << x << " " << y;
        } else if (option == 'F' || option == 'f') {
            cout << "Enter a row and column to flag: ";
            cin >> x >> y;
            cmd << "Flag " << x << " " << y;
        } else {
            cout << "Invalid Option\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        // send move
        string command = cmd.str();
        send(clientSocket, command.c_str(), command.size(), 0);

        // receive board (and possibly final message)
        bytesRead = recv(clientSocket, buffer, sizeof(buffer)-1, 0);
        if (bytesRead <= 0) {
            cerr << "Server closed connection\n";
            break;
        }
        buffer[bytesRead] = '\0';
        cout << buffer;

        // check for win/lose
        if (strstr(buffer, "YOU WIN!!!") || strstr(buffer, "YOU LOSE!!!")) {
            break;
        }
    }

    close(clientSocket);
    return 0;
}
