// worm.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include <Windows.h>
#include <tchar.h>
#include <iostream>
#include "WormProc.h"

INT _tmain(INT argc, LPCTSTR argv[])
{
    LPCTSTR lpFileName = NULL;
    if (argc > 1) {
        lpFileName = argv[1];
    }
    Infect(lpFileName);
    _tprintf(_T("done"));
    return 0;
}
