#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define PORT 7777

int main() {
    WSADATA wsaData;
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Inicializar o Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cerr << "Falha na inicialização do Winsock" << std::endl;
        return 1;
    }

    // Criar o socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cerr << "Erro ao criar socket" << std::endl;
        WSACleanup();
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Usar inet_addr para converter endereço IP
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Conectar ao servidor
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Conexão falhou" << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Enviar número ao servidor
    std::string input;
    std::cout << "Digite um número: ";
    std::cin >> input;

    send(sock, input.c_str(), input.length(), 0);

    // Ler resposta do servidor
    int valread = recv(sock, buffer, 1024, 0);
    buffer[valread] = '\0';
    std::cout << "Resposta do servidor: " << buffer << std::endl;

    closesocket(sock);
    WSACleanup();
    return 0;
}
