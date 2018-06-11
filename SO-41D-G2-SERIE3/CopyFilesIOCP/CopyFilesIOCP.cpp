// SearchIntFilesIOCP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CopyFilesIOCP.h"

POPER_CTX CreateOpContext(HANDLE fIn, HANDLE fOut, AsyncCallback cb, LPVOID userCtx) {
	POPER_CTX op = (POPER_CTX)calloc(1, sizeof(OPER_CTX));
	op->cb = cb;
	op->userCtx = userCtx;
	op->fIn = fIn;
	op->fOut = fOut;
	op->toRead = true;
	return op;
}

VOID DestroyOpContext(POPER_CTX ctx) {
	CloseHandle(ctx->fIn);
	free(ctx);
}
/*
VOID Callback(LPVOID ctx, DWORD status, UINT64 transferedByte) {
	printf("Foram tranferidos %lld bytes", transferedByte);
}
*/

int main(int argc, char *argv[])
{
	CopyFolder(argv[1], argv[2]);
	/*
	if (argc != 3) {
		printf("usage: CopyFileAsync <fileSrc> <fileDst> \n");
		return 1;
	}
	AsyncInit();
	if (!CopyFileAsync(argv[1], argv[2], Callback, NULL)) {
		printf("error starting async oper!\n");
		return 1;
	}
	AsyncTerminate();
	printf("press enter to terminate\n");
	getchar();

	return 0;
	*/
}

