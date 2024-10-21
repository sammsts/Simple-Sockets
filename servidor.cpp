#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>    // Para múltiplas threads
#include <string>
#pragma comment(lib, "ws2_32.lib")

#define PORT 7777

void handleClient(SOCKET clientSocket) {
    char buffer[1024] = {0};
    int valread = recv(clientSocket, buffer, 1024, 0);

    if (valread > 0) {
        buffer[valread] = '\0';
        std::string clientMessage = buffer;

        // Exibir mensagem recebida e IP do cliente
        std::cout << "Numero recebido: " << clientMessage << std::endl;

        // Verificar se o valor é válido e determinar se é par ou ímpar
        int numero;
        try {
            numero = std::stoi(clientMessage);
            std::string resposta = (numero % 2 == 0) ? "Numero eh par" : "Numero eh impar";
            send(clientSocket, resposta.c_str(), resposta.length(), 0);
        } catch (...) {
            std::string resposta = "Erro: valor invalido";
            send(clientSocket, resposta.c_str(), resposta.length(), 0);
        }
    }

    // Encerrar conexão com o cliente
    closesocket(clientSocket);
}

int main() {
    WSADATA wsaData;
    SOCKET serverSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int addrlen = sizeof(clientAddr);

    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Falha na inicializacao do Winsock" << std::endl;
        return 1;
    }

    // Criar socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cerr << "Erro ao criar socket" << std::endl;
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Fazer bind na porta
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Erro ao fazer bind" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Servidor aguardando conexoes na porta 7777..." << std::endl;

    // Escutar por conexões
    if (listen(serverSocket, 3) < 0) {
        std::cerr << "Erro ao escutar" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Aceitar conexões em loop e criar uma nova thread para cada cliente
    while (true) {
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrlen);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Erro ao aceitar conexao" << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        // Criar uma nova thread para tratar o cliente
        std::thread clientThread(handleClient, clientSocket);
        clientThread.detach(); // Permitir que a thread funcione de forma independente
    }

    // Encerrar o socket do servidor (nunca será alcançado neste loop infinito)
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
