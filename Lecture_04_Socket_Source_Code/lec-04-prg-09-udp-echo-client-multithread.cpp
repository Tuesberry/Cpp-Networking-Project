#include <string>
#include <iostream>
#include <stdlib.h>
#include <WinSock2.h>

#pragma warning(disable : 4996)
#pragma comment(lib, "ws2_32.lib") 
#define BUF_SIZE 1024
#define SERVER_PORT_NUM 65456

SOCKADDR_IN cliaddr;
SOCKADDR_IN servaddr;
bool IsQuit = false;
bool CanRecv = false;
DWORD WINAPI RecvThread(void* socket);

int main()
{
    SOCKET clientSocket;
    
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cout << "> WSAStartup() failed" << std::endl;
        return -1;
    }

    std::cout << "> echo-client is activated" << std::endl;

    clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cout << "> socket() failed" << std::endl;
        return -1;
    }

    servaddr.sin_family = AF_INET;
    const char* ip_address = "127.0.0.1";
    servaddr.sin_addr.s_addr = inet_addr(ip_address);
    servaddr.sin_port = htons(SERVER_PORT_NUM);

    char SendMessage[BUF_SIZE];

    HANDLE hThread;

    while (!IsQuit)
    {
        hThread = CreateThread(NULL, 0, RecvThread, (void*)clientSocket, 0, NULL);

        while (true)
        {
            std::cout << "> ";
            std::cin.getline(SendMessage, BUF_SIZE);

            if (sendto(clientSocket, SendMessage, BUF_SIZE, 0, (SOCKADDR*)&servaddr, sizeof(servaddr)) < 0)
            {
                std::cout << "> sendto() failed." << std::endl;
                IsQuit = true;
                break;
            }
            std::string m(SendMessage);
            if (m == "quit")
            {
                IsQuit = true;
                break;
            }
            memset(SendMessage, 0, BUF_SIZE);
            CanRecv = true;
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
    char RecvMessage[BUF_SIZE];
    int RecvSize;
    int ServaddrSize = sizeof(servaddr);

    while (true)
    {
        if (!CanRecv)continue;
        ServaddrSize = sizeof(servaddr);
        RecvSize = recvfrom(clientSocket, RecvMessage, BUF_SIZE, 0, (SOCKADDR*)&servaddr, &ServaddrSize);
        if (RecvSize < 0)
        {
            if (IsQuit)break;
            std::cout << "> recvfrom() failed." << std::endl;
            IsQuit = true;
            break;
        }
        std::cout << "> received: " << RecvMessage << std::endl; 
        std::string m(RecvMessage);
        if (m == "quit")
        {
            IsQuit = true;
            break;
        }
        memset(RecvMessage, 0, BUF_SIZE);
    }
    return 0;
}