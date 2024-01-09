#include <iostream>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
#include <arpa/inet.h>
#include <string>
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

    } else if(command == "LOGIN") {

        std::string password;
        iss >> password;

        return db.loginUser(username,password);

    } else if (command == "DELETE_TABLE") {
        std::string tableName;
        iss >> tableName;

        if (db.deleteTable(username, tableName)) {
            return "Table " + tableName + " deleted successfully.";
        } else {
            return "Failed to delete table " + tableName + ".";
        }
    } else if (command == "GRANT_ACCESS") {

        std::string tableName;
        iss >> tableName;

        std::string userWithNewAccess;
        iss >> userWithNewAccess;

        //"I" || "U" || "D" || "S"
        std::string right;
        iss >>  right;
        if(db.grantAccess(userWithNewAccess, tableName, right))
        {
            return "Access to table " + tableName + " for the user: " + userWithNewAccess + " granted.";
        }
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

        if(db.update(primaryKey, data, tableName, username)) return "Update was successful";

        return "Update failed.";

    } else if (command == "DELETE") {
        std::string primaryKey;
        iss >> primaryKey;

        std::string tableName;
        iss >> tableName;

        if(db.deleteRow(primaryKey, username, tableName)) return "Delete successful.";
    } else if (command == "LIST_MY_TABLES") {
            auto createdTables = db.listTablesCreatedByUser(username);
            std::stringstream response;
            for (const auto& tableName : createdTables) {
                response << tableName << "\n";
            }
            return response.str().empty() ? "No tables created." : response.str();
        }
    else if(command == "GET_ROWS") {
        std::string tableName;
        iss >> tableName;

        return db.getRows(username, tableName);
    }
    else if(command == "GET_ROW") {
        std::string primaryKey;
        iss >> primaryKey;

        std::string tableName;
        iss >> tableName;

        return db.getRow(primaryKey, username, tableName);
    } else if(command == "ADD_COLUMN") {

        std::string columnName;
        iss >> columnName;

        std::string tableName;
        iss >> tableName;

        if(db.addColumn(columnName, username, tableName)) return "Column added.";
    }
    return "Unknown command.\n";
}

void *handleClient(void *clientSocket) {
    int socket = *((int *) clientSocket);
    free(clientSocket);

    char buffer[4096];
    std::string username;

    // Receive username from client
    int bytesReceived = recv(socket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived <= 0) {
        std::string message= "Error while receiving username from the client";
        send(socket, message.c_str(), message.size(), 0);
        close(socket);
        return nullptr;
    }
    username.assign(buffer);
    std::memset(buffer, 0, sizeof(buffer));

    Database db;

    std::string fileName = "/tmp/semestralka/Users_table.csv";
    std::string userFound = Database::findUserinCsvFile(fileName, username);

    std::string responseMsg;
    if (username == userFound) {
        std::cout << "User " << username << " is registered.\n" << std::endl;
        responseMsg = "Enter password: ";
    } else {
        std::cout << "User " << username << " is not registered.\n" << std::endl;
        responseMsg = "Create password: ";
    }
    send(socket, responseMsg.c_str(), responseMsg.size(), 0);
    std::cout << "Sent message to client.\n" << std::endl;


    recv(socket, buffer, sizeof(buffer) - 1, 0);
    std::cout << "Received: " << buffer << std::endl;

    std::string newPassword;
    newPassword.assign(buffer);
    std::cout << newPassword;


        if(username != userFound) {
            std::cout << "Trying to register the user:" << std::endl;
            if (!db.registerUser(username, newPassword)) {
                send(socket, "User registered successfully", 29, 0);
            }
        } else {
            std::string mess = "Logged In.";
            send(socket, mess.c_str(), mess.size(), 0);
        }
    bool isRunning = true;
    while (isRunning) {
        std::memset(buffer, 0, sizeof(buffer));
        bytesReceived = recv(socket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            isRunning = false;
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

    int port = 8084;
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

    std::cout << "Server started. Listening on port " << port << "..." << std::endl;

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