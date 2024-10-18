#include <iostream>
#include <winsock2.h> // Biblioteca para sockets no Windows
#include <ws2tcpip.h> // Funções adicionais para manipulação de endereços IP
#pragma comment(lib, "ws2_32.lib") // Linka o arquivo de biblioteca ws2_32.lib

#define PORT 7777

bool isValidNumber(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) return false;
    }
    return true;
}

int main() {
    WSADATA wsaData;
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Inicializar o Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cerr << "Falha na inicialização do Winsock" << std::endl;
        return 1;
    }

    // Criar o socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cerr << "Erro ao criar o socket" << std::endl;
        WSACleanup();
        return 1;
    }

    // Configurar o endereço do servidor
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Vincular o socket à porta 7777
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        std::cerr << "Erro no bind" << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Esperar por conexões
    if (listen(server_fd, 3) == SOCKET_ERROR) {
        std::cerr << "Erro ao escutar" << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    std::cout << "Servidor aguardando conexões na porta " << PORT << "..." << std::endl;

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) == INVALID_SOCKET) {
            std::cerr << "Erro ao aceitar conexão" << std::endl;
            closesocket(server_fd);
            WSACleanup();
            return 1;
        }

        // Receber número do cliente
        int valread = recv(new_socket, buffer, 1024, 0);
        buffer[valread] = '\0';
        
        std::string response;
        if (!isValidNumber(buffer)) {
            response = "Erro: valor inválido";
        } else {
            int number = atoi(buffer);
            std::string parity = (number % 2 == 0) ? "par" : "ímpar";
            response = "O número é " + parity;

            // Imprimir localmente se é par ou ímpar junto com o IP de origem
            std::cout << "Recebido do IP " << inet_ntoa(address.sin_addr) << ": " << number << " é " << parity << std::endl;
        }

        // Enviar resposta ao cliente
        send(new_socket, response.c_str(), response.length(), 0);
        closesocket(new_socket);
    }

    // Finalizar Winsock
    closesocket(server_fd);
    WSACleanup();

    return 0;
}
