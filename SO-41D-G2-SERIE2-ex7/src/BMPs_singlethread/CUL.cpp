#pragma once
#include "cul.h"

VOID CUL_Init(PCUL latch, DWORD value) {
	latch->Signaled = CreateEvent(NULL, TRUE, FALSE, NULL);
	latch->Mutex = CreateMutex(NULL, FALSE, NULL);
	latch->Counter = value;
}
VOID CUL_Increment(PCUL latch) {
	WaitForSingleObject(latch->Mutex, INFINITE);
	++latch->Counter;
	ReleaseMutex(latch->Mutex);
}
VOID CUL_Wait(PCUL latch) {
	WaitForSingleObject(latch->Signaled, INFINITE);
}

VOID CUL_Signal(PCUL latch) {
	WaitForSingleObject(latch->Mutex, INFINITE);
	int res = --latch->Counter;
	ReleaseMutex(latch->Mutex);
	if (res == 0)
		SetEvent(latch->Signaled);
}


VOID CUL_Destroy(PCUL latch) {
	CloseHandle(latch->Signaled);
	CloseHandle(latch->Mutex);
}

