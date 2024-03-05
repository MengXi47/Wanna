#pragma once
#include <Windows.h>
#include "WanaZip.h"

class WanaDecryptor
{
	PWanaZip m_pWanaZip;
	BOOL DecryptTraverse(LPCTSTR);
public:
	WanaDecryptor();
	~WanaDecryptor();
	BOOL InitWanaZip(DWORD);
	BOOL Decrypt(LPCTSTR);
	BOOL DecryptAll();
};

typedef WanaDecryptor* PWanaDecryptor;