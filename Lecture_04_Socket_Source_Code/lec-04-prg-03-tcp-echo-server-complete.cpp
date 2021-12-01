#include <string>
#include <iostream>
#include <stdlib.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib") //ws2_32.dll을 참조하겠다
#define BUF_SIZE 1024
#define PORT_NUM 65456	//포트 넘버

int main()
{
	WSADATA wsaData;
	//윈도우 소켓 구현에 대한 정보를 가진다.
	//원도우 소켓 초기화 정보 구조체
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cout << "> WSAStartup() failed";
		return -1;
	}
	/*
	* 윈도우 소켓을 새로 만들때 윈도우 소켓을 초기화해야 한다.
	* WSAStartup은 윈도우 소켓을 초기화하고 초기화한 상태를 wsadata구조체에 저장한다.
	* WSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData);
	* wVersionRequested : 사용할 윈도우 소켓의 버전. 상위 2바이트에는 주 버전 번호, 하위 2바이트에는 부 버전 번호 입력
	*			MAKEWORD함수를 사용한다. MAKEWORD(2,2)는 0x0202와 같다.
	* lpWSAData : 윈도우 소켓 설정을 전달한다.
	*/

	std::cout << "> echo-server is activated" << std::endl;

	//소켓 생성
	SOCKET serverSocket;
	serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET)
	{
		//소켓 생성 에러
		std::cout << "> socket() failed" << std::endl;
		return -1;
	}

	//소켓 주소 설정
	SOCKADDR_IN servaddr = { 0 };
	servaddr.sin_family = AF_INET;
	u_long IP_ADDR = INADDR_ANY;
	servaddr.sin_addr.s_addr = htonl(IP_ADDR);
	servaddr.sin_port = htons(PORT_NUM);

	//IP주소와 포트번호 Bind
	//소켓에 IP주소와 포트 할당
	if (bind(serverSocket, (SOCKADDR*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR)
	{
		std::cout << "> bind() failed and program terminated" << std::endl;
		return -1;
	}

	//listen을 통해 연결 준비
	if (listen(serverSocket, 1) == SOCKET_ERROR)
	{
		std::cout << "> listen() failed and program terminated" << std::endl;
		return -1;
	}

	//클라이언트와 통신
	SOCKET clientSocket;
	SOCKADDR_IN cliaddr;
	int clientAddressSize;
	int strLen;
	char message[BUF_SIZE] = { 0, };
	char data[BUF_SIZE] = { 0, };
	while (1)
	{
		//클라이언트의 연결 요청 수락
		clientAddressSize = sizeof(cliaddr);
		clientSocket = accept(serverSocket, (SOCKADDR*)&cliaddr, &clientAddressSize);

		if (clientSocket != -1)
		{
			std::cout << "> client connected by IP address " << cliaddr.sin_addr.S_un.S_addr << " with Port number " << cliaddr.sin_port << std::endl;
		}

		//클라이언트로부터 recv
		while ((strLen = recv(clientSocket, message, BUF_SIZE, 0)) != 0)
		{
			if (strLen == -1)
			{
				std::cout << "> client disconnected" << std::endl;
				break;
			}
			memset(data, 0, BUF_SIZE);
			memcpy(data, message, strLen);
			std::cout << "> echoed: " << data << std::endl;
			send(clientSocket, message, strLen, 0);
		}
		
		std::string m(data);
		if (m == "quit")
		{
			break;
		}

		closesocket(clientSocket);
	}

	closesocket(serverSocket);
	WSACleanup();

	std::cout << "> echo-server is de-activated" << std::endl;

	return 0;
}