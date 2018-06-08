// CompletionPort.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "CopyFilesIOCP.h"
#include "winerror.h"

#define STATUS_OK 0
#define MAX_THREADS 1

static HANDLE iocpThreads[MAX_THREADS];
static HANDLE completionPort;


// I/O Wrapers

HANDLE CreateNewCompletionPort(DWORD dwNumberOfConcurrentThreads) {
	return CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, dwNumberOfConcurrentThreads);
}

BOOL AssociateDeviceWithCompletionPort(HANDLE hComplPort, HANDLE hDevice, DWORD CompletionKey) {
	HANDLE h = CreateIoCompletionPort(hDevice, hComplPort, CompletionKey, 0);
	return h == hComplPort;
}

HANDLE OpenAsync(PCSTR fName, DWORD permissions) {
	HANDLE hFile = CreateFileA(fName, permissions,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE) return NULL;
	if (!AssociateDeviceWithCompletionPort(completionPort, hFile, (ULONG_PTR)hFile)) {
		CloseHandle(hFile);
		return NULL;
	}
	return hFile;
}


//ler de forma assincrona size bytes
BOOL ReadAsync(HANDLE hFile, DWORD size, POPER_CTX opCtx) {
	if (!ReadFile(hFile, opCtx->buffer, size, NULL, &opCtx->ovr)) {
		return GetLastError() == ERROR_IO_PENDING;
	}
	return TRUE;
}

//escrever de forma assincrona size bytes
BOOL WriteAsync(HANDLE hFile, DWORD size, POPER_CTX opCtx) {
	if (!WriteFile(hFile, opCtx->buffer, size, NULL, &opCtx->ovr)) {
		return GetLastError() == ERROR_IO_PENDING;
	}
	return TRUE;
}


/*preparar para o processo de copia de ficheiros 
  e fazer primeira leitura para iniciar trabalho
  que completion port irá acabar.*/
BOOL CopyFileAsync(PCSTR srcFile, PCSTR dstFile, AsyncCallback cb, LPVOID userCtx) {

	HANDLE fIn = OpenAsync(srcFile, GENERIC_READ);
	if (fIn == NULL) 
		return FALSE;
	HANDLE fOut = OpenAsync(dstFile, GENERIC_WRITE);
	if (fOut == NULL) 
		return FALSE;
	POPER_CTX opCtx = CreateOpContext(fIn, fOut, cb, userCtx);
	if(ReadAsync(fIn, 1, opCtx))
		return TRUE;

	CloseHandle(fIn);
	DestroyOpContext(opCtx);
	return FALSE;
}

//quando é para despachar a conclusão da cópia
VOID DispatchAndReleaseOper(POPER_CTX opCtx, DWORD status, DWORD transferedBytes) {
	opCtx->cb(opCtx->userCtx, status, opCtx->currPos);
	DestroyOpContext(opCtx);
}

//chamado pelas threads do completion port
VOID ProcessRequest(POPER_CTX opCtx, DWORD transferedBytes) {
	if (transferedBytes == 0) { // operation done, call callback!
		DispatchAndReleaseOper(opCtx, STATUS_OK, opCtx->currPos);
		return;
	}

	if (opCtx->toRead) { 
		if (!ReadAsync(opCtx->fIn, BUFFER_SIZE, opCtx))
			DispatchAndReleaseOper(opCtx, GetLastError(), opCtx->currPos);
		else {
			// adjust current read position
			LARGE_INTEGER li;
			opCtx->currPos += transferedBytes;
			li.QuadPart = opCtx->currPos;
			// adjust overlapped offset
			opCtx->ovr.Offset = li.LowPart;
			opCtx->ovr.OffsetHigh = li.HighPart;
		}
	}
	else
		if(!WriteAsync(opCtx->fOut, transferedBytes, opCtx))
			DispatchAndReleaseOper(opCtx, GetLastError(), opCtx->currPos);
	opCtx->toRead = !opCtx->toRead;
}

DWORD WINAPI IOCP_ThreadFunc(LPVOID arg) {
	DWORD transferedBytes;
	ULONG_PTR completionKey;
	POPER_CTX opCtx;

	while (TRUE) {
		BOOL res = GetQueuedCompletionStatus(completionPort,
			&transferedBytes, &completionKey, (LPOVERLAPPED *)&opCtx, INFINITE);

		if (!res) {
			transferedBytes = 0;
			DWORD error = GetLastError();
			if (error != ERROR_HANDLE_EOF) {
				// operation error, abort calling callback
				DispatchAndReleaseOper(opCtx, error,0);
				continue;
			}
		}
		ProcessRequest(opCtx, transferedBytes);
	}
	return 0;
}

static long usingInitResource, 
			usingTerminateResource;

BOOL AsyncInit() {
	
	if (0 == InterlockedExchange(&usingInitResource, 1)) {
		completionPort = CreateNewCompletionPort(0);
		if (completionPort == NULL) return FALSE;
		for (int i = 0; i < MAX_THREADS; ++i) {
			iocpThreads[i] = CreateThread(NULL, 0, IOCP_ThreadFunc, NULL, 0, NULL);
		}
		return TRUE;
	}
}

VOID AsyncTerminate() {
	if (0 == InterlockedExchange(&usingTerminateResource, 1)) {

		CloseHandle(fIn);
		DestroyOpContext(opCtx);

		InterlockedExchange(&usingInitResource, 0);
	}
}
