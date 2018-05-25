#include "stdafx.h"

DWORD WINAPI ReadFromPipe(LPVOID hRead);

HANDLE LaunchProcess(TCHAR *cmdLine) {
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	if (!CreateProcess(NULL, cmdLine, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
		return NULL;
	}
	return pi.hProcess;
}

HANDLE Parent() {

	HANDLE hFatherRead, hChildWrite;
	HANDLE hChildRead, hFatherWrite;

	DWORD ThreadId1;
	DWORD ThreadId2;

	char buffer[] = "parent!\r\n";

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	CreatePipe(&hFatherRead, &hChildWrite, &sa, 0);
	/*							 ____________
		CHILD WRITES	  --->  |            |	--->	FATHER READS FROM
		IN THE PIPE				|____________|			THE PIPE
		(hWritePipe =hChildWrite)                         (hReadPipe =hFatherRead)
	*/

	CreatePipe(&hChildRead, &hFatherWrite, &sa, 0);
	/*							 ____________
		FATHER WRITES	  --->  |            |	--->	CHILD READS FROM
		IN THE PIPE				|____________|			THE PIPE
		(hWritePipe =hFatherWrite)                         (hReadPipe =hChildRead)
	*/
	
	DWORD written;
	TCHAR cmd[250];
	_stprintf_s(cmd, _T("Exercicio6.exe %d %d %d %d"), (DWORD)hChildRead, (DWORD)hChildWrite, (DWORD)hFatherRead, (DWORD)hFatherWrite);
	
	HANDLE hThread2 = CreateThread(NULL, 0, ReadFromPipe,
		(LPVOID)hFatherRead, CREATE_SUSPENDED, &ThreadId2);

	HANDLE child = LaunchProcess(cmd);

	CloseHandle(hChildRead);
	CloseHandle(hChildWrite);

	ResumeThread(hThread2);
	
	return hFatherWrite;
}


HANDLE Child(_TCHAR* argv[]) {
	DWORD written;

	DWORD ThreadId1;
	DWORD ThreadId2;

	HANDLE hChildRead = (HANDLE)_tstoi(argv[1]);
	HANDLE hChildWrite = (HANDLE)_tstoi(argv[2]);

	CloseHandle((HANDLE)_tstoi(argv[3]));
	CloseHandle((HANDLE)_tstoi(argv[4]));

	HANDLE hThread2 = CreateThread(NULL, 0, ReadFromPipe,
		(LPVOID)hChildRead, CREATE_SUSPENDED, &ThreadId2);

	ResumeThread(hThread2);

	return hChildWrite;
}

DWORD WINAPI ReadFromPipe(LPVOID hRead) {
	CHAR read_buff[256];
	memset(&read_buff, 0, 256);
	DWORD nBytesRead;
	DWORD nCharsWritten;

	while (TRUE)
	{
		printf("\n>");
		if (!ReadFile((HANDLE)hRead, read_buff, 256, &nBytesRead, NULL)) {
			if (GetLastError() == ERROR_BROKEN_PIPE)
				break; // pipe done - normal exit path.
		}
		else {
			if (nBytesRead == 0)break;
			WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), read_buff, nBytesRead - 1, &nCharsWritten, NULL);
		}
	}
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	printf("Press '#' to Exit Console \n");
	HANDLE hWrite;
	if (argc == 1) {
		hWrite = Parent();
		printf("Father Console: \n");
	}
	else {
		hWrite = Child(argv);
		printf("Child Console: \n");
	}
	
	while (true) {
		CHAR read_buff[256];
		DWORD nBytesRead, nBytesWrote;
		// Get input from our console and send it to child through the pipe.
			if (ReadFile(GetStdHandle(STD_INPUT_HANDLE), read_buff, 256, &nBytesRead, NULL)) {
				if ('#'== read_buff[0]) {
					CloseHandle(hWrite);
					exit(0);
				}
				else if (!WriteFile((HANDLE)hWrite, read_buff, nBytesRead - 1, &nBytesWrote, NULL)) {
					if (GetLastError() == ERROR_NO_DATA) 
						break; // Pipe was closed (normal exit path).
				}
			}
		}
	return 0;
}

