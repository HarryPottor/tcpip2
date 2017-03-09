#include <Windows.h>
#include <process.h>
#include <iostream>
using namespace std;

HANDLE InputEvent;
HANDLE AddEvent;

int sum = 0;
int num = 0;

unsigned WINAPI InputMain(void*);
unsigned WINAPI AddMain(void *);
int main()
{
	InputEvent = CreateEvent(NULL, true, true, NULL);
	AddEvent = CreateEvent(NULL, true, false, NULL);

	HANDLE thread[2];
	thread[0] = (HANDLE)_beginthreadex(NULL, 0, InputMain, NULL, 0, NULL);
	thread[1] = (HANDLE)_beginthreadex(NULL, 0, AddMain, NULL, 0, NULL);

	WaitForSingleObject(thread[0], INFINITE);
	WaitForSingleObject(thread[1], INFINITE);

	CloseHandle(InputEvent);
	CloseHandle(AddEvent);
	system("pause");
	return 0;
}


unsigned WINAPI InputMain(void*arg)
{
	for (int i = 0; i < 5; i++)
	{
		WaitForSingleObject(InputEvent, INFINITE);
		ResetEvent(InputEvent);
		cout << "input :";
		cin >> num;
		SetEvent(AddEvent);
		
	}
	return 0;
}
unsigned WINAPI AddMain(void *arg)
{
	for (int i = 0; i < 5; i++)
	{
		WaitForSingleObject(AddEvent, INFINITE);
		ResetEvent(AddEvent);
		sum += num;
		cout << "sum = " << sum << endl;
		SetEvent(InputEvent);
		
	}
	return 0;
}