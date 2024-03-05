#pragma once

#include <Windows.h>

ULONG hexdump(PUCHAR, ULONG);

BOOL ReadFile_DEBUG(
	HANDLE,
	LPVOID,
	DWORD,
	LPDWORD,
	LPOVERLAPPED);

BOOL WriteFile_DEBUG(
	HANDLE,
	LPCVOID,
	DWORD,
	LPDWORD,
	LPOVERLAPPED);
