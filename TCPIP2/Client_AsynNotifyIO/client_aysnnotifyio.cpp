#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <string>
#define BUF_SIZE 100

#pragma comment(lib, "ws2_32.lib")
using namespace std;



void ErrorHanding(string msg);
int main(int argc, char * argv[])
{
	WSADATA wsaData;
	SOCKET servsock;
	SOCKADDR_IN servaddr;
	char msg[BUF_SIZE];
	string buf;
	WSAEVENT newEvent;
	WSANETWORKEVENTS netEvents;

	if (argc != 3)
	{
		cout << "usage: " << argv[0] << " <ip> <port>" << endl;
		system("pause");
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		ErrorHanding("wsastartup error");
	}

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
	newEvent = WSACreateEvent();
	if (WSAEventSelect(servsock, newEvent, FD_READ | FD_WRITE) == SOCKET_ERROR)
	{
		ErrorHanding("wsaeventselect error");
	}
	while (true)
	{
		int posIndex = WSAWaitForMultipleEvents(1, &newEvent, false, WSA_INFINITE, false);
		int startIndex = posIndex - WSA_WAIT_EVENT_0;
		posIndex = WSAWaitForMultipleEvents(1, &newEvent, true, 0, false);
		if (posIndex == WSA_WAIT_FAILED || posIndex == WSA_WAIT_TIMEOUT)
		{
			continue;
		}
		else
		{
			WSAEnumNetworkEvents(servsock, newEvent, &netEvents);
			//只有第一次的时候会触发，FD_WRITE事件在发送缓冲区有多出的空位时会触发，
			//但不是在有足够的空位时触发，就是说你得先把发送缓冲区填满。
			if (netEvents.lNetworkEvents == FD_WRITE)
			{
				if (netEvents.iErrorCode[FD_WRITE_BIT] != 0)
				{
					puts("write error");
					continue;
				}
				getline(cin, buf);
				send(servsock, buf.c_str(), buf.size(), 0);
			}
			if (netEvents.lNetworkEvents == FD_READ)
			{
				if (netEvents.iErrorCode[FD_READ_BIT] != 0)
				{
					puts("read error");
					continue;
				}
				int len = recv(servsock, msg, BUF_SIZE, 0);
				if (len > 0 )
				{
					msg[len] = 0;
					cout << "Receive From server: " << msg << endl;
				}
				else
				{
					WSACloseEvent(newEvent);
					closesocket(servsock);
				}
				
			}
		}
		
	}
	
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