#include "stdafx.h"
#include "..\Ex1DLL\CopyFileAsync.h"

VOID Callback(LPVOID ctx, DWORD status, UINT64 transferedByte) {
	printf("Foram tranferidos %lld bytes \n", transferedByte);
}

int main(int argc, char *argv[])
{
	AsyncInit();
	int errors = CopyFolder(argv[1], argv[2], Callback);
	AsyncTerminate();
	getchar();
}