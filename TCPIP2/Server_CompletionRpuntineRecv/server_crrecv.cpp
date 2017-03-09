#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <string>
#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 1024
using namespace std;

void CALLBACK CompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void ErrorHanding(string msg);

WSABUF wsabuf;
char msg[BUF_SIZE];
int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		cout << "usage: " << argv[0] << " <ip> <port>" << endl;
		system("pause");
		exit(1);
	}
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		ErrorHanding("wsastartup error");
	}
	
	SOCKET servsock, clntsock;
	SOCKADDR_IN servaddr, clntaddr;
	int clntaddrsz;

	servsock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (servsock == INVALID_SOCKET)
	{
		ErrorHanding("socket error");
	}

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

	clntaddrsz = sizeof(clntaddr);
	clntsock = accept(servsock, (SOCKADDR*)&clntaddr, &clntaddrsz);
	if (clntsock == INVALID_SOCKET)
	{
		ErrorHanding("accept error");
	}

	WSAEVENT wsaevent = WSACreateEvent();
	WSAOVERLAPPED overlapped;
	memset(&overlapped, 0, sizeof(overlapped));
	overlapped.hEvent = wsaevent;
	
	wsabuf.buf = msg;
	wsabuf.len = BUF_SIZE;
	DWORD recvlen = 0, flag = 0;
	if (WSARecv(clntsock, &wsabuf, 1, &recvlen, &flag, &overlapped, CompRoutine) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSA_IO_PENDING)
		{
			cout << "background data receive" << endl;
		}
	}

	int waitres = WSAWaitForMultipleEvents(1, &wsaevent, true, WSA_INFINITE, true);
	if (waitres == WSA_WAIT_IO_COMPLETION)
	{
		cout << "overlapped I?O complete" << endl;
	}
	else
	{
		ErrorHanding("wsarecv error");
	}

	WSACloseEvent(wsaevent);
	closesocket(clntsock);
	closesocket(servsock);
	WSACleanup();
	system("pause");
	return 0;
}

void CALLBACK CompRoutine(DWORD error, DWORD recvsz, LPWSAOVERLAPPED lpOverlapped, DWORD flag)
{
	if (error != 0)
	{
		ErrorHanding("cr error");
	}
	else
	{
		cout << "receive msg: " << recvsz << " " << msg << endl;
	}
}


void ErrorHanding(string msg)
{
	cout << msg << endl;
	system("pause");
	exit(1);
}