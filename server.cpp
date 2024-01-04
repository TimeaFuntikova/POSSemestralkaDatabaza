#include <iostream>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

void *handleClient(void *clientSocket) {
    int socket = *((int*)clientSocket);
    free(clientSocket);

    // Client communication

    close(socket);
    return NULL;
}

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 5);

    while (true) {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrSize = sizeof(clientAddr);
        int *clientSocket = (int*)malloc(sizeof(int));
        *clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);

        pthread_t thread;
        pthread_create(&thread, NULL, handleClient, clientSocket);
    }
}
