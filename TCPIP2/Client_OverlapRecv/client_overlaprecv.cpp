#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <string>
#pragma comment(lib, "ws2_32.lib")
using namespace std;
#define BUF_SIZE 100

void ErrorHanding(string msg);

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	SOCKET servsock, clntsock;
	SOCKADDR_IN servaddr, clntaddr;
	int clntaddrsz;

	if (argc != 2)
	{
		cout << "usage: " << argv[0] << "<port>" << endl;
		system("pause");
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHanding("wsastartup error");
	}

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

	WSAEVENT event = WSACreateEvent();
	WSAOVERLAPPED overlapped;
	memset(&overlapped, 0, sizeof(overlapped));
	overlapped.hEvent = event;
	WSABUF databuf;
	char buf[BUF_SIZE];
	databuf.buf = buf;
	databuf.len = BUF_SIZE;

	DWORD recvbyte = 0;
	DWORD flags = 0;

	if (WSARecv(clntsock, &databuf, 1, &recvbyte, &flags, &overlapped, NULL) 
		== SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSA_IO_PENDING)
		{
			cout << "background data receive" << endl;
			WSAWaitForMultipleEvents(1, &event, true, WSA_INFINITE, false);
			WSAGetOverlappedResult(servsock, &overlapped, &recvbyte, false, NULL);
		}
		else
		{
			cout << WSAGetLastError() << endl;
			ErrorHanding("wsarecv error");
		}
	}

	cout << "receive msg:" << buf << endl;

	WSACloseEvent(event);
	closesocket(clntsock);
	closesocket(servsock);
	WSACleanup();
	system("pause");
	return 0;
}


void ErrorHanding(string msg)
{
	cout << msg << endl;
	system("pause");
	exit(1);
}