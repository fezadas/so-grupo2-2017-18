#include "stdafx.h"
#define DEBUG

#define MAX_THREADS 10

VOID TestEx2_Thread1(UT_ARGUMENT Argument) {
	int i = 1000000;
	while (i-- >= 0)UtYield();
}

VOID TestEx2_Thread2(UT_ARGUMENT Argument) {
	int i = 1000000;
	while (i-- >= 0)UtYield();
}

DWORD GetContextSwitchTime() {
	UtCreate(TestEx2_Thread1, NULL, 8 * 4096, NULL);
	UtCreate(TestEx2_Thread2, NULL, 8 * 4096, NULL);
	LARGE_INTEGER begin, end, freq;
	DWORD time1 = GetTickCount();
	UtRun();
	DWORD time2 = GetTickCount();
	DWORD time = time2 - time1;
	return time;
}

int main() {
	UtInit();

	DWORD timeInMilli = GetContextSwitchTime();
	FLOAT timeInMicro = timeInMilli * 1000 / 2000000.0;
	printf("Counting time of ContextSwitch with UThreads \n");
	printf("%f micro segundos. \n", timeInMicro);

	UtEnd();
	getchar();
	return 0;
}
