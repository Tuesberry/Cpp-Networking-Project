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
	char message[BUF_SIZE];
	int recvSize, sendSize;
	while (true)
	{
		int ClientSize = sizeof(cliaddr);
		//Recvfrom
		recvSize = recvfrom(serverSocket, message, BUF_SIZE, 0, (SOCKADDR*)&cliaddr, &ClientSize);
		if ( recvSize < 0 )
		{
			std::cout << "> recvfrom() failed." << std::endl;
			break;
		}
		std::cout << "> echoed: " << message << std::endl;
		//Sendto
		sendSize = sendto(serverSocket, message, recvSize, 0, (SOCKADDR*)&cliaddr, ClientSize);
		if (sendSize < 0 || recvSize != sendSize)
		{
			std::cout << "> sendto() failed." << std::endl;
			break;
		}
		memset(message, 0, BUF_SIZE);
	}

	closesocket(serverSocket);
	WSACleanup();

	std::cout << "> echo-server is de-activated" << std::endl;

	return 0;
}