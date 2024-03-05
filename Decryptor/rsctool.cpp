#include "rsctool.h"

BOOL RetrieveResource(
    ULONG rcID,
    LPCTSTR lpType,
    PUCHAR pbBuffer,
    ULONG cbBuffer,
    PULONG pcbResult
)
{
    ULONG cMessage = 0;
    HMODULE hModule = GetModuleHandle(NULL);
    if (!hModule) {
        return FALSE;
    }
    HRSRC hResource = FindResource(
        hModule,
        MAKEINTRESOURCE(rcID),
        lpType);
    if (!hResource) {
        return FALSE;
    }
    HGLOBAL hMemory = LoadResource(
        hModule,
        hResource);
    if (!hMemory) {
        return FALSE;
    }
    DWORD dwSize = SizeofResource(
        hModule,
        hResource);
    if (!dwSize) {
        return FALSE;
    }
    if (pcbResult) {
        *pcbResult = dwSize;
    }
    if (pbBuffer) {
        if (cbBuffer < dwSize) {
            return FALSE;
        }
        LPVOID lpAddress = LockResource(hMemory);
        if (!lpAddress) {
            return FALSE;
        }
        CopyMemory(pbBuffer, lpAddress, dwSize);
    }
    return TRUE;
}

PUCHAR AllocResource(ULONG rcID, PULONG pcbResult)
{
    PUCHAR pbBuffer;
    ULONG cbBuffer;
    if (!RetrieveResource(
        rcID,
        RT_RCDATA,
        NULL,
        NULL,
        &cbBuffer)) {
        return NULL;
    }
    if (cbBuffer <= 0) {
        return NULL;
    }
    if (!(pbBuffer = (PUCHAR)HeapAlloc(
        GetProcessHeap(),
        0,
        cbBuffer))) {
        return NULL;
    }
    if (!RetrieveResource(
        rcID,
        RT_RCDATA,
        pbBuffer,
        cbBuffer,
        &cbBuffer)) {
        return NULL;
    }
    if (pcbResult) {
        *pcbResult = cbBuffer;
    }
    return pbBuffer;
}
