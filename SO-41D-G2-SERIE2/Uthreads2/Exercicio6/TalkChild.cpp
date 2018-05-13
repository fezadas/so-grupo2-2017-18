#include "stdafx.h"


int main()
{
	FILE*    fp;
	CHAR     szInput[1024];
	TCHAR line[128];

	ZeroMemory(line, 1024);
	while (TRUE)
	{
		printf("Child Console");
		_getts_s(line, 128);
		fflush(NULL);  // Must flush output buffers or else redirection
					   // will be problematic.
		ZeroMemory(line, strlen(szInput));

	}
}