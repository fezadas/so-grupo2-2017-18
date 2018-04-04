#pragma once

#ifdef EX6_EXPORTS
#define EX6_API __declspec(dllexport)
#else
#define EX6_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
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
	enum FLIP_enum_t { vertical, horizontal };
	EX6_API VOID BMP_Flip(TCHAR filenameIn, TCHAR filenameOut, FLIP_enum_t type);
	#define BLOCKSIZE (0x100000)

#ifdef  __cplusplus
}
#endif