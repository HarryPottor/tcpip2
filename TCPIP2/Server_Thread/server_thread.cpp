#include <iostream>
#include <WinSock2.h>
#include <process.h>
#include <Windows.h>
#include <string>

using namespace std;

#define BUF_SIZE 100
#define MAX_CLNT 256

unsigned WINAPI HandleClnt(void * arg);
void SendMsg(char* msg, int len);
void ShowError(string msg);

int iClntCnt = 0;
SOCKET ClntSocks[MAX_CLNT];
HANDLE hMutex;

int main(int argc, char * argv[])
{
	WSADATA wsaData;
	SOCKET servsock, clntsock;
	SOCKADDR_IN servaddr, clntaddr;
	int clntaddrsz;
	HANDLE hThread;

	if (argc !=2)
	{
		cout << "usage: " << argv[0] << "<port>" << endl;
		system("pause");
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		ShowError("wsastartup error");
	}

	hMutex = CreateMutex(NULL, false, NULL);

	servsock = socket(PF_INET, SOCK_STREAM, 0);
	if (servsock == INVALID_SOCKET)
	{
		ShowError("socket error");
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	if (bind(servsock, (SOCKADDR*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR)
	{
		ShowError("bind error");
	}

	if (listen(servsock, 5) == SOCKET_ERROR)
	{
		ShowError("listen error");
	}

	while (true)
	{
		clntaddrsz = sizeof(clntaddr);
		clntsock = accept(servsock, (SOCKADDR*)&clntaddr, &clntaddrsz);
		WaitForSingleObject(hMutex, INFINITE);
		ClntSocks[iClntCnt++] = clntsock;
		ReleaseMutex(hMutex);

		hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClnt, (void*)&clntsock, 0, NULL);
		cout << "connected client IP: " << inet_ntoa(clntaddr.sin_addr) << endl;;
	}
	
	closesocket(servsock);

	WSACleanup();
	system("pause");
	return 0;
}

unsigned WINAPI HandleClnt(void * arg)
{
	SOCKET clntsock = *(SOCKET*)arg;
	int len;
	char msg[BUF_SIZE];

	while ((len = recv(clntsock, msg, BUF_SIZE, 0)) > 0 )
	{
		cout << "send msg   len:" << len << endl;
		SendMsg(msg, len);
	}

	WaitForSingleObject(hMutex, INFINITE);
	for (int i = 0; i < iClntCnt; i++)
	{
		if (ClntSocks[i] == clntsock)
		{
			while (i < iClntCnt)
			{
				ClntSocks[i] = ClntSocks[i + 1];
				i++;
			}
			break;
		}
	}
	iClntCnt--;
	ReleaseMutex(hMutex);
	closesocket(clntsock);

	return 0;
}

void SendMsg(char* msg, int len)
{
	WaitForSingleObject(hMutex, INFINITE);
	for (int i = 0; i < iClntCnt; i++)
	{
		send(ClntSocks[i], msg, len, 0);
	}

	ReleaseMutex(hMutex);
}

void ShowError(string msg)
{

}