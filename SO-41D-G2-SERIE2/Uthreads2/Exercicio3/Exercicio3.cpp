// Exercicio3.cpp : define o ponto de entrada para o aplicativo do console.
//

#include "stdafx.h"
#include "..\Include\UThread.h"

//Bug na inserção na lista Joiners

VOID FuncTestJoin(UT_ARGUMENT arg) {

}

VOID Func1(UT_ARGUMENT arg) {
	HANDLE array[2];
	array[0] = UtCreate(FuncTestJoin, NULL, 8 * 4096, "WorkerThread1");
	array[1] = UtCreate(FuncTestJoin, NULL, 8 * 4096, "WorkerThread2");
	HANDLE a = UtSelf();
	UtMultiJoin(array, 2);
	printf("%d", UtGetCount(UtSelf()));
}

VOID UtMultiJoinTest()
{
	UtInit();
	BOOL isAlive = FALSE;
	HANDLE hMainThread = UtCreate(Func1, &isAlive, 8 * 4096, "MainThread");
	printf("Counter : %d", UtGetCount(hMainThread));
	UtRun();
	printf("Counter : %d", UtGetCount(hMainThread));
	UtEnd();
	getchar();
}


int main()
{
	UtMultiJoinTest();
}

