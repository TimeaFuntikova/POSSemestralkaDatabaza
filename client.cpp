#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

void performOperation(int clientSocket, const std::string& request) {
    int sendRes = send(clientSocket, request.c_str(), request.size(), 0);
    if (sendRes == -1) {
        std::cerr << "Failed to send request to server." << std::endl;
        return;
    }

    char buffer[4096];
    memset(buffer, 0, 4096);
    int bytesReceived = recv(clientSocket, buffer, 4096, 0);
    if (bytesReceived == -1) {
        std::cerr << "Error in receiving response from server." << std::endl;
    } else if (bytesReceived == 0) {
        std::cout << "Server closed the connection." << std::endl;
    } else {
        std::cout << "SERVER: " << std::string(buffer, bytesReceived) << "\r\n";
    }
}

std::string getUsername() {
    std::string username;
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);
    return username;
}

std::string getPassword() {
    std::string password;
    std::cout << "Enter your password: ";
    std::getline(std::cin, password);
    return password;
}

void printInstructions() {
    std::cout << "Available commands:" << std::endl;
    std::cout << "  CREATE_TABLE <tableName> <columnName1:DATA_TYPE:nullable, columnName2:DATA_TYPE:nullable...> - Create a new table" << std::endl;
    std::cout << "  LIST_MY_TABLES - Lists all tables created by the user logged in." << std::endl;
    std::cout << "  DELETE_TABLE <tableName> - Delete a table" << std::endl;
    std::cout << "  GRANT_ACCESS <tableName> <username> <permission : IUDS> - Grant access to a username to a specific table." << std::endl;
    std::cout << "  INSERT <tableName> <data1,data2,...> - Insert data into specific table." << std::endl;
    std::cout << "  q - Quit" << std::endl;
}

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Can't create socket" << std::endl;
        return -1;
    }

    struct sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8082);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Can't connect to server" << std::endl;
        close(clientSocket);
        return -2;
    }

    std::string username = getUsername();
    send(clientSocket, username.c_str(), username.size(), 0);

    char serverResponse[4096];
    recv(clientSocket, serverResponse, sizeof(serverResponse), 0);
    std::string response(serverResponse);
    std::cout << response << std::endl;

    // Check server's response for password entry or new password creation
    if (response.find("Enter password") != std::string::npos ||
        response.find("Create new password") != std::string::npos) {

        std::string password = getPassword();
        send(clientSocket, password.c_str(), password.size(), 0);

        // Receive response after sending password
        recv(clientSocket, serverResponse, sizeof(serverResponse), 0);
        std::cout << serverResponse << std::endl;
    }

    printInstructions();
    std::string userInput;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, userInput);

        if (userInput == "q") {
            break;
        }

        performOperation(clientSocket, userInput);
    }

    // Close the socket
    close(clientSocket);
    return 0;
}

