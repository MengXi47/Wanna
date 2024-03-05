#include "hexdump.h"
#include <tchar.h>
#ifndef DEBUG
#define DEBUG(fmt, ...) (_tprintf(_T(fmt), __VA_ARGS__))
#endif
ULONG hexdump(PUCHAR data, ULONG size)
{
	ULONG nResult = 0;
	for (ULONG i = 0; i < size; i += 16) {
		nResult += DEBUG("%08X |", i);
		for (ULONG j = 0; j < 16; j++) {
			if (i + j < size) {
				nResult += DEBUG(" %02X", data[i + j]);
			}
			else {
				nResult += DEBUG("   ");
			}
			if ((j + 1) % 8 == 0) {
				nResult += DEBUG(" ");
			}
		}
		nResult += DEBUG("|");
		for (ULONG j = 0; j < 16; j++) {
			if (i + j < size) {
				UCHAR k = data[i + j];
				UCHAR c = k < 32 || k > 127 ? '.' : k;
				nResult += DEBUG("%c", c);
			}
			else {
				nResult += DEBUG(" ");
			}
		}
		nResult += DEBUG("\n");
	}
	return nResult;
}

BOOL ReadFile_DEBUG(
	HANDLE       hFile,
	LPVOID       lpBuffer,
	DWORD        nNumberOfBytesToRead,
	LPDWORD      lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped
)
{
	BOOL retval = ReadFile(
		hFile,
		lpBuffer,
		nNumberOfBytesToRead,
		lpNumberOfBytesRead,
		lpOverlapped
	);
	DEBUG("Read %d\n", nNumberOfBytesToRead);
	hexdump((PUCHAR)lpBuffer, nNumberOfBytesToRead);
	return retval;
}

BOOL WriteFile_DEBUG(
	HANDLE       hFile,
	LPCVOID      lpBuffer,
	DWORD        nNumberOfBytesToWrite,
	LPDWORD      lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped
)
{
	DEBUG("Write %d\n", nNumberOfBytesToWrite);
	hexdump((PUCHAR)lpBuffer, nNumberOfBytesToWrite);
	return WriteFile(
		hFile,
		lpBuffer,
		nNumberOfBytesToWrite,
		lpNumberOfBytesWritten,
		lpOverlapped
	);
}

