
#include "stdafx.h"
#include "..\Include\UThread.h"

HANDLE hMainThread;

VOID FuncTestJoin(UT_ARGUMENT arg) {
	printf("%s is now Running. \n MainThread Counter of Joiners has the value of: %d\n", (CHAR*)arg, UtGetCount(hMainThread));
	printf("Counter will be decremented. \n");
}

VOID Func1(UT_ARGUMENT arg) {
	HANDLE array[2];
	array[0] = UtCreate(FuncTestJoin, (UT_ARGUMENT)"WorkerThread1", 8 * 4096, "WorkerThread1");
	array[1] = UtCreate(FuncTestJoin, (UT_ARGUMENT)"WorkerThread2", 8 * 4096, "WorkerThread2");
	printf("MainThread is now Running, but will be Deactived by MultiJoin.\nCounter of Joiners has the value of: %d.\n", UtGetCount(hMainThread));
	printf("\n");
	UtMultiJoin(array, 2);
	printf("\n");
	printf("Counter reachs value of zero.\n");
	printf("MainThread wakes up, and finishes the work.\n");
}

VOID UtMultiJoinTest()
{
	UtInit();
	hMainThread = UtCreate(Func1, NULL, 8 * 4096, "MainThread");
	int x = UtGetCount(hMainThread);
	UtRun();
	UtEnd();
	getchar();
}

int main()
{
	UtMultiJoinTest();
}

