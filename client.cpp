#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error creating client socket\n";
        return EXIT_FAILURE;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Replace with your server's IP
    serverAddr.sin_port = htons(10102);
    std::string operation = "";
    std::string loginId = "";
    int needToLogin = 0;
    while (operation != "logout") {
        std::string params;
        if (needToLogin == 0) {
            int a;
            std::cout << "Welcome to database server first you need to login or register";
            std::cout << "Write 2 to register and 1 for login";
            std::cin >> a;
            if (a == 1) {
                std::cin >> params;
                operation = "login," + params;
            }
        }
        if (connect(clientSocket, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1) {
            std::cerr << "Error connecting to the server\n";
            close(clientSocket);
            return EXIT_FAILURE;
        }
        // Send parameters to the server
        send(clientSocket, operation.c_str(), operation.size(), 0);

        // Receive and display the server's response
        char buffer[1024];
        recv(clientSocket, buffer, sizeof(buffer), 0);
        std::cout << "Server response: " << buffer << std::endl;
    }
    close(clientSocket);

    return EXIT_SUCCESS;
}