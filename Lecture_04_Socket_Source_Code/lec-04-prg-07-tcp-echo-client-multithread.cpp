#include <string>
#include <iostream>
#include <stdlib.h>
#include <WinSock2.h>

#pragma warning(disable : 4996)
#pragma comment(lib, "ws2_32.lib") 
#define BUF_SIZE 1024
#define SERVER_PORT_NUM 65456

DWORD WINAPI RecvThread(void* socket);

bool IsQuit = false;

int main()
{
    SOCKET clientSocket;
    SOCKADDR_IN servaddr;

    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cout << "> WSAStartup() failed" << std::endl;
        return -1;
    }

    std::cout << "> echo-client is activated" << std::endl;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cout << "> socket() failed" << std::endl;
        return -1;
    }

    servaddr.sin_family = AF_INET;
    const char* ip_address = "127.0.0.1";
    servaddr.sin_addr.s_addr = inet_addr(ip_address);
    servaddr.sin_port = htons(SERVER_PORT_NUM);

    if (connect(clientSocket, (SOCKADDR*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR)
    {
        std::cout << "> connect() failed and program terminated" << std::endl;
        return -1;
    }

    HANDLE hThread;
    char message[BUF_SIZE];

    while (!IsQuit)
    {
        hThread = CreateThread(NULL, 0, RecvThread, (void*)clientSocket, 0, NULL);

        while (1)
        {
            std::cout << "> ";
            std::cin.getline(message, BUF_SIZE);
            std::cin.clear();
            if (send(clientSocket, message, strlen(message), 0) == SOCKET_ERROR)
            {
                std::cout << "> send() failed and program terminated" << std::endl;
                IsQuit = true;
            }

            std::string m(message);
            if (m == "quit")
            {
                IsQuit = true;
                break;
            }
        }
        CloseHandle(hThread);
    }
    closesocket(clientSocket);
    WSACleanup();
    std::cout << "> echo-client is de-activated" << std::endl;
    return 0;
}

DWORD WINAPI RecvThread(void* socket)
{
    SOCKET clientSocket = (SOCKET)socket;
    int strLen;
    char RecvMessage[BUF_SIZE] = { 0, };

    while ((strLen = recv(clientSocket, RecvMessage, BUF_SIZE, 0))!= 0 && !IsQuit)
    {
        if (strLen == SOCKET_ERROR)
        {
            std::cout << "> recv() failed and program terminated" << std::endl;
            return -1;
        }
        std::cout << "> received: " << RecvMessage << std::endl;
        memset(RecvMessage, 0, BUF_SIZE);
    }
    closesocket(clientSocket);
    return 0;
}