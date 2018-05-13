#include "stdafx.h"

void ReadFromPipeAndWriteInConsole(HANDLE hPipeRead);
void Init(HANDLE hChildStdOut,HANDLE hChildStdIn,HANDLE hChildStdErr);
DWORD WINAPI ReadFromConsoleAndWritePipe(LPVOID lpvThreadParam);

HANDLE hChildProcess = NULL;
HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE); // Handle to parents std input.
BOOL bRunThread = TRUE;


int main()
{
	HANDLE hFatherReadTemp, hFatherRead, hChildWrite;
	HANDLE hFatherWriteTemp, hChildRead, hFatherWrite;
	HANDLE hErrorWrite;
	HANDLE hThread;
	DWORD ThreadId;

	SECURITY_ATTRIBUTES sa;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	//create pipe where father reads what child writes
	if (!CreatePipe(&hFatherReadTemp, &hChildWrite, &sa, 0));
		//DisplayError(T"CreatePipe");
	/*
									 ____________
			CHILD WRITES	  --->  |            |	--->	FATHER READS FROM		
			IN THE PIPE				|____________|			THE PIPE
		(hWritePipe =hChildWrite)                         (hReadPipe =hFatherRead)

	*/
	
	if (!DuplicateHandle(GetCurrentProcess(), hChildWrite,
		GetCurrentProcess(), &hErrorWrite, 0,
		TRUE, DUPLICATE_SAME_ACCESS));
		//DisplayError("DuplicateHandle");

		//create pipe where child reads what father writes
	if (!CreatePipe(&hChildRead, &hFatherWriteTemp, &sa, 0));
		//DisplayError("CreatePipe");

		/*
								 ____________
		FATHER WRITES	  --->  |            |	--->	CHILD READS FROM
		IN THE PIPE				|____________|			THE PIPE
		(hWritePipe =hFatherWrite)                         (hReadPipe =hChildRead)

		*/

	if (!DuplicateHandle(GetCurrentProcess(), hFatherReadTemp,
		GetCurrentProcess(),
		&hFatherRead, // Address of new handle.
		0, FALSE, // Make it uninheritable.
		DUPLICATE_SAME_ACCESS));
		//DisplayError("DupliateHandle");

	if (!DuplicateHandle(GetCurrentProcess(), hFatherWriteTemp,
		GetCurrentProcess(),
		&hFatherWrite, // Address of new handle.
		0, FALSE, // Make it uninheritable.
		DUPLICATE_SAME_ACCESS));
		//DisplayError("DupliateHandle");

	CloseHandle(hFatherReadTemp); //DisplayError("CloseHandle");
	CloseHandle(hFatherWriteTemp); //DisplayError("CloseHandle");

	Init(hChildWrite, hChildRead, hErrorWrite);

	CloseHandle(hChildWrite);
	CloseHandle(hChildRead);
	CloseHandle(hErrorWrite);

	// Launch the thread that gets the input and sends it to the child.
	hThread = CreateThread(NULL, 0, ReadFromConsoleAndWritePipe,
		(LPVOID)hFatherWrite, 0, &ThreadId);


	

	// Force the read on the input to return by closing the stdin handle.
	CloseHandle(hStdIn);


	// Tell the thread to exit and wait for thread to die.
	bRunThread = FALSE;

	WaitForSingleObject(hThread, INFINITE);

	ReadFromPipeAndWriteInConsole(hChildRead);

	CloseHandle(hFatherRead);
	CloseHandle(hFatherWrite);

	return 0;
}

/*void Init() {

	STARTUPINFO si = { sizeof(si) };
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);

	PROCESS_INFORMATION pi;
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES) };

	HANDLE hReadPipe;
	HANDLE hWritePipeChild;

	HANDLE hReadPipeChild;
	HANDLE hWritePipe;
		
	TCHAR childName[MAX_PATH] = _T("TalkChild.exe");

	//create child process
	CreateProcess(NULL, childName, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

	CreatePipe(&hReadPipe, &hWritePipeChild, NULL, 0);

	CreatePipe(&hReadPipeChild, &hWritePipe, NULL, 0);
}
*/

void Init(HANDLE hStdOut,HANDLE hStdIn,HANDLE hStdErr)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;

	// Set up the start up info struct.
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.hStdOutput = hStdOut;
	si.hStdInput = hStdIn;
	si.hStdError = hStdErr;
	
	TCHAR childName[MAX_PATH] = _T("Exercicio6Child.exe");

	bool a = CreateProcess(NULL, childName, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);


	// Set global child process handle to cause threads to exit.
	hChildProcess = pi.hProcess;

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

DWORD WINAPI ReadFromConsoleAndWritePipe(LPVOID lpvThreadParam)
{
	CHAR read_buff[256];
	DWORD nBytesRead, nBytesWrote;
	DWORD dwRead;
	HANDLE hPipeWrite = (HANDLE)lpvThreadParam;

	// Get input from our console and send it to child through the pipe.
	while (bRunThread)
	{
		ReadConsole(hStdIn, read_buff, 1, &dwRead, NULL);
	
		int c = 0;
		while (read_buff[c] != '\0') {
			printf("%c", read_buff[c]);
			c++;
		}
		bool a = WriteFile(hPipeWrite, read_buff, dwRead, &dwRead, NULL);
		{
			if (GetLastError() == ERROR_NO_DATA)
				break; // Pipe was closed (normal exit path).
		}
	}

	return 1;
}




