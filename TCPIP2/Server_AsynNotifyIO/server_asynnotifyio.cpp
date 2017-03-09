#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
using namespace std;
#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 100

void CompressSockets(SOCKET hSockArr[], int index, int total);
void CompressEvents(WSAEVENT hEventArr[], int index, int total);

void ErrorHandling(char *msg);

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	SOCKET servsock, clntsock;
	SOCKADDR_IN servaddr, clntaddr;
	int clntaddrsz;

	//����SOCKET�����WSAEVENT����
	SOCKET hSockArr[WSA_MAXIMUM_WAIT_EVENTS];
	WSAEVENT hEventArr[WSA_MAXIMUM_WAIT_EVENTS];

	WSAEVENT newEvent;
	WSANETWORKEVENTS netEvents;

	int numOfClient = 0;
	int len;
	int posIndex, startIndex;
	char msg[BUF_SIZE];

	if (argc != 2)
	{
		cout << "usage: " << argv[0] << "<port>" << endl;
		system("pause");
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHandling("wsastartup error");
	}

	servsock = socket(PF_INET, SOCK_STREAM, 0);
	if (servsock == INVALID_SOCKET)
	{
		ErrorHandling("socket error");
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	if (bind(servsock, (SOCKADDR*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR)
	{
		ErrorHandling("bind error");
	}

	if (listen(servsock, 5) == SOCKET_ERROR)
	{
		ErrorHandling("listen error");
	}

	//������������WSAEVENT����servsock����Ϊ������
	newEvent = WSACreateEvent();
	if (WSAEventSelect(servsock, newEvent, FD_ACCEPT) == SOCKET_ERROR)
	{
		ErrorHandling("wsaEventSelect error");
	}
	//��servsock���ý�������
	hSockArr[numOfClient] = servsock;
	//��event���ý�������
	hEventArr[numOfClient] = newEvent;

	numOfClient++;

	while (true)
	{
		cout << "in the while" << endl;
		//���м������WSAEVENT����������ס������ͨ������ֵ�ҵ���С���Ǹ�io�ı�״̬��WSAEVENT
		posIndex = WSAWaitForMultipleEvents(numOfClient, hEventArr, false, WSA_INFINITE, false);
		startIndex = posIndex - WSA_WAIT_EVENT_0;
		for (int i = startIndex; i < numOfClient; i++)
		{
			//ȷ���Ƿ���� singaled��WSAEVENT, û�н������timeout
			int sigEventIndex = WSAWaitForMultipleEvents(1, &hEventArr[i], true, 0, false);
			if (sigEventIndex == WSA_WAIT_FAILED || sigEventIndex == WSA_WAIT_TIMEOUT)
			{
				continue;
			}
			else
			{
				sigEventIndex = i;
				//ͨ������������WSANETWORKEVENTS�ṹ�� ��ȡWSAEVENT����IO�ı�ľ�����Ϣ��������Ҫsocket��wsaevent
				WSAEnumNetworkEvents(hSockArr[sigEventIndex], hEventArr[sigEventIndex], &netEvents);
				if (netEvents.lNetworkEvents & FD_ACCEPT)
				{
					if (netEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
					{
						puts("accept error");
						break;
					}

					clntaddrsz = sizeof(clntaddr);
					clntsock = accept(hSockArr[sigEventIndex], (SOCKADDR*)&clntaddr, &clntaddrsz);
					newEvent = WSACreateEvent();
					WSAEventSelect(clntsock, newEvent, FD_READ);
					hEventArr[numOfClient] = newEvent;
					hSockArr[numOfClient] = clntsock;
					numOfClient++;
					puts("connected new client ...");

				}
				if (netEvents.lNetworkEvents & FD_READ)
				{
					if (netEvents.iErrorCode[FD_READ_BIT] != 0)
					{
						puts("read error");
						break;
					}
					len = recv(hSockArr[sigEventIndex], msg, BUF_SIZE, 0);
					send(hSockArr[sigEventIndex], msg, len, 0);
				}
				if (netEvents.lNetworkEvents & FD_CLOSE)
				{
					if (netEvents.iErrorCode[FD_CLOSE_BIT] != 0)
					{
						puts("close error");
						break;
					}
					WSACloseEvent(hEventArr[sigEventIndex]);
					closesocket(hSockArr[sigEventIndex]);

					numOfClient--;
					CompressSockets(hSockArr, sigEventIndex, numOfClient);
					CompressEvents(hEventArr, sigEventIndex, numOfClient);
				}

			}

		}
	}


	system("pause");
	return 0;
}

void CompressSockets(SOCKET hSockArr[], int index, int total)
{
	for (int i = index; i < total; i++)
	{
		hSockArr[i] = hSockArr[i++];
	}

}
void CompressEvents(WSAEVENT hEventArr[], int index, int total)
{
	for (int i = index; i < total; i++)
	{
		hEventArr[i] = hEventArr[i++];
	}
}

void ErrorHandling(char *msg)
{
	cout << msg << endl;
	system("pause");
	exit(1);
}