#include <iostream>
#include <WinSock2.h>
#include <process.h>
#include <string>
#include <Windows.h>
using namespace std;

#define BUF_SIZE 100

unsigned WINAPI ReadMain(void *);
unsigned WINAPI WriteMain(void *);
void ShowError(string msg);

int main(int argc, char *argv[])
{
	SOCKET servsock;
	SOCKADDR_IN servaddr;
	HANDLE thread[2];
	WSADATA wsaData;

	if (argc != 3)
	{
		cout << "usage: " << argv[0] << "<ip> <port>" << endl;
		exit(1);
	}
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		ShowError("wsastartup error");
	}

	servsock = socket(PF_INET, SOCK_STREAM, 0);
	if (servsock == INVALID_SOCKET)
	{
		ShowError("socket error");
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(atoi(argv[2]));
	
	if (connect(servsock, (SOCKADDR*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR)
	{
		ShowError("connect error");
	}

	thread[0] = (HANDLE)_beginthreadex(NULL, 0, ReadMain, (void*)&servsock, 0, NULL);
	thread[1] = (HANDLE)_beginthreadex(NULL, 0, WriteMain, (void*)&servsock, 0, NULL);

	WaitForMultipleObjects(2, thread, false, INFINITE);
	
	closesocket(servsock);
	WSACleanup();

	system("pause");
	return 0;
}

unsigned WINAPI ReadMain(void * arg)
{
	SOCKET sock = *(SOCKET*)arg;
	int len;
	char msg[BUF_SIZE];
	while (true)
	{
		len = recv(sock, msg, BUF_SIZE, 0);
		if (len > 0)
		{
			cout << len << endl;
			msg[len] = 0;
			cout << "recefrom server: " << msg << endl;
		}
		else if (len == 0)
		{
			cout << "server disconnect" << endl;
			break;
		}
	}
	return 0;
}
unsigned WINAPI WriteMain(void *arg)
{
	SOCKET sock = *(SOCKET*)arg;
	string msg;
	
	while (true)
	{
		getline(cin, msg);
		if (msg == "quit")
		{
			break;
		}
		send(sock, msg.c_str(), msg.size(), 0);
	}
	return 0;
}

void ShowError(string msg)
{
	cout << msg << endl;
	system("pause");
	exit(1);
}