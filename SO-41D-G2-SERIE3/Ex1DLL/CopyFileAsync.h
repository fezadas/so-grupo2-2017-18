#ifdef DLL_EXPORTS
#define CopyFileAsyncc __declspec(dllexport)
#else
#define CopyFileAsyncc __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <psapi.h>
#include <winnt.h>
#include "CUL.h"

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

	CopyFileAsyncc BOOL AsyncInit();

	CopyFileAsyncc VOID AsyncTerminate();

	CopyFileAsyncc BOOL CopyFileAsync(PCSTR srcFile, PCSTR dstFile, AsyncCallback cb, LPVOID userCtx);

	CopyFileAsyncc BOOL CopyFileAsyncWrapper(PCSTR srcFile, PCSTR dstFile, AsyncCallback cb);

	CopyFileAsyncc INT CopyFolder(LPCSTR pathRefFiles, LPCSTR pathOutFiles, AsyncCallback cb);

	typedef struct {
		LPCSTR pathDstFiles;
		INT errorCode;			// 0(OPER_SUCCESS) means the operation concludes successfully
		CUL cul;
		AsyncCallback cb;
	} MUTATIONS_RESULT_CTX, *PMUTATIONS_RESULT_CTX;
}