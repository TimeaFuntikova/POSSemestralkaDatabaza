#include <iostream>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
#include <arpa/inet.h>
#include <cstring>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;
#include "database.h"

DataType parseDataType(const std::string& typeStr) {
    if (typeStr == "INT") return DataType::INT;
    if (typeStr == "DOUBLE") return DataType::DOUBLE;
    if (typeStr == "STRING") return DataType::STRING;
    if (typeStr == "BOOL") return DataType::BOOL;
    if (typeStr == "DATE") return DataType::DATE;
    return DataType::STRING;
}

std::string processRequest(const std::string& request, Database& db, const std::string& username) {

    std::istringstream iss(request);
    std::string command;
    iss >> command;

    std::cout << "Received command: " << command << std::endl;

    if (command == "CREATE_TABLE") {
        std::string tableName;
        iss >> tableName;
        Table newTable(tableName, username);

        std::string columnDef;
        std::getline(iss, columnDef);
        std::istringstream colDefs(columnDef);

        std::string colDef;
        while (std::getline(colDefs, colDef, ',')) {
            std::istringstream colIss(colDef);
            std::string columnName, columnType, columnNullable;
            std::getline(colIss, columnName, ':');
            std::getline(colIss, columnType, ':');
            std::getline(colIss, columnNullable);

            DataType dataType = parseDataType(columnType);
            bool isNullable = (columnNullable == "true");

            newTable.addColumn(columnName, dataType, isNullable);
        }

        const auto& columns = newTable.getColumns();
        db.createTable(tableName, columns, username);

        return "Table " + tableName + " created successfully.";

    } else if (command == "DELETE_TABLE") {
        std::string tableName;
        iss >> tableName;

        if (db.deleteTable(username, tableName)) {
            return "Table " + tableName + " deleted successfully.";
        } else {
            return "Failed to delete table " + tableName + ".";
        }
    } else if (command == "GRANT_ACCESS") {

        std::string userWithNewAccess;
        iss >> userWithNewAccess;

        std::string tableName;
        iss >> tableName;


        //"I" || "U" || "D" || "S"
        std::string right;
        iss >>  right;
        if(db.grantAccess(userWithNewAccess, tableName, right)) return "Access to table " + tableName + " for the user: " + userWithNewAccess + " granted.";
    }else if(command == "INSERT_INFO") {
        std::string tableName;
        iss >> tableName;

        return Database::getHeader("/tmp/semestralka/" + tableName + ".csv");
    } else if(command == "INSERT") {
        std::string tableName;
        iss >> tableName;

        std::string data;
        iss >> data;

        if(db.insert(data, tableName, username)) return "Row added successfully.\n";
    } else if (command == "GET") {
    } else if (command == "UPDATE") {

        std::string primaryKey;
        iss >> primaryKey;

        std::string tableName;
        iss >> tableName;

        std::string data;
        iss >> data;

        if(db.update(primaryKey, data, tableName, username)) return "Update was successfull";

        return "Update failed.";
    } else if (command == "DELETE") {
    } else if (command == "LIST_MY_TABLES") {
            auto createdTables = db.listTablesCreatedByUser(username);
            std::stringstream response;
            for (const auto& tableName : createdTables) {
                response << tableName << "\n";
            }
            return response.str().empty() ? "No tables created." : response.str();
        }
    return "Unknown command.\n";
}

void *handleClient(void *clientSocket) {
    int socket = *((int*)clientSocket);
    free(clientSocket);

    char buffer[4096];
    std::string username;

    // Receive username from client
    recv(socket, buffer, sizeof(buffer), 0);
    username.assign(buffer);
    std::memset(buffer, 0, sizeof(buffer));

    Database db;

    if (Database::isUserRegistered(username)) {
        std::cout << "User " << username << " is registered." << std::endl;
        send(socket, "Enter password", 14, 0);

        // Receive password from client
        recv(socket, buffer, sizeof(buffer), 0);
        std::string password(buffer);

        if (Database::validateUserPassword(username, password)) {
            std::string successMsg = "Login successful";
            send(socket, successMsg.c_str(), successMsg.length(), 0);
        } else {
            std::string notSuccessMsg = "Incorrect password";
            send(socket, notSuccessMsg.c_str(), notSuccessMsg.length(), 0);

            close(socket);
            return nullptr;
        }
    } else {
        std::cout << "User " << username << " is not registered." << std::endl;
        send(socket, "Create new password", 19, 0);

        // Receive new password from client
        recv(socket, buffer, sizeof(buffer), 0);
        std::string newPassword(buffer);

        Database::registerUser(username, newPassword);
        send(socket, "User registered successfully", 29, 0);
    }

    bool isRunning = true;
    while (isRunning) {
        std::memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(socket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            isRunning = false;  // Client disconnected or error occurred
            break;
        }

        std::string request(buffer, bytesReceived);
        std::string response = processRequest(request, db, username);
        send(socket, response.c_str(), response.size(), 0);

        if (request == "q") {
            isRunning = false;
        }
    }

    close(socket);
    std::cout << "User " << username << " has disconnected." << std::endl;
    return nullptr;
}


int main() {
    std::cout << fs::current_path();
    Database db;



    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    int port = 8082;
    struct sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Failed to bind to port " << port << std::endl;
        return -2;
    }

    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Failed to listen on socket" << std::endl;
        return -3;
    }

    std::cout << "Server started. Listening on port 8080..." << std::endl;

    while (true) {
        struct sockaddr_in clientAddr{};
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