#include "ex6.h"

int main(int argc, char* argv[])
{
	if (argc != 4) {
		printf("Usage: copyfile <src> <dst> <1-vertical/0-horizontal>\n");
		return -1;
	}
	BMP_Flip((TCHAR)argv[1],(TCHAR)argv[2], (TCHAR)argv[3] == 1 ? vertical:horizontal);
	return 0;
}