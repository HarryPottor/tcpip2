#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <string>

using namespace std;

#pragma comment(lib, "ws2_32.lib")
#define BUF_SIZE 1024
void ErrorHanding(string msg);
int main(int argc, char*argv[])
{
	if (argc != 3)
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

	SOCKET servsock;
	SOCKADDR_IN servaddr;
	servsock = socket(PF_INET, SOCK_STREAM, 0);
	if (servsock == INVALID_SOCKET)
	{
		ErrorHanding("socket error");
	}
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(atoi(argv[2]));

	if (connect(servsock, (SOCKADDR*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR)
	{
		ErrorHanding("connect error");
	}

	while (true)
	{
		cout << "input message(q to quit):" << endl;
		string msg;
		getline(cin, msg);
		if (msg == "quit")
		{
			break;
		}
		send(servsock, msg.c_str(), msg.size(), 0);
		int readlen = 0;
		char message[BUF_SIZE];
		while (true)
		{
			readlen += recv(servsock, &message[readlen], BUF_SIZE-1, 0);
			if (readlen >= msg.size())
			{
				break;
			}
		}
		message[readlen] = 0;
		cout << "message from server:" << message << endl;
	}

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
