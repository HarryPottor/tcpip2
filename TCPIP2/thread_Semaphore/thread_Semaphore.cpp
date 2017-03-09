#include <Windows.h>
#include <process.h>
#include <iostream>
using namespace std;

unsigned WINAPI InputMain(void*);
unsigned WINAPI AddMain(void *);

HANDLE hSemInput;
HANDLE hSemAdd;
int sum = 0;
int num = 0;
int main()
{
	hSemInput = CreateSemaphore(NULL, 1, 1, NULL);
	hSemAdd = CreateSemaphore(NULL, 0, 1, NULL);

	HANDLE thread[2];
	thread[0] = (HANDLE)_beginthreadex(NULL, 0, InputMain, NULL, 0, NULL);
	thread[1] = (HANDLE)_beginthreadex(NULL, 0, AddMain, NULL, 0, NULL);
	
	if (WaitForSingleObject(thread[0], INFINITE) == WAIT_OBJECT_0)
	{
		cout << "wait success hSemAdd" << endl;
	}
	if (WaitForSingleObject(thread[1], INFINITE) == WAIT_FAILED)
	{
		cout << "wait failed" << endl;
	}
	CloseHandle(hSemAdd);
	CloseHandle(hSemInput);
	CloseHandle(thread[0]);
	CloseHandle(thread[1]);

	system("pause");
	return 0;
}

unsigned WINAPI InputMain(void*arg)
{
	for (int i = 0; i < 5; i++)
	{
		WaitForSingleObject(hSemInput, INFINITE);
		cout << "input :";
		cin >> num;

		ReleaseSemaphore(hSemAdd, 1, NULL);
	}
	return 0;
}
unsigned WINAPI AddMain(void *arg)
{
	for (int i = 0; i < 5; i++)
	{
		WaitForSingleObject(hSemAdd, INFINITE);
		sum += num;
		cout << "sum = " << sum << endl;
		ReleaseSemaphore(hSemInput, 1, NULL);
	}
	return 0;
}