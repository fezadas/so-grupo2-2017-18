#pragma once
#include "stdafx.h"

typedef BOOL(*ACTION)(LPCSTR pathFileName, LPCSTR fileName, LPVOID arg);

INT TraverseDirTree(LPCSTR dir, LPCSTR extension, ACTION  action, LPVOID arg);
