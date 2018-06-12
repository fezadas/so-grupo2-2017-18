// CompletionPort.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "cul.h"
#include "traversedirr.h"
#include "CopyFilesIOCP.h"
#include "winerror.h"

#define STATUS_OK 0
#define MAX_THREADS 1

static HANDLE iocpThreads[MAX_THREADS];
static HANDLE completionPort;
static HANDLE shutdownEvent;

typedef struct {
	LPCSTR pathDstFiles;	
	INT errorCode;			// 0(OPER_SUCCESS) means the operation concludes successfully
	PCUL cul;
} MUTATIONS_RESULT_CTX, *PMUTATIONS_RESULT_CTX;

HANDLE CreateNewCompletionPort(DWORD dwNumberOfConcurrentThreads) {
	return CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, dwNumberOfConcurrentThreads);
}

BOOL AssociateDeviceWithCompletionPort(HANDLE hComplPort, HANDLE hDevice, DWORD CompletionKey) {
	return CreateIoCompletionPort(hDevice, hComplPort, CompletionKey, 0) == hComplPort;
}

HANDLE OpenAsync(bool isScrFile, PCSTR fName, DWORD permissions) {
	int flag;
	if (isScrFile)
		flag = OPEN_EXISTING;
	else
		flag = CREATE_NEW;
	HANDLE hFile = CreateFileA(fName, permissions,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		flag,
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
	opCtx->toRead = !opCtx->toRead;
	if (!ReadFile(hFile, opCtx->buffer, size, NULL, &opCtx->ovr)) {
		//printf("Leitura\n");
		return GetLastError() == ERROR_IO_PENDING;
	}
	//printf("Leitura FORA\n");
	return TRUE;
}

//escrever de forma assincrona size bytes
BOOL WriteAsync(HANDLE hFile, DWORD size, POPER_CTX opCtx) {
	opCtx->toRead = !opCtx->toRead;
	if (!WriteFile(hFile, opCtx->buffer, size, NULL, &opCtx->ovr)) {
		//printf("Escrita \n");
		return GetLastError() == ERROR_IO_PENDING;
		
	}
	//printf("Escrita FORA \n");
	return TRUE;
}

VOID Callback(LPVOID ctx, DWORD status, UINT64 transferedByte) {
	printf("Foram tranferidos %lld bytes", transferedByte);
}

BOOL CopyFileWrapper(LPCSTR pathFileName, LPCSTR fileName, LPVOID arg) {
	PMUTATIONS_RESULT_CTX ctx = (PMUTATIONS_RESULT_CTX)arg;
	//CUL_Increment(ctx->cul);
	char buffer[1024];
	sprintf_s(buffer, "%s/%s", ctx->pathDstFiles, fileName);
	bool res = CopyFileAsync(pathFileName, buffer, Callback, ctx);
	//CUL_Signal(ctx->cul);
	return res;
}

/*
preparar para o processo de copia de ficheiros
e fazer primeira leitura para iniciar trabalho
que completion port irá acabar.
*/
BOOL CopyFileAsync(PCSTR srcFile, PCSTR dstFile, AsyncCallback cb, LPVOID userCtx) {

	HANDLE fIn = OpenAsync(true, srcFile, GENERIC_READ);
	if (fIn == NULL) 
		return FALSE;
	HANDLE fOut = OpenAsync(false, dstFile, GENERIC_WRITE);
	if (fOut == NULL) 
		return FALSE;
	POPER_CTX opCtx = CreateOpContext(fIn, fOut, cb, userCtx);
	if(ReadAsync(fIn, BUFFER_SIZE, opCtx))
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
	}
	else {
		if (!WriteAsync(opCtx->fOut, transferedBytes, opCtx))
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
}


//BOOL isToShutdown() {
//	return WAIT_OBJECT_0 == WaitForSingleObject(shutdownEvent, 0);
//}

DWORD WINAPI IOCP_ThreadFunc(LPVOID arg) {
	DWORD transferedBytes;
	ULONG_PTR completionKey;
	POPER_CTX opCtx;
	//BOOL a = isToShutdown();
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

static long usingInitResource, usingTerminateResource;

BOOL AsyncInit() {
	
	if (0 == InterlockedExchange(&usingInitResource, 1)) {
		completionPort = CreateNewCompletionPort(0);
		/*shutdownEvent = CreateEvent(NULL, FALSE, FALSE, NULL);*/

		if (completionPort == NULL) return FALSE;
		for (int i = 0; i < MAX_THREADS; ++i) {
			iocpThreads[i] = CreateThread(NULL, 0, IOCP_ThreadFunc, NULL, 0, NULL);
		}
		return TRUE;
	}
}

VOID AsyncTerminate() {
	if (0 == InterlockedExchange(&usingTerminateResource, 1)) {
		////Start to shutdown
		//SetEvent(shutdownEvent);

		////Activate threads
		//for (int i = 0; i < MAX_THREADS; i++)
		//	PostQueuedCompletionStatus(completionPort, 0,(DWORD)NULL, NULL); 
		////Wait for all threads
		//WaitForMultipleObjects(MAX_THREADS, iocpThreads, TRUE, INFINITE);
	}
}

INT CopyFolder(LPCSTR pathRefFiles, LPCSTR pathOutFiles) {

	MUTATIONS_RESULT_CTX ctx;
	ctx.pathDstFiles = pathOutFiles;
	//ctx.errorCode = OPER_SUCCESS;
	ctx.cul = (PCUL)malloc(sizeof(CUL));
	CUL_Init((&ctx)->cul, 1); // inicializar o sincronizador com o valor 1
	// Iterate through pathRefFiles directory and sub directories
	// invoking de processor (BMP_GetFlipsOfRefFile) for each ref file
	if (!TraverseDirTree(pathRefFiles, ".txt", CopyFileWrapper, &ctx)) {
		//if (!OperHasError(&ctx))
		//	OperMarkError(&ctx, OPER_TRAVERSE_ERROR);
	}

	CUL_Signal((&ctx)->cul); // sinalizar fim da thread que deu o trabalho as workers
	CUL_Wait((&ctx)->cul);
	CUL_Destroy((&ctx)->cul);
	free(ctx.cul);

	return ctx.errorCode;
}