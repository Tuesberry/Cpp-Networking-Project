#include <string>
#include <iostream>
#include <stdlib.h>
#include <WinSock2.h>

#pragma warning(disable : 4996)
#pragma comment(lib, "ws2_32.lib") 
#define BUF_SIZE 1024
#define SERVER_PORT_NUM 65456

int main()
{   
    SOCKET clientSocket;
    SOCKADDR_IN servaddr;
    WSADATA wsaData;

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    std::cout << "> echo-client is activated" << std::endl;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    servaddr.sin_family = AF_INET;
    const char* ip_address = "127.0.0.1";
    servaddr.sin_addr.s_addr = inet_addr(ip_address);
    servaddr.sin_port = htons(SERVER_PORT_NUM);

    connect(clientSocket, (SOCKADDR*)&servaddr, sizeof(servaddr));  

    char message[BUF_SIZE];
    char* ptr = message;
    int length, received, left;
    while (1)
    {
        std::cout << "> ";
        std::cin.getline(message, BUF_SIZE);
        length = strlen(message);

        send(clientSocket, message, strlen(message), 0);
        
        ptr = message;
        left = length;
        while (left > 0)
        {
            received = recv(clientSocket, ptr, length, 0);
            if (received == 0)
            {
                break;
            }
            left -= received;
            ptr += received;
        }
        
        std::string m(message);
        if (m == "quit")
        {
            break;
        }

        std::cout << "> received: " << message << std::endl;
    }
    closesocket(clientSocket);
    WSACleanup();
    std::cout << "> echo-client is de-activated" << std::endl;
    return 0;
}
