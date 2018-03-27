#include "stdafx.h"
#include <Windows.h>
#include <wingdi.h>


typedef struct data {
	HANDLE srcFile;
	HANDLE dstFile;
	HANDLE srcMapping;
	HANDLE dstMapping;
	BYTE* srcView;
	BYTE* dstView;
}Data;

#define BLOCKSIZE (0x100000)

void rotate(Data* data, boolean vertical) {

	PBITMAPFILEHEADER bitMapFileHeader = (PBITMAPFILEHEADER)data->dstView;
	PBITMAPINFOHEADER bitMapInfoHeader = (PBITMAPINFOHEADER)(bitMapFileHeader + 1);

	//verifying if the file has 24 bits per pixel
	if (bitMapInfoHeader->biBitCount != 24)
		return;
	//represents the number of pixel in each column
	int height = bitMapInfoHeader->biHeight;
	//represents the number of pixel in each line
	int width = bitMapInfoHeader->biWidth;
	//represents the value to increment to the pointer, 
	//because the information is in addresses multiple of four
	int padding = (4 - ((width * sizeof(RGBTRIPLE) % 4)) % 4);
	PBYTE px = data->dstView + bitMapFileHeader->bfOffBits;

	if (vertical) {

	}
	else {
		for (int i = 0; i < height; i++) {
			//
			RGBTRIPLE* pixelsData = (RGBTRIPLE*)(px);
			//index k starts in the last pixel of the first line
			//index j starts in the first pixel of the first line
			//then we increment j and decrement k, swapping their positions
			for (int j = 0, k = width - 1; j < width; j++, k--) {
				if (k > j) {
					//RBGTriple swap
					RGBTRIPLE temp = pixelsData[j];
					pixelsData[j] = pixelsData[k];
					pixelsData[k] = temp;
				}
				//increment sizeof(RGBTriple) in order to go to the next pixel
				px += sizeof(RGBTRIPLE);

			}
			//ensure that I am in a multiple address of four, otherwise we can get wrong memory infomation
			px += padding;
		}
	}

}

int fileMapping(Data* fileData,LPCTSTR srcFile, LPCTSTR dstFile) {

	LARGE_INTEGER fileSize;
	HANDLE hSrc = INVALID_HANDLE_VALUE, hDst = INVALID_HANDLE_VALUE;
	HANDLE MapSrc = NULL, MapDst = NULL;
	BYTE *src = NULL, *dst = NULL;

	int error = 0;

	hSrc = CreateFile(srcFile,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hSrc == INVALID_HANDLE_VALUE) {
		error = -1;
		//goto terminate;
	}

	hDst = CreateFile(dstFile,
		GENERIC_WRITE | GENERIC_READ,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hDst == INVALID_HANDLE_VALUE) {
		error = -2;
		//goto terminate;
	}

	if (!GetFileSizeEx(hSrc, &fileSize)) {
		error = -3;
		//goto terminate;
	}
	// force the original file size on the destination file
	if (!SetFilePointerEx(hDst, fileSize, NULL, FILE_BEGIN)) {
		error = -3;
		//goto terminate;
	}

	if (!SetEndOfFile(hDst)) {
		error = -3;
		//goto terminate;
	}

	if ((MapSrc = CreateFileMapping(hSrc,
		NULL,
		PAGE_READONLY,
		0,
		0,
		NULL)) == NULL) {
		error = -4;
		//goto terminate;
	}
	if ((MapDst = CreateFileMapping(hDst,
		NULL,
		PAGE_READWRITE,
		0,
		0,
		NULL)) == NULL) {

		error = -5;
		//goto terminate;
	}


	LARGE_INTEGER remaining = fileSize;
	LARGE_INTEGER offset;
	offset.QuadPart = 0;

	// copy the file with successive views 
	// why not copy the file with a single full view?
	while (remaining.QuadPart > (LONGLONG)0) {

		printf("."); fflush(stdout);
		DWORD toTransfer = remaining.QuadPart < BLOCKSIZE
			? (DWORD)remaining.QuadPart : BLOCKSIZE;
		if ((src = (LPBYTE)MapViewOfFile(MapSrc,
			FILE_MAP_READ,
			offset.HighPart,
			offset.LowPart,
			toTransfer)) == NULL) {
			error = -7;
			goto terminate;
		}
		if ((dst = (BYTE *)MapViewOfFile(MapDst,
			FILE_MAP_WRITE,
			offset.HighPart,
			offset.LowPart,
			toTransfer)) == NULL) {
			error = -8;
			goto terminate;
		}
		offset.QuadPart += toTransfer;
		remaining.QuadPart -= toTransfer;
		
		//saving handles and address from createFileMapping and MapViewOfFile
		//in a struct in order to obtain that information later
		
		fileData->srcFile = hSrc;
		fileData->dstFile = hDst;
		fileData->srcMapping = MapSrc;
		fileData->dstMapping = MapDst;
		fileData->srcView = src;
		fileData->dstView = dst;

		//copy data from the source file to the destiny file

		memcpy(dst, src, toTransfer);

		//now we can rotate the RGBTriple structs in our dst file, but first we need
		//to add the offsetBits to dst address

		rotate(fileData,false);

		offset.QuadPart += toTransfer;
		remaining.QuadPart -= toTransfer;
	}
terminate:
	if (src != NULL)	UnmapViewOfFile(src);
	if (dst != NULL)  UnmapViewOfFile(dst);
	if (MapSrc != NULL)	CloseHandle(MapSrc);
	if (MapDst != NULL)  CloseHandle(MapDst);
	if (hSrc != INVALID_HANDLE_VALUE) CloseHandle(hSrc);
	if (hDst != INVALID_HANDLE_VALUE) CloseHandle(hDst);
	return error;
}



int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 3) {
		_tprintf(_T("Usage: copyfile <src> <dst>\n"));
		return 1;
	}
	Data data;
	fileMapping(&data,argv[1], argv[2]);
	return 1;
}