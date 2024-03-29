#include "stdafx.h"

HANDLE threadA, threadB;

//////////////////////////

VOID FuncR(UT_ARGUMENT argument) {
	printf("  Thread start\n");
	UtTerminateThread(threadA);
	printf("  Thread end\n"); //this must not be printed.
	UtExit();
}
VOID TestWhenIsRunning() {
	printf("Test UtTerminateThread 1\n");
	printf("  When thread is running must terminate immediatly.\n");
	printf("  'Thread end' message must not be printed.\n\n");
	UtInit();
	threadA = UtCreate(FuncR, (UT_ARGUMENT)"t", 8 * 4096, NULL);
	UtRun();
	UtEnd();
	printf("\nEnd test.");
}

//////////////////////////

VOID FuncThreadB(UT_ARGUMENT argument) {
	printf("  Thread B starting\n");
	printf("  -Terminate thread A-\n");
	UtTerminateThread(threadA);
	printf("  -Yield process-\n");
	UtYield();
	printf("  Thread B end\n");
	UtExit();
}
VOID FuncThreadA(UT_ARGUMENT argument) {
	printf("  Thread A start\n");
	UtYield();
	printf("  Thread A end\n"); //this must not be printed.
	UtExit();
}
VOID TestWhenSchedule() {
	printf("Test UtTerminateThread 2\n");
	printf("  In the next Schedule operation if thread is ready it must terminate.\n");
	printf("  'Thread A end' message must not be printed.\n\n");
	UtInit();
	threadA = UtCreate(FuncThreadA, (UT_ARGUMENT)"tA", 0, NULL);
	threadB = UtCreate(FuncThreadB, (UT_ARGUMENT)"tB", 0, NULL);
	UtRun();
	UtEnd();
	printf("\nEnd test.");
}

//////////////////////////

VOID FuncThread2(UT_ARGUMENT argument) {
	printf("  Thread 2 starting\n");
	printf("  -terminate thread 1-\n");
	UtTerminateThread(threadA);
	printf("  -activate thread 1-\n");
	UtActivate(threadA);
	printf("  Thread 2 end\n");
}
VOID FuncThread1(UT_ARGUMENT argument) {
	printf("  Thread 1 start\n");
	printf("  -deactivate thread 1-\n");
	UtDeactivate();
	printf("  Thread 1 end\n"); //this must not be printed.
}
VOID TestWhenBlockedToReady() {
	printf("Start test UtTerminateThread 3\n");
	printf("  When thread changes to ready after being blocked must be terminated.\n");
	printf("  'Thread 1 end' message must not be printed.\n\n");
	UtInit();
	threadA = UtCreate(FuncThread1, (UT_ARGUMENT)"t1", 8 * 4096, NULL);
	threadB = UtCreate(FuncThread2, (UT_ARGUMENT)"t2", 8 * 4096, NULL);
	UtRun();
	UtEnd();
	printf("\nEnd test.");
}

//////////////////////////

int main() {
	//Test UtTerminateThread
	TestWhenIsRunning();
	printf("\n\n------------------------\n\n");
	TestWhenSchedule();
	printf("\n\n------------------------\n\n");
	TestWhenBlockedToReady();
	getchar();
}
