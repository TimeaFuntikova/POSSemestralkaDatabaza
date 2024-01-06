#include <iostream>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
#include <arpa/inet.h>
#include "DatabaseLogic.h"


std::string processRequest(const std::string& request, DatabaseLogic& db) {
    std::istringstream iss(request);
    std::string command;
    std::getline(iss,command,',');

    if (command == "CREATE") {
        std::string table;
        iss >> table;
        db.createTable(table);
        return "Table created successfully.\n";
    } else if (command == "ADD") {
        std::string table, rowData;
        iss >> table;
        std::getline(iss, rowData);
        Row newRow;
        // Parse rowData and fill newRow
        // ...
        // db.getTable(table)->addRow(newRow);
        return "Row added successfully.\n";
    } else if (command == "GET") {
        // Similar logic for GET
    } else if (command == "UPDATE") {
        // Similar logic for UPDATE
    } else if (command == "DELETE") {
        // Similar logic for DELETE
    } else if (command == "LOGIN") {
        std::string username,password;
        std::getline(iss,username,',');
        std::getline(iss,password, ',');

        db.loginUser(username,password);
    }
    return "Unknown command.\n";

    // ... rest

    return "Unknown command.\n";
}

void *handleClient(void *clientSocket) {
    int socket = *((int*)clientSocket);
    free(clientSocket);

    std::cout << "Client connected on socket: " << socket << std::endl;

    char buffer[1024];
    DatabaseLogic db;

    while (true) {
        int bytesReceived = recv(socket, buffer, 1024, 0);
        if (bytesReceived <= 0) {
            std::cout << "Client disconnected or error occurred on socket: " << socket << std::endl;
            break;
        }

        std::string request(buffer, bytesReceived);
        std::cout << "Received request: " << request << " on socket: " << socket << std::endl;

        std::string response = processRequest(request, db);
        send(socket, response.c_str(), response.size(), 0);

        std::cout << "Sent response: " << response << " to socket: " << socket << std::endl;
    }

    close(socket);
    std::cout << "Socket closed: " << socket << std::endl;
    return NULL;
}

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(10111);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Failed to bind to port 8080" << std::endl;
        return -2;
    }

    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Failed to listen on socket" << std::endl;
        return -3;
    }

    std::cout << "Server started. Listening on port 8080..." << std::endl;

    while (true) {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrSize = sizeof(clientAddr);
        int *clientSocket = (int*)malloc(sizeof(int));
        *clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);

        if (*clientSocket == -1) {
            std::cerr << "Failed to accept connection" << std::endl;
            continue;
        }

        char clientIp[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIp, INET_ADDRSTRLEN);
        int clientPort = ntohs(clientAddr.sin_port);
        std::cout << "Client connected - IP: " << clientIp << ", Port: " << clientPort << std::endl;

        pthread_t thread;
        if (pthread_create(&thread, NULL, handleClient, clientSocket) != 0) {
            std::cerr << "Failed to create thread" << std::endl;
            continue;
        }

        pthread_detach(thread);
    }
}