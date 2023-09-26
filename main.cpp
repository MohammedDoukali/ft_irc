#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cerrno>
#include <vector>
#include "lib.hpp"

const int MAX_CLIENTS = 10;

int  check_port(std::string str)
{
    int port;
    std::istringstream nm(str);
    if (nm >> port && nm.eof())
    {
      if (port < 1024 || port > 65535)
      {
        std::cerr << "Error: Port Number Must Be An Integer Between 1024 And 65535." << std::endl;
        exit(0);
      }
    }
    else
    {
        std::cerr << "Error : The Port Must Be Integer Value !!" << std::endl;
        exit(0);
    }
    return port;
}

void connect_server_client(glob *stru)
{
    int clientSocket,maxSocketDescriptor;
    char buffer[1024];
    std::vector<int> clientSockets(MAX_CLIENTS, 0);
    fd_set readfds;

    maxSocketDescriptor = stru->serverSocket;

    while (true) {
    FD_ZERO(&readfds);
    FD_SET(stru->serverSocket, &readfds);
    // Add child sockets to set
    for (size_t i = 0; i < clientSockets.size(); ++i) {
        if (clientSockets[i] > 0)
            FD_SET(clientSockets[i], &readfds);
        if (clientSockets[i] > maxSocketDescriptor)
            maxSocketDescriptor = clientSockets[i];
    }
    
    // Wait for activity on one of the sockets
    int activity = select(maxSocketDescriptor + 1, &readfds, NULL, NULL, NULL);
    if ((activity < 0) && (errno != EINTR)) {
        std::cerr << "Select error...\n";
        exit(EXIT_FAILURE);
    }
        // If something happened on the server socket, then it's an incoming connection
    if (FD_ISSET(stru->serverSocket, &readfds)) {
        clientSocket = accept(stru->serverSocket, NULL, NULL);
        if (clientSocket < 0) {
            std::cerr << "Acceptance failed...\n";
            exit(EXIT_FAILURE);
        }
        std::cout << "New client connected.\n";
    
        // Add the new client socket to the vector
        for (size_t i = 0; i < clientSockets.size(); ++i) {
            if (clientSockets[i] == 0) {
                clientSockets[i] = clientSocket;
                break;
            }
        }
    }
        
    // Check each client for activity
    for (size_t i = 0; i < clientSockets.size(); ++i) {
        clientSocket = clientSockets[i];
        if (FD_ISSET(clientSocket, &readfds)) {
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0';  // Null-terminate the received data
                std::cout << "Message from client " << i << ": " << buffer << "\n";
            } else if (bytesRead == 0) {
                std::cout << "Client " << i << " disconnected.\n";
                close(clientSocket);
                clientSockets[i] = 0;
            } else {
                std::cerr << "Error in recv from client " << i << ".\n";
                close(clientSocket);
                clientSockets[i] = 0;
            }
        }
    }
    }
}
void setup_socket(int port, glob *stru)
{
        // Create A socket //
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    stru->serverSocket = serverSocket;
        
    if (serverSocket == -1)
    {
        std::cerr << "Failed to create socket\n";
        exit(EXIT_FAILURE);
    }

        // Configure Server Adress //
    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr)); // initialize struct 
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

        // Bind The Socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1){
    std::cerr << "Binding failed\n";
    exit(EXIT_FAILURE);}

        // Listen for Connections
    const int BACKLOG = 10;  // Maximum number of pending connections
    if (listen(serverSocket, BACKLOG) == -1) {
        close(serverSocket);
    std::cerr << "Listen failed\n";
    exit(EXIT_FAILURE);}
    std::cout << "Server listening.. On Port " << port << std::endl;

    stru->serverAddr = serverAddr;
}
int main(int ac, char **av)
{
    glob str;
    if (ac == 3)
    {
       int port = check_port(av[1]);
       setup_socket(port,&str);
       connect_server_client(&str);
    }   
    else
        std::cerr << "Error : Number of Arguments Invalid !! " << std::endl;
}