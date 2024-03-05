#include "DecQueue.h"

DECQUEUE::DECQUEUE(HWND hWnd = NULL)
{
    m_hStopEvent = CreateEvent(
        NULL,
        0,
        FALSE,
        NULL);
    m_hFillCount = CreateSemaphore(
        NULL,
        0,
        MAXQUEUE,
        NULL);
    m_hEmptyCount = CreateSemaphore(
        NULL,
        MAXQUEUE,
        MAXQUEUE,
        NULL);
    InitializeCriticalSection(&m_CriticalSection);
    m_iHead = 0;
    m_iTail = 0;
    m_nCount = 0;
    m_Status = IDC_DECQUEUE_NONE;
    m_Command = IDC_DECQUEUE_NONE;
#ifndef DECQUEUE_SCANONLY
    m_pDecryptor = new WanaDecryptor();
#endif
    m_hWnd = hWnd;
}

DECQUEUE::~DECQUEUE()
{
    if (m_hFillCount) {
        CloseHandle(m_hFillCount);
    }
    if (m_hEmptyCount) {
        CloseHandle(m_hEmptyCount);
    }
    if (m_hStopEvent) {
        CloseHandle(m_hStopEvent);
    }
    DeleteCriticalSection(&m_CriticalSection);
#ifndef DECQUEUE_SCANONLY
    delete m_pDecryptor;
#endif
}

void DECQUEUE::SendData(
    LPCTSTR pName,
    DWORD dwFileAttributes)
{
    WaitForSingleObject(m_hEmptyCount, INFINITE);
    EnterCriticalSection(&m_CriticalSection);
    m_aFileInfo[m_iHead].m_szName[0] = 0;
    m_aFileInfo[m_iHead].m_dwFileAttributes = 0;
    if (!pName) {
        m_Status = IDC_DECQUEUE_DONE;
    }
    else if (m_nCount < MAXQUEUE) {
        m_Status = IDC_DECQUEUE_DATA;
        if (pName) {
            _tcscpy_s(m_aFileInfo[m_iHead].m_szName,
                MAX_PATH,
                pName);
        }
        m_aFileInfo[m_iHead].m_dwFileAttributes =
            dwFileAttributes;
        m_iHead = (m_iHead + 1) % MAXQUEUE;
        m_nCount++;
    }
    LeaveCriticalSection(&m_CriticalSection);
    ReleaseSemaphore(m_hFillCount, 1, NULL);
    if (m_hWnd) {
        SendMessage(m_hWnd, WM_USER, m_Status, NULL);
    }
}

BOOL DECQUEUE::RecvData(
    TCHAR* pName,
    PDWORD pdwFileAttributes)
{
    WaitForSingleObject(m_hFillCount, INFINITE);
    EnterCriticalSection(&m_CriticalSection);
    BOOL bResult = TRUE;
    if (m_Status == IDC_DECQUEUE_DONE) {
        bResult = FALSE;
    }
    if (m_nCount > 0) {
        if (pName) {
            _tcscpy_s(pName,
                MAX_PATH,
                m_aFileInfo[m_iTail].m_szName);
        }
        if (pdwFileAttributes) {
            *pdwFileAttributes =
                m_aFileInfo[m_iTail].m_dwFileAttributes;
        }
        m_iTail = (m_iTail + 1) % MAXQUEUE;
        m_nCount--;
    }
    LeaveCriticalSection(&m_CriticalSection);
    ReleaseSemaphore(m_hEmptyCount, 1, NULL);
    return bResult;
}

BOOL DECQUEUE::Traverse(
    LPCTSTR pPath,
    DWORD dwAttributes = 0,
    DWORD nLevel = 0)
{
    BOOL bResult = TRUE;
    if (CheckStopEvent()) {
        return FALSE;
    }
    if (!dwAttributes) {
        dwAttributes = GetFileAttributes(pPath);
        if (dwAttributes == INVALID_FILE_ATTRIBUTES) {
            return TRUE;
        }
    }
    if (!(dwAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
#ifndef DECQUEUE_SCANONLY
        LPCTSTR pSuffix = _tcsrchr(pPath, _T('.'));
        if (pSuffix) {
            if (!_tcsicmp(pSuffix, WZIP_SUFFIX_CIPHER) ||
                !_tcsicmp(pSuffix, WZIP_SUFFIX_WRITESRC)) {
                m_pDecryptor->Decrypt(pPath);
                SendData(pPath, dwAttributes);
            }
            else if (!_tcsicmp(pSuffix, WZIP_SUFFIX_TEMP)) {
                DeleteFile(pPath);
            }
        }
#else
        SendData(pPath, dwAttributes);
#endif
    }
    else {
        SendData(pPath, dwAttributes);
        TCHAR szFullPath[MAX_PATH + 1];
        WIN32_FIND_DATA FindFileData;
        _stprintf_s(szFullPath, _T("%s\\*.*"), pPath);
        HANDLE hFind = FindFirstFile(
            szFullPath,
            &FindFileData);
        if (INVALID_HANDLE_VALUE == hFind) {
            return TRUE;
        }
        do {
            if (!_tcscmp(FindFileData.cFileName, _T(".")) ||
                !_tcscmp(FindFileData.cFileName, _T(".."))) {
                continue;
            }
            _stprintf_s(szFullPath,
                _T("%s\\%s"),
                pPath,
                FindFileData.cFileName);
            bResult = Traverse(
                szFullPath,
                FindFileData.dwFileAttributes,
                nLevel + 1);
        } while (bResult &&
            FindNextFile(hFind, &FindFileData) != 0);
        FindClose(hFind);
    }
    if (nLevel <= 0) {
        SendData(NULL, 0);
    }
    return bResult;
}

void DECQUEUE::Stop()
{
    SetEvent(m_hStopEvent);
}

BOOL DECQUEUE::CheckStopEvent()
{
    DWORD retval = WaitForSingleObject(m_hStopEvent, 0);
    if (WAIT_OBJECT_0 == retval) {
        return TRUE;
    }
    return FALSE;
}

#ifndef ENCRYPT_ROOT_PATH
#define ENCRYPT_ROOT_PATH _T("C:\\")
#endif

DWORD WINAPI DecQueueThread(
    _In_ LPVOID lpParameter
)
{
    PDECQUEUE pQueue = (PDECQUEUE)lpParameter;
    BOOL bResult = TRUE;
    if (pQueue->m_Start) {
        bResult = pQueue->Traverse(pQueue->m_Start);
    }
    else {
        TCHAR szRootPathName[16] = ENCRYPT_ROOT_PATH;
        for (INT DiskNO = 25; DiskNO >= 0; DiskNO--) {
            DWORD Drives = GetLogicalDrives();
            if ((Drives >> DiskNO) & 1) {
                szRootPathName[0] = DiskNO + 65;
                bResult = pQueue->Traverse(szRootPathName);
                if (!bResult) {
                    break;
                }
            }
        }
    }
    ExitThread(0);
}

#ifdef DECQUEUE_SCANONLY
int main()
{
    PDECQUEUE pQueue = new DECQUEUE();
    HANDLE hThread;
    TCHAR szFileName[MAX_PATH + 1];
    DWORD dwAttributes;
    hThread = CreateThread(NULL, 0, DecQueueThread, pQueue, 0, NULL);
    int i = 0;
    while (TRUE) {
        if (!pQueue->RecvData(szFileName, &dwAttributes)) {
            break;
        }
        _tprintf(_T("%d Recv %s\n"), i, szFileName);
        i++;
        if (i >= 100) {
            pQueue->Stop();
        }
    }
    return 0;
}
#endif