#include <string>
#include <iostream>
#include <stdlib.h>
#include <WinSock2.h>

#pragma warning(disable : 4996)
#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 1024
#define PORT_NUM 65456	

int main()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cout << "> WSAStartup() failed";
		return -1;
	}

	std::cout << "> echo-server is activated" << std::endl;

	//소켓 생성
	SOCKET serverSocket;
	serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (serverSocket == INVALID_SOCKET)
	{
		//소켓 생성 에러
		std::cout << "> socket() failed" << std::endl;
		return -1;
	}

	//소켓 주소 설정
	SOCKADDR_IN servaddr = { 0 };
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT_NUM);

	//IP주소와 포트번호 Bind
	//소켓에 IP주소와 포트 할당
	bind(serverSocket, (SOCKADDR*)&servaddr, sizeof(servaddr));

	//클라이언트와 통신
	SOCKADDR_IN cliaddr;
	SOCKADDR_IN CliaddrList[10];
	int clientCount = 0;
	char message[BUF_SIZE];
	int recvSize, sendSize;
	while (1)
	{
		int ClientSize = sizeof(cliaddr);
		//Recvfrom
		recvSize = recvfrom(serverSocket, message, BUF_SIZE, 0, (SOCKADDR*)&cliaddr, &ClientSize);
		if (recvSize < 0)
		{
			std::cout << "> recvfrom() failed." << std::endl;
			break;
		}
		std::string m(message);
		if (m[0] == '#' || m == "quit")
		{
			if (m == "#REG")
			{
				std::cout << "> client registered  ('" << cliaddr.sin_addr.S_un.S_addr << "', " << cliaddr.sin_port << ")" << std::endl;
				CliaddrList[clientCount] = cliaddr;
				clientCount++;
			}
			else if (m == "#DEREG" || m == "quit")
			{
				for (int i = 0; i < clientCount; i++)
				{
					if (CliaddrList[i].sin_port == cliaddr.sin_port)
					{
						CliaddrList[i] = CliaddrList[clientCount - 1];
						clientCount--;
						std::cout << "> Client de-registered  ('" << cliaddr.sin_addr.S_un.S_addr << "', " << cliaddr.sin_port << ")" << std::endl;
						break;
					}
				}
			}
		}
		else 
		{
			if (clientCount == 0)
			{
				std::cout << "> no clients to echo" << std::endl;
			}
			else 
			{
				bool found = false;
				for (int i = 0; i < clientCount; i++)
				{
					if (CliaddrList[i].sin_port == cliaddr.sin_port)
					{
						found = true;
						break;
					}
				}
				if (found == false)
				{
					std::cout << "> ignores a message from un-registered client" << std::endl;
				}
				else 
				{
					std::cout << "> received ( " << message << " ) and echoed to " << clientCount << " clients" << std::endl;
					//Sendto
					for (int i = 0; i < clientCount; i++)
					{
						ClientSize = sizeof(CliaddrList[i]);
						sendSize = sendto(serverSocket, message, recvSize, 0, (SOCKADDR*)&CliaddrList[i], ClientSize);
					}
					if (sendSize < 0 || recvSize != sendSize)
					{
						std::cout << "> sendto() failed." << std::endl;
						break;
					}
				}
			}
		}
		
		memset(message, 0, BUF_SIZE);
	}

	closesocket(serverSocket);
	WSACleanup();

	std::cout << "> echo-server is de-activated" << std::endl;

	return 0;
}