// Common.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tchar.h>
#include "hexdump.h"
#include "ezfile.h"

#define HEXDUMP_BUFSIZE (1024 * 64)

BOOL do_hexdump(LPCTSTR pFileName)
{
	UCHAR abBuffer[HEXDUMP_BUFSIZE];
	ULONG cbBuffer = sizeof(abBuffer);
	ReadBuffer(pFileName, { 0 }, 0, abBuffer, sizeof(abBuffer), &cbBuffer);
	hexdump(abBuffer, cbBuffer);
	return TRUE;
}

INT _tmain(INT argc, LPCTSTR argv[])
{
	if (argc > 2) {
		if (!_tcsicmp(argv[1], _T("hexdump"))) {
			do_hexdump(argv[2]);
		}
	}
	else {
		_tprintf(_T("Usage: %s hexdump FILENAME\n"), argv[0]);
	}
	return 0;
}
