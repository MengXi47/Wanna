#pragma once
#include <Windows.h>

BOOL RetrieveResource(
    ULONG rcID,
    LPCTSTR lpType,
    PUCHAR pbBuffer,
    ULONG cbBuffer,
    PULONG pcbResult
);

PUCHAR AllocResource(
    ULONG rcID,
    PULONG pcbResult
);