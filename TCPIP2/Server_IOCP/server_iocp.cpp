#include <iostream>
#include <cstdlib>
#include <process.h>
#include <WinSock2.h>
#include <Windows.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")
using namespace std;

#define BUF_SIZE 100
#define READ 3
#define WRITE 5

typedef struct
{
	SOCKET clntsock;
	SOCKADDR_IN clntaddr;
}PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

typedef struct
{
	OVERLAPPED overlapped;
	WSABUF wsabuf;
	char buffer[BUF_SIZE];
	int mode;
}PER_IO_DATA, *LPPER_IO_DATA;

unsigned WINAPI ThreadMain(void* CPIO);
void ErrorHanding(string msg);

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		cout << "usage: " << argv[0] << " <port>" << endl;
		system("pause");
		exit(1);
	}
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		ErrorHanding("wsastartup error");
	}

	HANDLE hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	for (auto i = 0; i < sysInfo.dwNumberOfProcessors; i++)
	{
		_beginthreadex(NULL, 0, ThreadMain, hComPort, 0, NULL);
	}
	
	SOCKET servsock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (servsock == INVALID_SOCKET)
	{
		ErrorHanding("socket error");
	}
	SOCKADDR_IN servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	if (bind(servsock, (SOCKADDR*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR)
	{
		ErrorHanding("bind error");
	}

	if (listen(servsock, 5) == SOCKET_ERROR)
	{
		ErrorHanding("listen error");
	}

	LPPER_HANDLE_DATA lpHandle;
	LPPER_IO_DATA lpIOData;
	DWORD recvLen = 0;
	DWORD flag = 0;
	while (true)
	{
		
		SOCKET clntsock;
		SOCKADDR_IN clntaddr;
		int addrlen = sizeof(clntaddr);

		clntsock = accept(servsock, (SOCKADDR*)&clntaddr, &addrlen);
		lpHandle = (LPPER_HANDLE_DATA)malloc(sizeof(PER_HANDLE_DATA));
		lpHandle->clntsock = clntsock;
		memcpy(&lpHandle->clntaddr, &clntaddr, sizeof(clntaddr));

		CreateIoCompletionPort((HANDLE)clntsock, hComPort, (DWORD)lpHandle, 0);

		lpIOData = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
		memset(lpIOData, 0, sizeof(PER_IO_DATA));
		lpIOData->wsabuf.buf = lpIOData->buffer;
		lpIOData->wsabuf.len = BUF_SIZE;
		lpIOData->mode = READ;

		WSARecv(lpHandle->clntsock, &(lpIOData->wsabuf), 1,
			&recvLen, &flag, &(lpIOData->overlapped), NULL);
			
	}
	WSACleanup();
	return 0;
}

unsigned WINAPI ThreadMain(void* CPIO)
{
	
	HANDLE hComPort = (HANDLE)CPIO;
	SOCKET sock;
	DWORD transBytes;
	LPPER_HANDLE_DATA lpHandle = NULL;
	LPPER_IO_DATA lpIOData = NULL;
	DWORD flags = 0;

	while (true)
	{
		GetQueuedCompletionStatus(hComPort, &transBytes, (LPDWORD)&lpHandle,
			(LPOVERLAPPED*)&lpIOData, INFINITE);
		sock = lpHandle->clntsock;
		if (lpIOData->mode == READ)
		{
			cout << "message received" << endl;
			if (transBytes == 0)
			{
				closesocket(sock);
				free(lpHandle);
				free(lpIOData);
				continue;
			}
			memset(&(lpIOData->overlapped), 0, sizeof(OVERLAPPED));
			lpIOData->wsabuf.len = transBytes;
			lpIOData->mode = WRITE;
			WSASend(sock, &(lpIOData->wsabuf), 1, NULL, 0,
				&(lpIOData->overlapped), NULL);

			lpIOData = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
			memset(&(lpIOData->overlapped), 0, sizeof(OVERLAPPED));
			lpIOData->wsabuf.len = BUF_SIZE;
			lpIOData->wsabuf.buf = lpIOData->buffer;
			lpIOData->mode = READ;
			WSARecv(sock, &(lpIOData->wsabuf), 1, NULL, &flags,
				&(lpIOData->overlapped), NULL);
		}
		else
		{
			cout << "message send" << endl;
			free(lpIOData);
		}
	}
	
	return 0;
}

void ErrorHanding(string msg)
{
	cout << msg << endl;
	system("pause");
	exit(1);
}


