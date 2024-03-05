#pragma once

#include "resource.h"

#define RESOURCE_PASSWORD "WNcry@2olP"

/*
definition of CreateFont:
https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-createfonta

HFONT CreateFont(
  int     cHeight,
  int     cWidth,
  int     cEscapement,
  int     cOrientation,
  int     cWeight,
  DWORD   bItalic,
  DWORD   bUnderline,
  DWORD   bStrikeOut,
  DWORD   iCharSet,
  DWORD   iOutPrecision,
  DWORD   iClipPrecision,
  DWORD   iQuality,
  DWORD   iPitchAndFamily,
  LPCTSTR pszFaceName
);
*/

#define DefaultFont(cHeight, bUnderline) \
	(CreateFont( \
		cHeight, \
		0, \
		0, \
		0, \
		FW_BLACK, \
		FALSE, \
		bUnderline, \
		FALSE, \
		DEFAULT_CHARSET, \
		OUT_OUTLINE_PRECIS, \
		CLIP_DEFAULT_PRECIS, \
		CLEARTYPE_QUALITY, \
		VARIABLE_PITCH, \
		_T("Arial")))

#define SetDlgItemFont(rcID, font) \
	(SendMessage( \
		GetDlgItem(hDlg, rcID), \
		WM_SETFONT, \
		(WPARAM)font, NULL))