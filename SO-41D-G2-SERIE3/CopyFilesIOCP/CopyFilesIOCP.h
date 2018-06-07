#pragma once

extern HANDLE completionPort;

#define BUFFER_SIZE 4096

typedef struct OperCtx *POPER_CTX;
typedef VOID(*AsyncCallback)(LPVOID userCtx, DWORD status, UINT64 transferedBytes);

typedef struct OperCtx {
	OVERLAPPED ovr;
	HANDLE fIn, fOut;
	BYTE buffer[BUFFER_SIZE];
	UINT64 currPos;
	BOOL toRead;
	AsyncCallback cb;
	LPVOID userCtx;
} OPER_CTX, *POPER_CTX;

BOOL AsyncInit();
HANDLE OpenAsync(PCSTR fName, DWORD permissions);
BOOL ReadAsync(HANDLE hFile, DWORD toRead, POPER_CTX opCtx);
BOOL WriteAsync(HANDLE hFile, DWORD toRead, POPER_CTX opCtx);
BOOL CopyFileAsync(PCSTR srcFile, PCSTR dstFile, AsyncCallback cb, LPVOID userCtx);


POPER_CTX CreateOpContext(HANDLE fIn, HANDLE fOut, AsyncCallback cb, LPVOID userCtx);
VOID DestroyOpContext(POPER_CTX ctx);