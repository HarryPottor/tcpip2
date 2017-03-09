#include <iostream>
#include <Windows.h>
#include <process.h>

using namespace std;

unsigned WINAPI InputMain(void *arg);
unsigned WINAPI OutputMain(void *arg);
unsigned WINAPI AddMain(void *arg);
unsigned WINAPI SubMain(void *arg);

CRITICAL_SECTION  critical;

char ch;
int sum = 100;
int main()
{
	HANDLE Thread[4];
	InitializeCriticalSection(&critical);
	Thread[0] = (HANDLE)_beginthreadex(NULL, 0, InputMain, NULL, 0, NULL);
	Thread[1] = (HANDLE)_beginthreadex(NULL, 0, OutputMain, NULL, 0, NULL);
	Thread[2] = (HANDLE)_beginthreadex(NULL, 0, AddMain, NULL, 0, NULL);
	Thread[3] = (HANDLE)_beginthreadex(NULL, 0, SubMain, NULL, 0, NULL);



	WaitForMultipleObjects(4, Thread, TRUE, INFINITE);
	cout << "sum = " << sum << endl;
	
	for (int i = 0; i < 4; i++)
	{
		CloseHandle(Thread[i]);
	}
	
	DeleteCriticalSection(&critical);
	system("pause");
	return 0;
}

unsigned WINAPI InputMain(void *arg)
{
	return 0;
}
unsigned WINAPI OutputMain(void *arg)
{
	return 0;
}

unsigned WINAPI AddMain(void * arg)
{
	EnterCriticalSection(&critical);
	for (int i = 0; i < 100000; i++)
	{
		sum += 1;
	}
	LeaveCriticalSection(&critical);
	return 0;
}

unsigned WINAPI SubMain(void * arg)
{
	EnterCriticalSection(&critical);
	for (int i = 0; i < 100000; i++)
	{
		sum -= 1;
	}
	LeaveCriticalSection(&critical);
	return 0;
}
