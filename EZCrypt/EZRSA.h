#pragma once

#include <Windows.h>
#include <bcrypt.h>

class EZRSA
{
	BCRYPT_HANDLE m_hProv;
	BCRYPT_KEY_HANDLE m_hKey;
public:
	EZRSA();
	~EZRSA();
	BOOL GenKey();
	BOOL Import(LPCWSTR, PUCHAR, ULONG);
	BOOL Import(LPCWSTR, LPCTSTR);
	BOOL Export(LPCWSTR, PUCHAR, ULONG, PULONG);
	BOOL Export(LPCWSTR, LPCTSTR, EZRSA*);
	BOOL Encrypt(PUCHAR, ULONG, PUCHAR, ULONG, PULONG);
	BOOL Decrypt(PUCHAR, ULONG, PUCHAR, ULONG, PULONG);
};

typedef EZRSA* PEZRSA;

BOOL RSAKeyMatch(PEZRSA, PEZRSA);
BOOL RSABlobMatch(PUCHAR, ULONG, PUCHAR, ULONG);
BOOL RSAFileMatch(LPCTSTR, LPCTSTR);