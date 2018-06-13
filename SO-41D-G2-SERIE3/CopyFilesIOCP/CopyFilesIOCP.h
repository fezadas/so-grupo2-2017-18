#pragma once

extern HANDLE completionPort;

#define BUFFER_SIZE 10

typedef struct OperCtx *POPER_CTX;
typedef VOID(*AsyncCallback)(LPVOID ctx, DWORD status, UINT64 transferedByte);

typedef struct OperCtx {
	OVERLAPPED ovr;
	HANDLE fIn, fOut;
	BYTE buffer[BUFFER_SIZE];
	UINT64 currPos;
	BOOL toRead;
	AsyncCallback cb;
	LPVOID userCtx;
	BOOL state;
	HANDLE mutex;
} OPER_CTX, *POPER_CTX;

BOOL AsyncInit();
VOID AsyncTerminate();
BOOL CopyFileAsync(PCSTR srcFile, PCSTR dstFile, AsyncCallback cb, LPVOID userCtx);
INT CopyFolder(LPCSTR pathRefFiles, LPCSTR pathOutFiles, AsyncCallback cb);

POPER_CTX CreateOpContext(HANDLE fIn, HANDLE fOut, AsyncCallback cb, LPVOID userCtx);
VOID DestroyOpContext(POPER_CTX ctx);