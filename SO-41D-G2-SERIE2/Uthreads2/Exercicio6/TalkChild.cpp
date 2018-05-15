#include "stdafx.h"

BOOL bRunThread = TRUE;
void ReadFromPipeAndWriteInConsole(HANDLE hPipeRead);


int main()
{
	FILE*    fp;
	CHAR     szInput[1024];
	TCHAR line[128];
	HANDLE hErrorWrite;
	HANDLE hThread;
	DWORD ThreadId;
	HANDLE hChildRead = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hChildWrite = GetStdHandle(STD_OUTPUT_HANDLE);

	printf("ChildConsole");

	hThread = CreateThread(NULL, 0, ReadFromConsoleAndWritePipe,
		(LPVOID)hChildWrite, 0, &ThreadId);

	ReadFromPipeAndWriteInConsole(hChildRead);

	// Tell the thread to exit and wait for thread to die.
	bRunThread = FALSE;

	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hChildRead);
	CloseHandle(hChildWrite);

	return 0;
	
}
DWORD WINAPI ReadFromConsoleAndWritePipe(LPVOID lpvThreadParam)
{
	TCHAR read_buff[256];
	DWORD nBytesRead, nBytesWrote;
	DWORD dwRead;
	HANDLE hPipeWrite = (HANDLE)lpvThreadParam;

	// Get input from our console and send it to child through the pipe.
	while (bRunThread)
	{
		_getts_s(read_buff, 256);
		WriteFile(hPipeWrite, read_buff, dwRead, &dwRead, NULL);
		{
			if (GetLastError() == ERROR_NO_DATA)
				break; // Pipe was closed (normal exit path).
		}
	}

	return 1;
}
void ReadFromPipeAndWriteInConsole(HANDLE hPipeRead)
{
	CHAR lpBuffer[256];
	DWORD nBytesRead;
	DWORD nCharsWritten;

	while (TRUE)
	{
		if (!ReadFile(hPipeRead, lpBuffer, sizeof(lpBuffer),
			&nBytesRead, NULL) || !nBytesRead)
		{

			if (GetLastError() == ERROR_BROKEN_PIPE)
				break; // pipe done - normal exit path.
		}
		if (!WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), lpBuffer,
			nBytesRead, &nCharsWritten, NULL));
	}
}
