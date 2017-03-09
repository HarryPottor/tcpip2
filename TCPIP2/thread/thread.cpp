#include <iostream>
#include <process.h>
#include <Windows.h>

using namespace std;

unsigned WINAPI ThreadMain(void * arg);


int main()
{
	int num = 5;
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadMain, (void*)&num, 0, NULL);
	DWORD res = WaitForSingleObject(hThread, INFINITE);
	if (res == WAIT_FAILED)
	{
		cout << "wait failed" << endl;
	}
	else if (res == WAIT_OBJECT_0)
	{
		cout << "signaled" << endl;
	}
	else if (res == WAIT_TIMEOUT)
	{
		cout << "time out" << endl;
	}


	cout << "end of thread" << endl;
	CloseHandle(hThread);
	system("pause");
	return 0;
}

unsigned WINAPI ThreadMain(void * arg)
{
	int num = *(int*)arg;
	for (int i = 0; i < num; i++)
	{
		cout << "thread" << endl;
		Sleep(1000);
	}
	return 0;
}