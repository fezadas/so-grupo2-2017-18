#pragma once
#include "stdafx.h"
#include "cul.h"

VOID CUL_Init(PCUL latch, DWORD value) {
	latch->Signaled = CreateEvent(NULL, TRUE, FALSE, NULL);
	latch->LimitSignaled = CreateEvent(NULL, FALSE, FALSE, NULL); //auto reset
	latch->Mutex = CreateMutex(NULL, FALSE, NULL);
	latch->Counter = value;
	latch->WorkLimit = 2;
}

VOID CUL_Increment(PCUL latch) {
	if (latch->Counter >= latch->WorkLimit)  // verificar se nao ultrapassaste o teu limite de threads de trabalho
		WaitForSingleObject(latch->LimitSignaled, INFINITE); // esperar que alguma thread sinalize
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
	if (latch->Counter < latch->WorkLimit)
		SetEvent(latch->LimitSignaled); // sinalizar a thread que ficou a espera do trabalho
	if (res == 0)
		SetEvent(latch->Signaled);
}

VOID CUL_Destroy(PCUL latch) {
	CloseHandle(latch->Signaled);
	CloseHandle(latch->Mutex);
	CloseHandle(latch->LimitSignaled);
}