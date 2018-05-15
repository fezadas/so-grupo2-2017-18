// Exercicio1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\Include\UThread.h"
#include "..\UThread\UThreadInternal.h"


VOID Func(UT_ARGUMENT Argument) {
	UtYield();
}

VOID FuncA(UT_ARGUMENT Argument) {
	printf("threadA expected to be running: ACTUAL STATE = %s", ((PUTHREAD)threadA)->State);
	printf("threadB expected to be ready: ACTUAL STATE = %s", ((PUTHREAD)threadB)->State);
	printf("yield processing");
	UtYield();
}

VOID FuncB(UT_ARGUMENT Argument) {
	printf("threadB expected to be running: ACTUAL STATE = %s", ((PUTHREAD)threadB)->State);
	printf("threadA expected to be ready: ACTUAL STATE = %s", ((PUTHREAD)threadA)->State);
}

HANDLE threadA;
HANDLE threadB;

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

