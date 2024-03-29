#include "stdafx.h"
#define DEFAULT_SIZE 1

DWORD getTotalPrivatePhysicalSpace(HANDLE hProcess) {
	PSAPI_WORKING_SET_INFORMATION* ws;
	DWORD p[DEFAULT_SIZE];
	DWORD numberOfEntries = 0;
	BYTE * pv;
	size_t pvSize = 0;

	bool flag = QueryWorkingSet(hProcess, p, sizeof(p));
	numberOfEntries = ((PSAPI_WORKING_SET_INFORMATION *)p)->NumberOfEntries;

	if ((flag == FALSE) && (GetLastError() == ERROR_BAD_LENGTH))
	{
		pvSize = sizeof(PSAPI_WORKING_SET_INFORMATION) + ((numberOfEntries - 1) * sizeof(PSAPI_WORKING_SET_BLOCK));
		pv = new BYTE[pvSize * 2];
		//memset(pv, 0, pvSize * 2); // necessário colocar valor a zeros ?
		ws = (PSAPI_WORKING_SET_INFORMATION*)pv;
	}
	;

	flag = QueryWorkingSet(hProcess, (PVOID)ws, pvSize * 2);

	DWORD nPrivatePages = 0;
	for (int idx = 0; idx < numberOfEntries; ++idx, ws++) {
		if (ws->WorkingSetInfo->Shared) continue;
		++nPrivatePages;
	}

	SYSTEM_INFO lp;
	GetSystemInfo(&lp);
	delete [] pv;
	return lp.dwPageSize*nPrivatePages;
}

/*
int main()
{
	//HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
	//	PROCESS_VM_READ, FALSE, 26512);
	HANDLE hProcess = GetCurrentProcess();
	DWORD threshold = 5;

	while (threshold) {
		DWORD t = getTotalPrivatePhysicalSpace(hProcess);
		printf("Private Physical usage : %d KBytes \n", t / 1024);
		Sleep(5000);
		 if(getTotalPrivatePhysicalSpace(hProcess)>t)--threshold;	
	}
	
	printf("Possible Leak of memory.");
		
}
*/

