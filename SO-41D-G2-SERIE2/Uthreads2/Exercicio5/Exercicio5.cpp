// Exercicio5.cpp : define o ponto de entrada para o aplicativo do console.
//

#include "stdafx.h"
#define ITERATIONS 2000000

DWORD WINAPI Func(LPVOID arg) {
	for (int i = 0; i < ITERATIONS; i++)
		SwitchToThread();
	return 1;
}

DWORD GetSwitchThreadTimeInMicroSameProcess() {
	
	DWORD ret1, ret2;
	HANDLE tHandle1 = CreateThread(NULL, 0, Func, NULL, CREATE_SUSPENDED, &ret1);
	HANDLE tHandle2 = CreateThread(NULL, 0, Func, NULL, CREATE_SUSPENDED, &ret2);

	if (tHandle1 == NULL || tHandle2 == NULL) {
		CloseHandle(tHandle1);
		CloseHandle(tHandle2);
		return -1;
	}

	SetThreadPriority(tHandle1, THREAD_PRIORITY_HIGHEST);
	SetThreadPriority(tHandle2, THREAD_PRIORITY_HIGHEST);

	SetThreadAffinityMask(tHandle1, 1);
	SetThreadAffinityMask(tHandle2, 1);

	DWORD time1 = GetTickCount();

	ResumeThread(tHandle1);
	ResumeThread(tHandle2);

	WaitForSingleObject(tHandle1, INFINITE);
	WaitForSingleObject(tHandle2, INFINITE);

	DWORD time2 = GetTickCount();

	DWORD retValue1, retValue2;
	GetExitCodeThread(tHandle1, &retValue1);
	GetExitCodeThread(tHandle2, &retValue2);

	CloseHandle(tHandle1);
	CloseHandle(tHandle2);

	return (time2 - time1);
}

DWORD GetSwitchThreadTimeInMicroDiffProcess() {

	//create an array to save the 2 threads from 2 different processes: 
	//0-this process || 1-Exercicio5OtherProcess.exe 's process
	HANDLE threads[2];

	//create thread for this process with Func function
	threads[0] = CreateThread(NULL, 0, Func, NULL, CREATE_SUSPENDED, NULL);
	SetThreadPriority(threads[0], THREAD_PRIORITY_TIME_CRITICAL);
	SetThreadAffinityMask(threads[0], 0);

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;

	DWORD time1 = GetTickCount();

	TCHAR childName[MAX_PATH] = _T("Exercicio5OtherProc.exe");
	CreateProcess(NULL, childName, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	threads[1] = pi.hThread;

	ResumeThread(threads[0]);
	WaitForMultipleObjects(2, threads, TRUE, INFINITE);

	DWORD time2 = GetTickCount();
	return time2 - time1;
}

int main()
{
	printf("Counting time of Switching Threads same Process \n");
	printf("Time in micro = %f\n", ((GetSwitchThreadTimeInMicroSameProcess()* 1000) )/ (FLOAT)ITERATIONS);

	printf("\nPress any key to start test with 2 processes.\n");
	getchar();

	printf("Counting time of Switching Threads different Processes \n");
	printf("Time in micro: %f\n", ((GetSwitchThreadTimeInMicroDiffProcess() * 1000)) / (FLOAT)ITERATIONS);

	printf("\nPress any key to finish test.");
	getchar();
	
	return 0;
}

