// Exercicio5OtherProc.cpp : define o ponto de entrada para o aplicativo do console.
//

#include "stdafx.h"

int main()
{
	HANDLE thread = GetCurrentThread();
	SetThreadPriority(thread, THREAD_PRIORITY_HIGHEST);
	SetThreadAffinityMask(thread, 0);
	for (int i = 0; i < 10000000; ++i) {
		SwitchToThread();
	}
	return 0;
}

