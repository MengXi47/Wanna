#pragma once
#include <Windows.h>
#include <tchar.h>
#include <iostream>

//#define DECQUEUE_SCANONLY

#ifndef DECQUEUE_SCANONLY
#include "../WannaTry/WanaDecryptor.h"
#endif

#define MAXQUEUE 64

#define IDC_DECQUEUE_NONE 0
#define IDC_DECQUEUE_START 1
#define IDC_DECQUEUE_STOP 2
#define IDC_DECQUEUE_DONE 3
#define IDC_DECQUEUE_DATA 4



struct _FILEINFO {
    TCHAR m_szName[MAX_PATH + 1];
    DWORD m_dwFileAttributes;
};
class DECQUEUE {
private:
    HANDLE m_hStopEvent;
    HANDLE m_hFillCount = NULL;
    HANDLE m_hEmptyCount = NULL;
    CRITICAL_SECTION m_CriticalSection;
    _FILEINFO m_aFileInfo[MAXQUEUE];
    int m_iHead;
    int m_iTail;
    int m_nCount;
    int m_Status;
    int m_Command;
#ifndef DECQUEUE_SCANONLY
    PWanaDecryptor m_pDecryptor;
#endif
    HWND m_hWnd;
public:
    LPCTSTR m_Start;
    DECQUEUE(HWND);
    ~DECQUEUE();
    void SendData(LPCTSTR, DWORD);
    BOOL RecvData(TCHAR*, PDWORD);
    BOOL Traverse(LPCTSTR, DWORD, DWORD);
    void Stop();
    BOOL CheckStopEvent();
};

typedef DECQUEUE* PDECQUEUE;

DWORD WINAPI DecQueueThread(LPVOID);