#pragma once
#include <Windows.h>
typedef struct CUL {
	HANDLE Signaled;
	HANDLE Mutex;
	DWORD Counter; //InitialCounter;
} CUL, *PCUL;

VOID CUL_Init(PCUL latch, DWORD value);

VOID CUL_Increment(PCUL latch);

VOID CUL_Wait(PCUL latch);

VOID CUL_Signal(PCUL latch);

VOID CUL_Destroy(PCUL latch);

