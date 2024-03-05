#pragma once

#include <Windows.h>
#include "../config.h"
#include "hexdump.h"

BOOL ReadBuffer(
	LPCTSTR,
	LARGE_INTEGER,
	DWORD,
	PUCHAR,
	ULONG,
	PULONG);

BOOL WriteBuffer(
	LPCTSTR,
	LARGE_INTEGER,
	DWORD,
	PUCHAR,
	ULONG,
	PULONG);

BOOL UpdateFileAttributes(
	LPCTSTR,
	DWORD,
	BOOL
);

BOOL DeleteFileZero(
	LPCTSTR
);

BOOL FakeDeleteFile(
	LPCTSTR
);