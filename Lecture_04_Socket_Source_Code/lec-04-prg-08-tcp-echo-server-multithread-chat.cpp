#include <string>
#include <iostream>
#include <stdlib.h>
#include <WinSock2.h>
#include <cstddef>
#include <thread>
#include <future>

#pragma comment(lib, "ws2_32.lib") 

#define BUF_SIZE 1024
#define PORT_NUM 65456	
#define SERVER_IP "127.0.0.1"
#define MAX_CLIENT_NUM 5

void quitThread();
DWORD WINAPI makeThread(void* socket);
SOCKET All_ClientSocket[MAX_CLIENT_NUM];

int ThreadCount = 0;

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
	bind(serverSocket, (SOCKADDR*)&servaddr, sizeof(servaddr));

	//listen을 통해 연결 준비
	listen(serverSocket, MAX_CLIENT_NUM);

	//클라이언트와 통신 준비
	SOCKET clientSocket;
	SOCKADDR_IN cliaddr;
	int clientAddressSize;

	char message[BUF_SIZE] = { 0, };
	char data[BUF_SIZE] = { 0, };

	//Thread Handle
	HANDLE hThread;

	std::cout << "> server loop running in thread (main thread):" << ++ThreadCount << std::endl;

	auto qThread = std::async(std::launch::async, quitThread);
	while (true)
	{
		//클라이언트의 연결 요청 수락
		clientAddressSize = sizeof(cliaddr);
		clientSocket = accept(serverSocket, (SOCKADDR*)&cliaddr, &clientAddressSize);

		if (clientSocket != -1)
		{
			std::cout << "> client connected by IP address " << cliaddr.sin_addr.S_un.S_addr << " with Port number " << cliaddr.sin_port << std::endl;
		}

		//Thread 생성
		hThread = CreateThread(NULL, 0, makeThread, (void*)clientSocket, 0, NULL);

		//Thread 종료
		CloseHandle(hThread);

	}

	closesocket(serverSocket);
	WSACleanup();

	std::cout << "> echo-server is de-activated" << std::endl;

	return 0;
}

DWORD WINAPI makeThread(void* socket)
{
	SOCKET clientSocket = (SOCKET)socket;
	SOCKET tempSocket;

	//클라이언트로부터 데이터 Receive
	int strLen;
	char message[BUF_SIZE] = { 0, };
	char data[BUF_SIZE] = { 0, };
	DWORD threadId = GetCurrentThreadId();
	int threadNum = ++ThreadCount;
	
	All_ClientSocket[threadNum-2] = clientSocket;

	while ((strLen = recv(clientSocket, message, BUF_SIZE, 0)) != 0 )
	{
		if (strLen == -1)
		{
			std::cout << "> disconect client" << std::endl;
			break;
		}
		memset(data, 0, BUF_SIZE);
		memcpy(data, message, strLen);
		std::cout << "> echoed: " << data << " by Thread-" << threadNum << std::endl;
		
		for (int i = 0; i < ThreadCount - 1; i++)
		{
			tempSocket = All_ClientSocket[i];
			send(tempSocket, message, strLen, 0);
		}
		
		std::string m(data);
		if (m == "quit")
		{
			break;
		}

	}
	closesocket(clientSocket);
	return 0;
}

void quitThread()
{
	while (true)
	{
		std::string msg;
		std::cout << "> " ;
		std::cin >> msg;
		std::cin.clear();
		if (msg == "quit")
		{
			if (ThreadCount == 1)
			{
				std::cout << "> stop precedure started" << std::endl;
				exit(0);
			}
			else
			{
				std::cout << "> active threads are remained : " << ThreadCount - 1 << " threads" << std::endl;
				ThreadCount--;
			}
		}
	}
}