// ClientEx1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\Ex1DLL\CopyFileAsync.h"
#include "..\Ex1DLL\CUL.h"

VOID Callback(LPVOID ctx, DWORD status, UINT64 transferedByte) {
	printf("Foram tranferidos %lld bytes \n", transferedByte);
}

int main(int argc, char *argv[])
{
	AsyncInit();
	CopyFileAsyncWrapper(argv[1], argv[2], Callback);
	AsyncTerminate();
	printf("Finished copy\n");
	getchar();
}

