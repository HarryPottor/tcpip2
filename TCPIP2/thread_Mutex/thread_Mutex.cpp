#include <iostream>
#include <Windows.h>
#include <process.h>
#include <cctype>

using namespace std;

unsigned WINAPI InputMain(void *arg);
unsigned WINAPI OutputMain(void *arg);

HANDLE mutex;
char ch;
int main()
{
	HANDLE Thread[2];
	mutex = CreateMutex(NULL, false, NULL);
	Thread[0] = (HANDLE)_beginthreadex(NULL, 0, InputMain, NULL, 0, NULL);
	Thread[1] = (HANDLE)_beginthreadex(NULL, 0, OutputMain, NULL, 0, NULL);

	WaitForMultipleObjects(2, Thread, true, INFINITE);
	CloseHandle(mutex);
	for (int i = 0; i < 2; i++)
	{
		CloseHandle(Thread[i]);
	}
	system("pause");
	return 0;
}

unsigned WINAPI InputMain(void *arg)
{
	while (true)
	{
		WaitForSingleObject(mutex, INFINITE);
		cout << "input ch = ";
		cin >> ch;
		ReleaseMutex(mutex);
	}
	
	return 0;
}
unsigned WINAPI OutputMain(void *arg)
{
	while (true)
	{
		WaitForSingleObject(mutex, INFINITE);
		if (isalpha(ch))
		{
			cout << "After change = " << static_cast<char>(toupper(ch)) << endl;;
		}
		else
		{
			cout << "Not a char" << endl;
		}
		

		ReleaseMutex(mutex);
	}


	return 0;
}