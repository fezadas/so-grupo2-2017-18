// Exercicio1.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "..\Include\UThread.h"

HANDLE threadA;
HANDLE threadB;

VOID Func(UT_ARGUMENT Argument) {
	UtYield();
}

VOID FuncA(UT_ARGUMENT Argument) {
	printf("threadA expected to be running: ACTUAL STATE = %s \n", UtStateArray[UtGetState(threadA)]);
	printf("threadB expected to be ready: ACTUAL STATE = %s \n", UtStateArray[UtGetState(threadB)]);
	printf("yield processing \n");
	UtYield();
}

VOID FuncB(UT_ARGUMENT Argument) {
	printf("threadB expected to be running: ACTUAL STATE = %s \n", UtStateArray[UtGetState(threadB)]);
	printf("threadA expected to be ready: ACTUAL STATE = %s \n", UtStateArray[UtGetState(threadA)]);
}

//VOID FuncTestJoin(UT_ARGUMENT arg) {
//
//}
//
//VOID Func1(UT_ARGUMENT arg) {
//	HANDLE array[2];
//	array[0] = UtCreate(FuncTestJoin, NULL, 8 * 4096, "WorkerThread1");
//	array[1] = UtCreate(FuncTestJoin, NULL, 8 * 4096, "WorkerThread2");
//	HANDLE a = UtSelf();
//	UtMultiJoin(array, 2);
//	printf("%d", UtGetCount(UtSelf()));
//}

VOID test_dump() {
	UtInit();
	UtCreate(Func, NULL, 0, "a");
	UtCreate(Func, NULL, 0, "b");
	UtDump();
	UtRun();
	UtEnd();
}

VOID test_state_change() {
	UtInit();
	threadA = UtCreate(FuncA, NULL, 0, "a");
	threadB = UtCreate(FuncB, NULL, 0, "b");
	UtRun();
	UtEnd();
}


int main()
{
	test_dump();
	getchar();
	test_state_change();
	getchar();
}

