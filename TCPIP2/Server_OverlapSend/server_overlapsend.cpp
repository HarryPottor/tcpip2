#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <string>
#pragma comment(lib, "ws2_32.lib")
using namespace std;


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
	//重叠IO第一步，创建异步socket
	SOCKET servsock;
	servsock =  WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (servsock == INVALID_SOCKET)
	{
		ErrorHanding("socket error");
	}
	SOCKADDR_IN servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(atoi(argv[2]));
	if (connect(servsock, (SOCKADDR*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR)
	{
		ErrorHanding("connect error");
	}

	//重叠IO第二部，组织数据，WSAOVERLAPPED和WSABUF
	WSAEVENT event = WSACreateEvent();
	WSAOVERLAPPED overlapped;
	memset(&overlapped, 0, sizeof(overlapped));
	overlapped.hEvent = event;

	WSABUF wsabuf;
	char msg[] = "hello overlapped";
	wsabuf.buf = msg;
	wsabuf.len = strlen(msg) + 1;

	DWORD sendlen;
	//重叠IO第三部，WSASend发送数据
	if (WSASend(servsock, &wsabuf, 1, &sendlen, 0, &overlapped, NULL) == SOCKET_ERROR)
	{
		//重叠IO第四部，获取失败标志，如果是 WSA_IO_PENDING 表示数据还没有传送完毕
		if (WSAGetLastError() == WSA_IO_PENDING)
		{
			cout << "back data send" << endl;
			//等待传入的事件完成
			WSAWaitForMultipleEvents(1, &event, true, WSA_INFINITE, false);
			//重叠IO第五步，获取传输的结果
			WSAGetOverlappedResult(servsock, &overlapped, &sendlen, false, NULL);
		}
		else
		{
			ErrorHanding("wsasend() error");
		}
	}

	cout << "send data size " << sendlen << endl;
	WSACloseEvent(event);
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