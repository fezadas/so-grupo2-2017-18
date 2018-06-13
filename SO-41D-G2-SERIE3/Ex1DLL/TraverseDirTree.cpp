#include "stdafx.h"
#include "traversedir.h"


static BOOL hasExtension(LPCSTR fileName, LPCSTR extension) {
	char *strend = (char *)fileName + strlen(fileName) - 1;
	char *p = strend;
	while (p >= fileName && *p != '.') --p;
	if (p < fileName) return FALSE;
	while (p <= strend && *extension != 0 && *p == *extension) {
		p++;
		extension++;
	}
	return *extension == 0;
}

/**
*	Traverses recursivelly a folder and executes an action per found file
*	For simplicity, it deals only with ascii char names
*	Parameters:
*		dir			- the directory to traverse
*		action		- the pointer to executed code per file
*		arg			- the argument passed to action
*		extension   - the filter to select files to process (ex: ".bmp")
**/
INT TraverseDirTree(LPCSTR dir, LPCSTR extension, ACTION  action, LPVOID arg) {

	CHAR buffer[MAX_PATH];		// auxiliary buffer
								// the buffer is needed to define a match string that guarantees 
								// a priori selection for all files
	sprintf_s(buffer, "%s\\%s", dir, "*.*");
	INT count=0;

	WIN32_FIND_DATAA fileData;

	HANDLE fileIter = FindFirstFileA(buffer, &fileData);
	if (fileIter == INVALID_HANDLE_VALUE) return -1;

	// Iterate through current directory and sub directories
	do {
		sprintf_s(buffer, "%s/%s", dir, fileData.cFileName);
		if ((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
			// Not processing "." and ".." files!
			if (strcmp(fileData.cFileName, ".") && strcmp(fileData.cFileName, "..")) {
				// Recursively process child directory
				TraverseDirTree(buffer, extension, action, arg);
			}
		}
		else {
			// Process only  archives with selected extension
			if (hasExtension(buffer, extension))
				if (!action(buffer, fileData.cFileName, arg)) {
					count++;
					break;
				}
				
		}
	} while (FindNextFileA(fileIter, &fileData) == TRUE);

	FindClose(fileIter);
	return count;
}