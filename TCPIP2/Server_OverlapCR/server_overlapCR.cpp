#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <string>

using namespace std;

#pragma comment(lib, "ws2_32.lib")
#define BUF_SIZE 1024

void CALLBACK ReadCR(DWORD, DWORD, LPOVERLAPPED, DWORD);
void CALLBACK WriteCR(DWORD, DWORD, LPOVERLAPPED, DWORD);
void ErrorHanding(string msg);

typedef struct
{
	SOCKET clntsock;
	char buf[BUF_SIZE];
	WSABUF wsabuf;
} PER_IO_DATA, *LPPER_IO_DATA;

int main(int argc, char*argv[])
{
	if (argc != 2)
	{
		cout << "usage: " << argv[0] << " <ip> <port>" << endl;
		system("pause");
		exit(1);
	}
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHanding("wsastartup error");
	}

	SOCKET servsock, clntsock;
	SOCKADDR_IN servaddr, clntaddr;
	int clntaddrsz;

	unsigned long mode = 1;
	servsock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	ioctlsocket(servsock, FIONBIO, &mode);
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
	while (true)
	{
		SleepEx(100, TRUE);
		clntsock = accept(servsock, (SOCKADDR*)&clntaddr, &clntaddrsz);
		if (clntsock == INVALID_SOCKET)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				continue;
			}
			else
			{
				ErrorHanding("accept error");
			}
		}
		cout << "client connect ... " << endl;

		LPWSAOVERLAPPED lpoverlapped = (LPWSAOVERLAPPED)malloc(sizeof(WSAOVERLAPPED));
		memset(lpoverlapped, 0, sizeof(WSAOVERLAPPED));

		LPPER_IO_DATA lpclntdata = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
		lpclntdata->clntsock = clntsock;
		lpclntdata->wsabuf.buf = lpclntdata->buf;
		lpclntdata->wsabuf.len = BUF_SIZE;

		lpoverlapped->hEvent = (HANDLE)lpclntdata;
		DWORD recvBytes = 0, flag = 0;
		WSARecv(clntsock, &(lpclntdata->wsabuf), 1, &recvBytes, &flag, lpoverlapped, ReadCR);
	}

	closesocket(clntsock);
	closesocket(servsock);
	WSACleanup();
	system("pause");
	return 0;
}
void CALLBACK ReadCR(DWORD error, DWORD recvsz, LPOVERLAPPED lpoverlapped, DWORD flag)
{
	cout << "readcr" << endl;
	LPPER_IO_DATA lpdata = (LPPER_IO_DATA)(lpoverlapped->hEvent);
	SOCKET clntsock = lpdata->clntsock;
	LPWSABUF lpwsabuf = &(lpdata->wsabuf);

	if (recvsz == 0)
	{
		closesocket(clntsock);
		free(lpoverlapped->hEvent);
		free(lpoverlapped);
		cout << "client disconnected .. " << endl;
	}
	else
	{
		lpwsabuf->len = recvsz;
		DWORD sendbytes = 0;
		WSASend(clntsock, lpwsabuf, 1, &sendbytes, 0, lpoverlapped, WriteCR);
	}
}
void CALLBACK WriteCR(DWORD error, DWORD recvsz, LPOVERLAPPED lpoverlapped, DWORD flag)
{
	cout << "writecr" << endl;
	LPPER_IO_DATA lpdata = (LPPER_IO_DATA)(lpoverlapped->hEvent);
	SOCKET clntsock = lpdata->clntsock;
	LPWSABUF lpwsabuf = &(lpdata->wsabuf);

	DWORD recvbytes = 0;
	DWORD flaginfo = 0;
	WSARecv(clntsock, lpwsabuf, 1, &recvbytes, &flaginfo, lpoverlapped, ReadCR);

}

void ErrorHanding(string msg)
{
	cout << msg << endl;
	system("pause");
	exit(1);
}