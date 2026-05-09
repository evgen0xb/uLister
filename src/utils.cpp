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

extern HINSTANCE	hInst;
extern HANDLE		hViewerLibrary;
extern int			numInstances;
extern int			NTLevel;
extern clsVTOptions VTOptions;

int keepinmemory;
wchar_t ininoloadtypes[ULISTMAXBUF];
wchar_t inionlyloadtypes[ULISTMAXBUF];
wchar_t ininopreviewtypes[ULISTMAXBUF];
wchar_t inionlypreviewtypes[ULISTMAXBUF];
wchar_t inipath[MAX_PATH];

#ifdef ULISTER64
wchar_t *REDIST_NT6 = L"\\redist64\\";
wchar_t *REDIST_NT5 = L"\\XPdist64\\";
#else
wchar_t *REDIST_NT6 = L"\\redist32\\";
wchar_t *REDIST_NT5 = L"\\XPdist32\\";
#endif

const wchar_t *CLIPBOARDSECTION = L"clipboard";
const wchar_t *VIEWERSECTION = L"viewer";
const wchar_t *ASKIP = L"SKIP";
const wchar_t *AON	 = L"ON";
const wchar_t *AOFF  = L"OFF";


///////////////////////////////////////////////////////////////////////////////////////////////////
VTWORD GetVTFileType(const wchar_t* FileToLoad) {
	typedef VTDWORD(*FIInitFUNC)(VTVOID);
	typedef VTWORD(*FIIdFileExFUNC)(VTDWORD, const VTVOID *, VTDWORD, VTWORD *, VTLPTSTR, VTWORD);
	typedef VTDWORD(*FIDeInitFUNC)(VTVOID);

	SCCERR  err = SCCERR_UNKNOWN;
	VTDWORD dwFlags = FIFLAG_NORMAL;
	VTWORD  wType = FI_UNKNOWN;
	VTBYTE  pTypeName[256];


	FIInitFUNC FIInit;
	FIIdFileExFUNC FIIdFileEx;
	FIDeInitFUNC FIDeInit;

	HINSTANCE hinstDLL;

	hinstDLL = LoadLibVT(L"SCCFI.DLL");
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
bool IsVTFileTypeAllowed(const wchar_t* FileToLoad, const wchar_t* onlyload, const wchar_t* noload) {
	// TRUE = OK
	// FALSE = refuse
	VTWORD  wType = GetVTFileType(FileToLoad);

	wchar_t FTypeStr[INT64STRMAXBUF];
	_itow_s(wType, FTypeStr, INT64STRMAXBUF, 10);
	if (wcslen(onlyload) > 0)
		if (wcsstr(onlyload, FTypeStr)) return true; else return false;

	if (wcslen(noload) > 0 && wcsstr(noload, FTypeStr)) return false; else return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void LoadVTFile(const HWND hViewWnd, const wchar_t* FileToLoad) {
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




/*
int LoadThisFile(const LPARAM lParam) { // TODO refactor
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
*/





///////////////////////////////////////////////////////////////////////////////////////////////////
HBITMAP GetVTFilePreview(const wchar_t* FileToLoad, const int width, const int height) {

	if (!hViewerLibrary)hViewerLibrary = LoadLibVT(L"SCCVW.DLL");
	if (!hViewerLibrary)return NULL;
	numInstances++; // TODO: numInstances not needed
	HWND hViewWnd = CreateWindow("SCCVIEWER", NULL, WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, 0, hInst, NULL);
	if (!IsWindow(hViewWnd)) {
		numInstances--;
		return NULL;
	}
	LoadVTFile(hViewWnd, FileToLoad);

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
void GetIniPath(wchar_t *_inipath)
{
	const wchar_t *ULISTERINI = L"\\ulister.ini";
	wchar_t *pathposition;
	int retlength;

	GetModuleFileNameW(hInst, _inipath, MAX_PATH); // самый высокий приоритет расположени€ ulister.ini в каталоге с плагином
	_inipath[MAX_PATH - 1] = L'\0'; // Windows XP fix: —трока усечена до символов nSize Ќќ не завершаетс€ значением NULL
	if (pathposition = wcsrchr(_inipath, L'\\'))
		*pathposition = L'\0';
	wcscat_s(_inipath, MAX_PATH, ULISTERINI);

	if (GetFileAttributesW(_inipath) == INVALID_FILE_ATTRIBUTES)
	{
		retlength = GetEnvironmentVariableW(L"COMMANDER_INI", _inipath, MAX_PATH); // иначе посмотреть ulister.ini там же, где и wincmd.ini
		if ((retlength < MAX_PATH) && retlength)
		{
			if (pathposition = wcsrchr(_inipath, L'\\'))
				*pathposition = L'\0';
			wcscat_s(_inipath, MAX_PATH, ULISTERINI);
		}

		if (GetFileAttributesW(_inipath) == INVALID_FILE_ATTRIBUTES)
		{
			GetEnvironmentVariableW(L"APPDATA", _inipath, MAX_PATH); // самый низкий приоритет ulister.ini в %APPDATA%
			wcscat_s(_inipath, MAX_PATH, ULISTERINI);
		}
	}

}
///////////////////////////////////////////////////////////////////////////////////////////////////
void InitUlister()
{
	const wchar_t *ULISTERSECTION = L"uLister";
	const wchar_t *OIT_DATA_PATH = L"OIT_DATA_PATH";

	wchar_t inioptdir[MAX_PATH];
	wchar_t oitdatapath[MAX_PATH];

	wchar_t buf[INT64STRMAXBUF];

	GetPrivateProfileStringW(ULISTERSECTION, L"keepinmemory", L"1", buf, INT64STRMAXBUF, inipath);
	if (_wcsicmp(buf, L"1") == 0) keepinmemory = 1; else keepinmemory = 0;

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
__int8 ReadIniClipbOpt(const wchar_t *optionname)
{
	wchar_t buf[INT64STRMAXBUF];
	__int8 result;

	GetPrivateProfileStringW(CLIPBOARDSECTION, optionname, ASKIP, buf, INT64STRMAXBUF, inipath);
	if (_wcsicmp(buf, AON) == 0) result = Opt::ON;
	else if (_wcsicmp(buf, AOFF) == 0) result = Opt::OFF;
	else result = Opt::SKIP;

	return result;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
VTDWORD ReadIniClipbOptSSDB(const wchar_t *optionname)
{
	// Spreadsheet Or Database Copy-Paste
	wchar_t buf[INT64STRMAXBUF];
	VTDWORD result;

	GetPrivateProfileStringW(CLIPBOARDSECTION, optionname, ASKIP, buf, INT64STRMAXBUF, inipath);
	if (_wcsicmp(buf, L"rtf") == 0) result = SCCVW_CLIPSUBFORMAT_TABLE;
	else if (_wcsicmp(buf, L"tabs") == 0) result = SCCVW_CLIPSUBFORMAT_TABS;
	else if (_wcsicmp(buf, L"optimizedtabs") == 0) result = SCCVW_CLIPSUBFORMAT_OPTIMIZEDTABS;
	else result = Opt::SKIP;

	return result;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
VTDWORD ReadIniViewOptDisplay(const wchar_t *optionname)
{
	// A.7.2 SCCID_WPDISPLAYMODE / SCCID_HTMLDISPLAYMODE / SCCID_EMAILDISPLAYMODE
	wchar_t buf[INT64STRMAXBUF];
	VTDWORD result;

	GetPrivateProfileStringW(VIEWERSECTION, optionname, ASKIP, buf, INT64STRMAXBUF, inipath);
	if (_wcsicmp(buf, L"draft") == 0) result = SCCVW_WPMODE_DRAFT;
	else if (_wcsicmp(buf, L"normal") == 0) result = SCCVW_WPMODE_NORMAL;
	else if (_wcsicmp(buf, L"preview") == 0) result = SCCVW_WPMODE_PREVIEW;
	else if (_wcsicmp(buf, L"weblayout") == 0) result = SCCVW_WPMODE_WEBLAYOUT;
	else result = Opt::SKIP;

	return result;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void InitClipboardOpts()
{
	VTOptions.VTClipboard.FORMAT_TEXT = ReadIniClipbOpt(L"ascii");
	VTOptions.VTClipboard.FORMAT_RTF = ReadIniClipbOpt(L"rtf");
	VTOptions.VTClipboard.FORMAT_UNICODE = ReadIniClipbOpt(L"unicode");
	VTOptions.VTClipboard.FORMAT_WINBITMAP = ReadIniClipbOpt(L"bitmap");
	VTOptions.VTClipboard.FORMAT_WINDIB = ReadIniClipbOpt(L"windib");
	VTOptions.VTClipboard.FORMAT_WINMETAFILE = ReadIniClipbOpt(L"metafile");
	VTOptions.VTClipboard.FORMAT_WINPALETTE = ReadIniClipbOpt(L"palette");

	VTOptions.VTClipboard.OLE_ENABLEDRAGDROP = ReadIniClipbOpt(L"dragdrop");

	VTOptions.VTClipboard.SSCLIPBOARD = ReadIniClipbOptSSDB(L"spreadsheet");
	VTOptions.VTClipboard.DBCLIPBOARD = ReadIniClipbOptSSDB(L"database");
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void InitViewerOpts()
{
	VTOptions.VTViewer.WP.DisplayMode = ReadIniViewOptDisplay(L"wpdisplaymode");
	VTOptions.VTViewer.HTML.DisplayMode = ReadIniViewOptDisplay(L"htmldisplaymode");
	VTOptions.VTViewer.EMAIL.DisplayMode = ReadIniViewOptDisplay(L"emaildisplaymode");
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void IniParse()
{
	GetIniPath(inipath);
	InitUlister();
	InitClipboardOpts();
	InitViewerOpts();
}
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GetLibPathVT(wchar_t *libpath, const wchar_t *libname, const int ntlev) {
// OUT: build libpath and (!) check if it exists;   true=OK

	GetModuleFileNameW(hInst, libpath, MAX_PATH);

	wchar_t *pathposition;
	if (pathposition = wcsrchr(libpath, L'\\'))
		*pathposition = L'\0';

	if (ntlev == WindowsNTLevel::WinNT5)
		wcscat_s(libpath, MAX_PATH, REDIST_NT5);
	else
		wcscat_s(libpath, MAX_PATH, REDIST_NT6);

	wcscat_s(libpath, MAX_PATH, libname);

	return (GetFileAttributesW(libpath) != INVALID_FILE_ATTRIBUTES);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
HINSTANCE LoadLibVT(const wchar_t *libname) {
	HANDLE lib = NULL;
	wchar_t path[MAX_PATH];

	// if WinXP, first try to load library from "XPdist*"
	if (NTLevel == WindowsNTLevel::WinNT5)
	{
		if (GetLibPathVT(path, libname, WindowsNTLevel::WinNT5))
		{
			lib = LoadLibraryExW(path, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
			if (lib == NULL) // DLL exist, but can't load...
				ErrMsgIssue(FileErrIssue::CantLoad, path, GetLastError());

			return (HINSTANCE)lib;
		}
	}

	// anycase, load library from "redist*"
	if (GetLibPathVT(path, libname, WindowsNTLevel::WinNT6))
	{
		lib = LoadLibraryExW(path, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		if (lib == NULL) // DLL exist, but can't load...
			ErrMsgIssue(FileErrIssue::CantLoad, path, GetLastError());
	}
	else
		ErrMsgIssue(FileErrIssue::CantFind, path, GetLastError());

	return (HINSTANCE)lib;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned long long REGCurrentBuildNumber()
{
	char readbuf[INT64STRMAXBUF];
	DWORD readbytes = INT64STRMAXBUF - 1;

	HKEY hKey = NULL;
	unsigned long long CurrentBuildNumber = 0;
	LONG Stat; // fix VS2005

	Stat = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &hKey);
	if (Stat == ERROR_SUCCESS)
	{
		Stat = RegQueryValueExA(hKey, "CurrentBuildNumber", NULL, NULL, (LPBYTE) readbuf, &readbytes); // Windows XP SP0 support
		if (Stat == ERROR_SUCCESS)
		{
			readbuf[readbytes] = 0; // the return value of RegQueryValueExA may not contain the '\0' character at the end
			CurrentBuildNumber = _strtoui64(readbuf, NULL, 10); // VS2005 fix
		}
	}

	RegCloseKey(hKey);
	return CurrentBuildNumber;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ErrMsgIssue(const int issuetype, const wchar_t *path, const DWORD dwError)
{
#ifdef ULISTER64
	const wchar_t *title = L"uLister x64 Error";
#else
	const wchar_t *title = L"uLister x86 Error";
#endif

	wchar_t *issuename;
	if (issuetype == FileErrIssue::CantFind)
		issuename = L"File not found:";
	else
		issuename = L"Can't load:";

	wchar_t buf[ULISTMAXBUF];
	swprintf_s(buf, ULISTMAXBUF,
		L"%s\n"
		L"[%s].\n"
		L"Error code: 0x%08X   (%lu)\n"
		L"INI=[%s]\n\n"
		L"Check Outside In dlls in plugin dir.\n"
		L"See readme.txt, install section.", issuename, path, dwError, dwError, inipath);

	MessageBoxW(NULL, buf, title, MB_OK);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
clsVTOptionsClipboard::clsVTOptionsClipboard()
{
	FORMAT_TEXT = Opt::SKIP;
	FORMAT_RTF = Opt::SKIP;
	FORMAT_UNICODE = Opt::SKIP;
	FORMAT_WINBITMAP = Opt::SKIP;
	FORMAT_WINDIB = Opt::SKIP;
	FORMAT_WINMETAFILE = Opt::SKIP;
	FORMAT_WINPALETTE = Opt::SKIP;
	OLE_ENABLEDRAGDROP = Opt::SKIP;
	SSCLIPBOARD = Opt::SKIP;
	DBCLIPBOARD = Opt::SKIP;
}

VTDWORD clsVTOptionsClipboard::Get_SCCVW_CLIPSUBFORMAT_SS(VTDWORD ClipSubFormat) const
{
	if (SSCLIPBOARD == Opt::SKIP) return ClipSubFormat;
	else return SSCLIPBOARD;
}
VTDWORD clsVTOptionsClipboard::Get_SCCVW_CLIPSUBFORMAT_DB(VTDWORD ClipSubFormat) const
{
	if (DBCLIPBOARD == Opt::SKIP) return ClipSubFormat;
	else return DBCLIPBOARD;
}
VTDWORD clsVTOptionsClipboard::Get_SCCVW_OLE(VTDWORD OLEFlags) const
{
	if (OLE_ENABLEDRAGDROP == Opt::SKIP) return OLEFlags;
	return (OLE_ENABLEDRAGDROP) ? SCCVW_OLE_ENABLEDRAGDROP : 0;
}

VTDWORD clsVTOptionsClipboard::Get_SCCVW_CLIPFORMAT(VTDWORD ClipFormat) const
{

	/*
	//ClipFormat = 0;
	wchar_t buf[ULISTMAXBUF];
	swprintf_s(buf, ULISTMAXBUF,
		L"SRC ClipFormat: 0x%08X (%u)\n"
		L"FORMAT_TEXT=%i\n"
		L"FORMAT_RTF=%i\n"
		L"FORMAT_UNICODE=%i\n"
		L"FORMAT_WINBITMAP=%i\n"
		L"FORMAT_WINDIB=%i\n"
		L"FORMAT_WINMETAFILE=%i\n"
		L"FORMAT_WINPALETTE=%i\n",
		ClipFormat, ClipFormat,
		FORMAT_TEXT, FORMAT_RTF, FORMAT_UNICODE, FORMAT_WINBITMAP, FORMAT_WINDIB, FORMAT_WINMETAFILE, FORMAT_WINPALETTE);
	MessageBoxW(NULL, buf, L"Get_SCCVW_CLIPFORMAT", MB_OK);
	*/



	// skip/set/reset bits
	if (FORMAT_TEXT != Opt::SKIP) ClipFormat = FORMAT_TEXT ? (ClipFormat | SCCVW_CLIPFORMAT_TEXT) : (ClipFormat & ~SCCVW_CLIPFORMAT_TEXT);
	if (FORMAT_RTF != Opt::SKIP) ClipFormat = FORMAT_RTF ? (ClipFormat | SCCVW_CLIPFORMAT_RTF) : (ClipFormat & ~SCCVW_CLIPFORMAT_RTF);
	if (FORMAT_UNICODE != Opt::SKIP) ClipFormat = FORMAT_UNICODE ? (ClipFormat | SCCVW_CLIPFORMAT_UNICODE) : (ClipFormat & ~SCCVW_CLIPFORMAT_UNICODE);
	if (FORMAT_WINBITMAP != Opt::SKIP) ClipFormat = FORMAT_WINBITMAP ? (ClipFormat | SCCVW_CLIPFORMAT_WINBITMAP) : (ClipFormat & ~SCCVW_CLIPFORMAT_WINBITMAP);
	if (FORMAT_WINDIB != Opt::SKIP) ClipFormat = FORMAT_WINDIB ? (ClipFormat | SCCVW_CLIPFORMAT_WINDIB) : (ClipFormat & ~SCCVW_CLIPFORMAT_WINDIB);
	if (FORMAT_WINMETAFILE != Opt::SKIP) ClipFormat = FORMAT_WINMETAFILE ? (ClipFormat | SCCVW_CLIPFORMAT_WINMETAFILE) : (ClipFormat & ~SCCVW_CLIPFORMAT_WINMETAFILE);
	if (FORMAT_WINPALETTE != Opt::SKIP) ClipFormat = FORMAT_WINPALETTE ? (ClipFormat | SCCVW_CLIPFORMAT_WINPALETTE) : (ClipFormat & ~SCCVW_CLIPFORMAT_WINPALETTE);



	/*
	swprintf_s(buf, ULISTMAXBUF,
		L"DST ClipFormat: 0x%08X (%u)\n",
		ClipFormat, ClipFormat);
	MessageBoxW(NULL, buf, L"Get_SCCVW_CLIPFORMAT", MB_OK);
	*/

	return ClipFormat;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
clsVTDisplayMode::clsVTDisplayMode()
{
	DisplayMode = Opt::SKIP;
}
VTDWORD clsVTDisplayMode::Get_SCCVW_DISPLAYMODE(VTDWORD Mode) const
{
	if (DisplayMode == Opt::SKIP) return Mode;
	else return DisplayMode;
}








///////////////////////////////////////////////////////////////////////////////////////////////////
void SendVTOptions(const ALLMYDATA *mydata, const clsVTOptions *_VTOptions)
{
	SCCVWOPTIONSPEC40 locOptionSpec;
	locOptionSpec.dwSize = sizeof(SCCVWOPTIONSPEC40);
	locOptionSpec.dwFlags = SCCVWOPTION_CURRENT;

	union
	{
		VTDWORD ClipFormat;
		VTDWORD OLEFlags;
		VTDWORD SpreadsheetClipboard;
		VTDWORD DatabaseClipboard;
		VTDWORD WPdisplaymode;
		VTDWORD HTMLdisplaymode;
		VTDWORD EMAILdisplaymode;
	};

	// unicode clipboard:
	locOptionSpec.dwId = SCCID_TOCLIPBOARD;
	locOptionSpec.pData = &ClipFormat;
	SendMessage(mydata->oiWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	ClipFormat = _VTOptions->VTClipboard.Get_SCCVW_CLIPFORMAT(ClipFormat);
	SendMessage(mydata->oiWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// drag-and-drop copying:
	locOptionSpec.dwId = SCCID_OLEFLAGS;
	//locOptionSpec.pData = &OLEFlags;
	SendMessage(mydata->oiWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	OLEFlags = _VTOptions->VTClipboard.Get_SCCVW_OLE(OLEFlags);
	SendMessage(mydata->oiWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// spreadsheet copying:
	locOptionSpec.dwId = SCCID_SSCLIPBOARD;
	//locOptionSpec.pData = &SpreadsheetClipboard;
	SendMessage(mydata->oiWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	SpreadsheetClipboard = _VTOptions->VTClipboard.Get_SCCVW_CLIPSUBFORMAT_SS(SpreadsheetClipboard);
	SendMessage(mydata->oiWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// database copying:
	locOptionSpec.dwId = SCCID_DBCLIPBOARD;
	//locOptionSpec.pData = &DatabaseClipboard;
	SendMessage(mydata->oiWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	DatabaseClipboard = _VTOptions->VTClipboard.Get_SCCVW_CLIPSUBFORMAT_DB(DatabaseClipboard);
	SendMessage(mydata->oiWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);








	// word processor display engine:
	locOptionSpec.dwId = SCCID_WPDISPLAYMODE;
	//locOptionSpec.pData = &WPdisplaymode;
	SendMessage(mydata->oiWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	WPdisplaymode = _VTOptions->VTViewer.WP.Get_SCCVW_DISPLAYMODE(WPdisplaymode);
	SendMessage(mydata->oiWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// HTML display engine:
	locOptionSpec.dwId = SCCID_HTMLDISPLAYMODE;
	//locOptionSpec.pData = &HTMLdisplaymode;
	SendMessage(mydata->oiWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	HTMLdisplaymode = _VTOptions->VTViewer.HTML.Get_SCCVW_DISPLAYMODE(HTMLdisplaymode);
	SendMessage(mydata->oiWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// email display engine:
	locOptionSpec.dwId = SCCID_EMAILDISPLAYMODE;
	//locOptionSpec.pData = &EMAILdisplaymode;
	SendMessage(mydata->oiWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	EMAILdisplaymode = _VTOptions->VTViewer.EMAIL.Get_SCCVW_DISPLAYMODE(EMAILdisplaymode);
	SendMessage(mydata->oiWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
}
