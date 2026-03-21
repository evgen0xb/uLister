/* 
	uLister is a powerful document viewer which can open more than 500 file formats.
	Copyright (C) 2011 Egor Vlaznev 
	
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include "ulister.h"

extern HINSTANCE  hInst;
extern HANDLE     hViewerLibrary;
extern int        numInstances;
int keepinmemory;
wchar_t ininoloadtypes[ULISTMAXBUF];
wchar_t inionlyloadtypes[ULISTMAXBUF];
wchar_t ininopreviewtypes[ULISTMAXBUF];
wchar_t inionlypreviewtypes[ULISTMAXBUF];
wchar_t inipath[MAX_PATH];

///////////////////////////////////////////////////////////////////////////////////////////////////
VTWORD gettype(wchar_t* FileToLoad) {
	typedef VTDWORD(*FIInitFUNC)(VTVOID);
	typedef VTWORD(*FIIdFileExFUNC)(VTDWORD, VTVOID *, VTDWORD, VTWORD *, VTLPTSTR, VTWORD);
	typedef VTDWORD(*FIDeInitFUNC)(VTVOID);

	SCCERR  err = SCCERR_UNKNOWN;
	VTDWORD dwFlags = FIFLAG_NORMAL;
	VTWORD  wType = FI_UNKNOWN;
	VTBYTE  pTypeName[256];


	FIInitFUNC FIInit;
	FIIdFileExFUNC FIIdFileEx;
	FIDeInitFUNC FIDeInit;

	HINSTANCE hinstDLL;

	hinstDLL = loadlib(L"SCCFI.DLL");
	if (hinstDLL) {
		FIInit = (FIInitFUNC)GetProcAddress(hinstDLL, "FIInit");
		FIIdFileEx = (FIIdFileExFUNC)GetProcAddress(hinstDLL, "FIIdFileEx");
		FIDeInit = (FIDeInitFUNC)GetProcAddress(hinstDLL, "FIDeInit");
		if (FIInit&&FIIdFileEx&&FIDeInit) {
			FIInit();
			err = FIIdFileEx(IOTYPE_UNICODEPATH, FileToLoad, dwFlags, &wType, (VTLPTSTR)pTypeName, 256);
			FIDeInit();
		}
		FreeLibrary(hinstDLL);
	}
	else return FI_UNKNOWN;

	if (err != SCCERR_OK) return FI_UNKNOWN;
	return 	wType;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
int CheckFile(wchar_t* FileToLoad, wchar_t* onlyload, wchar_t* noload) {
	VTWORD  wType;
	wType = gettype(FileToLoad);

	wchar_t ftype[10];
	_itow_s(wType, ftype, 10, 10);
	if (wcslen(onlyload) > 0) {
		if (wcsstr(onlyload, ftype))return 1;
		return NULL;
	}
	if (wcslen(noload) > 0) {
		if (wcsstr(noload, ftype))return NULL;
	}

	return 1;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void LoadFile(HWND hViewWnd, wchar_t* FileToLoad) {
	SCCVWVIEWFILE80  locViewFile;
	locViewFile.dwSize = sizeof(SCCVWVIEWFILE80);
	locViewFile.dwSpecType = IOTYPE_UNICODEPATH;
	locViewFile.pSpec = (VTVOID *)FileToLoad;
	locViewFile.dwViewAs = 0;
	locViewFile.bUseDisplayName = FALSE;
	locViewFile.bDeleteOnClose = FALSE;
	locViewFile.dwFlags = 0;
	locViewFile.dwReserved1 = 0;
	locViewFile.dwReserved2 = 0;
	SendMessage(hViewWnd, SCCVW_VIEWFILE, 0, (LPARAM)&locViewFile);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
int loadthisfile(LPARAM lParam) {
    PSCCVWVIEWTHISFILE40    locVTFPtr40;
    PSCCVWVIEWTHISFILE80    locVTFPtr80;
    locVTFPtr40 = (PSCCVWVIEWTHISFILE40)lParam;
    locVTFPtr80 = (PSCCVWVIEWTHISFILE80)lParam;

	if (GetFileAttributesA((char *)locVTFPtr40->sViewFile.pSpec) != INVALID_FILE_ATTRIBUTES) {
		char compath[MAX_PATH] = "";
		char comdir[MAX_PATH] = "";
		GetEnvironmentVariable("COMMANDER_PATH", comdir, MAX_PATH);
		char tcinipath[MAX_PATH] = "";
		GetEnvironmentVariable("COMMANDER_INI", tcinipath, MAX_PATH);
		if (comdir == NULL)return 1;
		strcat_s(compath, MAX_PATH, comdir);
#ifdef ULISTER64
		strcat_s(compath, MAX_PATH, "\\TOTALCMD64.EXE");
#else
		strcat_s(compath, MAX_PATH, "\\TOTALCMD.EXE");
#endif
		char allpar[MAX_PATH * 2 + 15] = "/S=L ";
		strcat_s(allpar, MAX_PATH * 2 + 15, (char *)locVTFPtr40->sViewFile.pSpec);
		strcat_s(allpar, MAX_PATH * 2 + 15, " /I=\"");
		strcat_s(allpar, MAX_PATH * 2 + 15, tcinipath);
		strcat_s(allpar, MAX_PATH * 2 + 15, "\"");
		ShellExecuteA(NULL, "open", compath, allpar, NULL, SW_RESTORE);
	}
	else
		if (GetFileAttributesW((wchar_t *)locVTFPtr80->sViewFile.pSpec) != INVALID_FILE_ATTRIBUTES) {
			MessageBoxW(0, (wchar_t *)locVTFPtr80->sViewFile.pSpec, 0, 0);
			wchar_t compath[MAX_PATH] = L"";
			wchar_t comdir[MAX_PATH] = L"";
			GetEnvironmentVariableW(L"COMMANDER_PATH", comdir, MAX_PATH);
			wchar_t tcinipath[MAX_PATH] = L"";
			GetEnvironmentVariableW(L"COMMANDER_INI", tcinipath, MAX_PATH);
			if (comdir == NULL)return 1;
			wcscat_s(compath, MAX_PATH, comdir);
#ifdef ULISTER64
			wcscat_s(compath, MAX_PATH, L"\\TOTALCMD64.EXE");
#else
			wcscat_s(compath, MAX_PATH, L"\\TOTALCMD.EXE");
#endif
			wchar_t allpar[MAX_PATH * 2 + 15] = L"/S=L ";
			wcscat_s(allpar, MAX_PATH * 2 + 15, (wchar_t *)locVTFPtr80->sViewFile.pSpec);
			wcscat_s(allpar, MAX_PATH * 2 + 15, L" /I=\"");
			wcscat_s(allpar, MAX_PATH * 2 + 15, tcinipath);
			wcscat_s(allpar, MAX_PATH * 2 + 15, L"\"");
			ShellExecuteW(NULL, L"open", compath, allpar, NULL, SW_RESTORE);
		}
    return GetLastError();
}
///////////////////////////////////////////////////////////////////////////////////////////////////
HBITMAP getpreview(wchar_t* FileToLoad,int width,int height) {

	if (!hViewerLibrary)hViewerLibrary = loadlib(L"SCCVW.DLL");
	if (!hViewerLibrary)return NULL;
	numInstances++;
	HWND hViewWnd = CreateWindow("SCCVIEWER", NULL, WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, 0, hInst, NULL);
	if (!IsWindow(hViewWnd)) {
		numInstances--;
		return NULL;
	}
	LoadFile(hViewWnd, FileToLoad);

	HDC OutputDC,FormatDC;
	HBITMAP bitmap,oldbitmap;
	SCCVWDRAWPAGE41  locDrawPage;
	BITMAPINFOHEADER  locBIH;


	FormatDC = GetDC(hViewWnd);
	OutputDC = CreateCompatibleDC(FormatDC);

	locBIH.biSize = sizeof(BITMAPINFOHEADER);
	locBIH.biWidth = width;
	locBIH.biHeight = height;
	locBIH.biPlanes = 1;
	locBIH.biBitCount = 24;
	locBIH.biCompression = BI_RGB;
	locBIH.biSizeImage = 0;
	locBIH.biXPelsPerMeter = 0;
	locBIH.biYPelsPerMeter = 0;
	locBIH.biClrUsed = 0;
	locBIH.biClrImportant = 0;

	bitmap = CreateDIBitmap(FormatDC, &locBIH, 0, NULL, NULL, DIB_RGB_COLORS);

	oldbitmap = (HBITMAP)SelectObject(OutputDC, bitmap);
	Rectangle(OutputDC, 0, 0, width, height);

	locDrawPage.dwSize = sizeof(SCCVWDRAWPAGE41);
	locDrawPage.dwPageToDraw = 0;
	locDrawPage.dwReserved = 0;
	locDrawPage.dwFlags = SCCVW_DPFLAG_DETERMINEOUTPUTTYPE;
	locDrawPage.lUnitsPerInch = 1440;
	locDrawPage.lFormatWidth = 12240;
	locDrawPage.lFormatHeight = 15840;
	locDrawPage.lTop = 0;
	locDrawPage.lLeft = 0;
	locDrawPage.lBottom = height;
	locDrawPage.lRight = width;
	locDrawPage.hOutputDC = OutputDC;
	locDrawPage.hFormatDC = FormatDC;
	SendMessage(hViewWnd, SCCVW_INITDRAWPAGE, 0, 0);
	SendMessage(hViewWnd, SCCVW_DRAWPAGE, 0, (LPARAM)(PSCCVWDRAWPAGE41)&locDrawPage);
	SelectObject(OutputDC, oldbitmap);
	DeleteDC(OutputDC);
	ReleaseDC(hViewWnd, FormatDC);
	SendMessage(hViewWnd, SCCVW_DEINITDRAWPAGE, 0, 0);
	SendMessage(hViewWnd, SCCVW_CLOSEFILE, 0, 0L);
	DestroyWindow(hViewWnd);
	numInstances--;
	if ((hViewerLibrary != NULL) && (keepinmemory == 0) && (numInstances == 0)) {
		FreeLibrary((HINSTANCE)hViewerLibrary);
		hViewerLibrary = NULL;
	}
	return bitmap;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void iniparse() {
	const wchar_t *ULISTERSECTION = L"uLister";
	const wchar_t *ULISTERINI = L"\\ulister.ini";
	const wchar_t *OIT_DATA_PATH = L"OIT_DATA_PATH";

	wchar_t inioptdir[MAX_PATH];
	wchar_t oitdatapath[MAX_PATH];
	wchar_t *pathposition;
	int retlength;

	GetModuleFileNameW(hInst, inipath, MAX_PATH); // самый высокий приоритет расположени€ ulister.ini в каталоге с плагином
	inipath[MAX_PATH - 1] = L'\0'; // Windows XP fix: —трока усечена до символов nSize Ќќ не завершаетс€ значением NULL
	if (pathposition = wcsrchr(inipath, L'\\'))
		*pathposition = L'\0';
	wcscat_s(inipath, MAX_PATH, ULISTERINI);

	if (GetFileAttributesW(inipath) == INVALID_FILE_ATTRIBUTES)
	{
		retlength = GetEnvironmentVariableW(L"COMMANDER_INI", inipath, MAX_PATH); // иначе посмотреть ulister.ini там же, где и wincmd.ini
		if ((retlength < MAX_PATH) && retlength)
		{
			if (pathposition = wcsrchr(inipath, L'\\'))
				*pathposition = L'\0';
			wcscat_s(inipath, MAX_PATH, ULISTERINI);
		}

		if (GetFileAttributesW(inipath) == INVALID_FILE_ATTRIBUTES)
		{
			GetEnvironmentVariableW(L"APPDATA", inipath, MAX_PATH); // самый низкий приоритет ulister.ini в %APPDATA%
			wcscat_s(inipath, MAX_PATH, ULISTERINI);
		}
	}

	wchar_t buf[ULISTMAXBUF] = L"";
	GetPrivateProfileStringW(ULISTERSECTION, L"keepinmemory", L"1", buf, ULISTMAXBUF, inipath);
	if (wcscmp(buf, L"1") == 0) keepinmemory = 1; else keepinmemory = 0;

	GetPrivateProfileStringW(ULISTERSECTION, L"optionsdir", L"", inioptdir, MAX_PATH, inipath);

	GetPrivateProfileStringW(ULISTERSECTION, L"noloadtypes", L"", ininoloadtypes, ULISTMAXBUF, inipath);
	GetPrivateProfileStringW(ULISTERSECTION, L"onlyloadtypes", L"", inionlyloadtypes, ULISTMAXBUF, inipath);
	GetPrivateProfileStringW(ULISTERSECTION, L"nopreviewtypes", L"", ininopreviewtypes, ULISTMAXBUF, inipath);
	GetPrivateProfileStringW(ULISTERSECTION, L"onlypreviewtypes", L"", inionlypreviewtypes, ULISTMAXBUF, inipath);

	if (wcslen(inioptdir) > 0) {
		ExpandEnvironmentStringsW(inioptdir, oitdatapath, MAX_PATH);
		SetEnvironmentVariableW(OIT_DATA_PATH, oitdatapath);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////
wchar_t* getlibpath(wchar_t *libname,wchar_t *libpath) {
	wchar_t *pathposition;
	GetModuleFileNameW(hInst, libpath, MAX_PATH);
	if (pathposition = wcsrchr(libpath, L'\\'))
		*pathposition = L'\0';

#ifdef ULISTER64
	wcscat_s(libpath, MAX_PATH,L"\\redist64\\");
#else
	wcscat_s(libpath, MAX_PATH,L"\\redist32\\");
#endif

	wcscat_s(libpath, MAX_PATH, libname);
	if (GetFileAttributesW(libpath) != INVALID_FILE_ATTRIBUTES) return libpath;
	return NULL;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
int libexist(wchar_t *libname) {
	wchar_t path[MAX_PATH];
	return (int) getlibpath(libname, path);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
HINSTANCE loadlib(wchar_t * libname) {
#ifdef ULISTER64
	const wchar_t *title = L"uLister x64 Error";
#else
	const wchar_t *title = L"uLister x86 Error";
#endif
	HANDLE lib;
	wchar_t path[MAX_PATH];
	if (getlibpath(libname, path))
	{
		lib = LoadLibraryExW(path, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	}
	else
	{
		DWORD dwError = GetLastError();
		wchar_t buf[ULISTMAXBUF];
		swprintf_s(buf, ULISTMAXBUF,
		L"Can't load:\n"
		L"[%s].\n"
		L"Error code: 0x%08X   (%lu)\n"
		L"INI=[%s]\n\n"
		L"Check Outside In dlls in plugin dir.\n"
		L"See readme.txt, install section.", path, dwError, dwError, inipath);

		MessageBoxW(NULL, buf, title, MB_OK);
		return 0;
	}
	return (HINSTANCE) lib;
}

