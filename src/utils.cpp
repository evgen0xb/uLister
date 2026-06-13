/* 
The plugin is provided as-is and without any warranty under the GPLv3 license.
*/

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include "ulister.h"

extern clsUlisterInstance	UlisterInstance;
extern clsUlisterOptions	UlisterOptions;
extern clsVTOptions			VTOptions;

#ifdef ULISTER64
wchar_t *REDIST_NT6 = L"\\redist64\\";
wchar_t *REDIST_NT5 = L"\\XPdist64\\";
#else
wchar_t *REDIST_NT6 = L"\\redist32\\";
wchar_t *REDIST_NT5 = L"\\XPdist32\\";
#endif

#ifdef ULISTER64
const wchar_t *formatsfilename = L"\\formats64.txt";
#else
const wchar_t *formatsfilename = L"\\formats32.txt";
#endif

const wchar_t ULISTERINI[]		= L"\\ulister.ini"; // ! MUST BE ARRAY !
const wchar_t *CLIPBOARDSECTION = L"clipboard";
const wchar_t *VIEWERSECTION	= L"viewer";
const wchar_t *ASKIP			= L"SKIP";
const wchar_t *AON				= L"ON";
const wchar_t *AOFF				= L"OFF";

const char *AFIINIT				= "FIInit";
const char *AFIDEINIT			= "FIDeInit";
const char *AFIGETFIRSTID		= "FIGetFirstId";
const char *AFIGETNEXTID		= "FIGetNextId";
const char *AFIIDFILEEX			= "FIIdFileEx";

extern const char *WNDCLASSNAME_SCCVIEWER;



///////////////////////////////////////////////////////////////////////////////////////////////////
VTWORD GetVTFileType(const wchar_t* FileToLoad)
{
	typedef VTDWORD(*FIInitFUNC)(VTVOID);
	typedef VTWORD(*FIIdFileExFUNC)(VTDWORD, const VTVOID *, VTDWORD, VTWORD *, VTLPTSTR, VTWORD);
	typedef VTDWORD(*FIDeInitFUNC)(VTVOID);

	FIInitFUNC FIInit;
	FIIdFileExFUNC FIIdFileEx;
	FIDeInitFUNC FIDeInit;

	VTWORD  wVTFileType = FI_UNKNOWN;
	SCCERR  FIErrorCode = SCCERR_UNKNOWN;

	HINSTANCE hInstFileIdentDLL = UlisterInstance.FileIdentInstanceInc();

	if (hInstFileIdentDLL)
	{
		FIInit = (FIInitFUNC)GetProcAddress(hInstFileIdentDLL, AFIINIT);
		FIIdFileEx = (FIIdFileExFUNC)GetProcAddress(hInstFileIdentDLL, AFIIDFILEEX);
		FIDeInit = (FIDeInitFUNC)GetProcAddress(hInstFileIdentDLL, AFIDEINIT);

		if (FIInit && FIIdFileEx && FIDeInit)
		{
			FIInit();
			VTDWORD dwFlags = FIFLAG_NORMAL;
			char pTypeName[VTMAXTYPENAMEBUF];
			FIErrorCode = FIIdFileEx(IOTYPE_UNICODEPATH, FileToLoad, dwFlags, &wVTFileType, (VTLPTSTR)pTypeName, VTMAXTYPENAMEBUF);
			FIDeInit();
		}
	}

	UlisterInstance.FileIdentInstanceDec(UlisterOptions.keepinmemory);

	return 	(FIErrorCode == SCCERR_OK) ? wVTFileType : FI_UNKNOWN;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
void CreatFormatsTxt(const wchar_t* path)
{
	typedef VTDWORD(*FIInitFUNC)(VTVOID);
	typedef VTDWORD(*FIDeInitFUNC)(VTVOID);
	typedef VTWORD(*FIGetFirstIdFUNC)(PFIGET pFiGet, VTWORD * pType, VTLPTSTR pTypeName, VTWORD wNameCount);
	typedef VTWORD(*FIGetNextIdFUNC)(PFIGET pFiGet, VTWORD * pType, VTLPTSTR pTypeName, VTWORD wNameCount);

	FIInitFUNC FIInit;
	FIDeInitFUNC FIDeInit;
	FIGetFirstIdFUNC FIGetFirstId;
	FIGetNextIdFUNC FIGetNextId;

	HINSTANCE hInstFileIdentDLL = UlisterInstance.FileIdentInstanceInc();

	if (hInstFileIdentDLL)
	{
		FIInit = (FIInitFUNC)GetProcAddress(hInstFileIdentDLL, AFIINIT);
		FIDeInit = (FIDeInitFUNC)GetProcAddress(hInstFileIdentDLL, AFIDEINIT);
		FIGetFirstId = (FIGetFirstIdFUNC)GetProcAddress(hInstFileIdentDLL, AFIGETFIRSTID);
		FIGetNextId = (FIGetFirstIdFUNC)GetProcAddress(hInstFileIdentDLL, AFIGETNEXTID);

		if (FIInit && FIDeInit && FIGetFirstId && FIGetNextId)
		{
			HANDLE hFile = CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				FIGET figetTag;
				VTBOOL MoreIDs;
				char buf[ULISTMAXBUF];
				DWORD bytesWritten;

				VTWORD TotalIDs = 0;
				VTWORD TypeNumber;
				char TypeName[VTMAXTYPENAMEBUF];

				FIInit();
				MoreIDs = FIGetFirstId(&figetTag, &TypeNumber, TypeName, VTMAXTYPENAMEBUF);
				while (MoreIDs)
				{
					TotalIDs++;
					_snprintf_s(buf, ULISTMAXBUF, _TRUNCATE, "%u  -  %s\n", TypeNumber, TypeName);
					//OutputDebugStringA(buf);
					WriteFile(hFile, buf, (DWORD)strlen(buf), &bytesWritten, NULL);
					MoreIDs = FIGetNextId(&figetTag, &TypeNumber, TypeName, VTMAXTYPENAMEBUF);
				}
				FIDeInit();
				_snprintf_s(buf, ULISTMAXBUF, _TRUNCATE, "\nTotal format types: %u\n", TotalIDs);
				//OutputDebugStringA(buf);
				WriteFile(hFile, buf, (DWORD)strlen(buf), &bytesWritten, NULL);

				CloseHandle(hFile);
			}
		}
	}

	UlisterInstance.FileIdentInstanceDec(UlisterOptions.keepinmemory);
}

void RequestFormatsTxt()
{
	wchar_t formatspath[MAX_PATH];
	wchar_t *pathposition;
	GetModuleFileNameW(UlisterInstance.hInstWLX, formatspath, MAX_PATH);
	formatspath[MAX_PATH - 1] = L'\0'; // Windows XP fix: The string is truncated to nSize characters and is not null-terminated
	if (pathposition = wcsrchr(formatspath, L'\\')) *pathposition = L'\0';
	wcscat_s(formatspath, MAX_PATH, formatsfilename);

	if (GetFileAttributesW(formatspath) == INVALID_FILE_ATTRIBUTES) CreatFormatsTxt(formatspath);
}



///////////////////////////////////////////////////////////////////////////////////////////////////
bool IsVTFileTypeAllowed(const wchar_t* FileToLoad, const wchar_t* onlyload, const wchar_t* noload)
{
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
bool LoadVTFile(const HWND hViewWnd, const wchar_t* FileToLoad)
{
	// TRUE - file load OK
	// FALSE - the file could not be loaded
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
	return (SendMessage(hViewWnd, SCCVW_VIEWFILE, 0, (LPARAM)&locViewFile) == SCCVWERR_OK);
}



///////////////////////////////////////////////////////////////////////////////////////////////////

// This message is sent from the view window to the developer when another file should be viewed.
// Currently, this occurs is when the user double - clicks or hits return on a file entry in an Archive view and on a hyperlink to a referenced document.
// In the case of the archive formats, the display engine decompresses the file then sends a SCCVW_VIEWTHISFILE message to the developer.
DWORD ViewThisFileHandler(const LPARAM lParam) // 4.74 SCCVW_VIEWTHISFILE
#define ULISTMAXARGS (MAX_PATH * 2 + 15) // [/S=L "" /I=""] + ini_path + target_path + '\0'
{
	union
	{
		PSCCVWVIEWTHISFILE40    locVTFPtr40;
		PSCCVWVIEWTHISFILE80    locVTFPtr80;
	}; locVTFPtr40 = (PSCCVWVIEWTHISFILE40)lParam; // locVTFPtr80 = (PSCCVWVIEWTHISFILE80)lParam;

	if (locVTFPtr40->sViewFile.dwSpecType == IOTYPE_ANSIPATH)
	{
		char TotalcmdExePath[MAX_PATH];
		char TotalcmdIniPath[MAX_PATH];
		char TotalArgs[ULISTMAXARGS];

		if (!GetModuleFileNameA(NULL, TotalcmdExePath, MAX_PATH)) return ERROR_EXE_MARKED_INVALID;
		TotalcmdExePath[MAX_PATH - 1] = '\0'; // Windows XP fix: The string is truncated to nSize characters and is not null-terminated
		if (!GetEnvironmentVariableA("COMMANDER_INI", TotalcmdIniPath, MAX_PATH)) return GetLastError();

		TotalArgs[0] = '\0';
		strcat_s(TotalArgs, ULISTMAXARGS, "/S=L \""); // switch to run TC as lister mode
		strcat_s(TotalArgs, ULISTMAXARGS, (char *)locVTFPtr40->sViewFile.pSpec); // path to the file to view
		strcat_s(TotalArgs, ULISTMAXARGS, "\" /I=\"");
		strcat_s(TotalArgs, ULISTMAXARGS, TotalcmdIniPath); // current TC ini file
		strcat_s(TotalArgs, ULISTMAXARGS, "\"");

		ShellExecuteA(NULL, "open", TotalcmdExePath, TotalArgs, NULL, SW_RESTORE);
	}
	else if (locVTFPtr80->sViewFile.dwSpecType == IOTYPE_UNICODEPATH)
	{
		wchar_t TotalcmdExePath[MAX_PATH];
		wchar_t TotalcmdIniPath[MAX_PATH];
		wchar_t TotalArgs[ULISTMAXARGS];

		if (!GetModuleFileNameW(NULL, TotalcmdExePath, MAX_PATH)) return ERROR_EXE_MARKED_INVALID;
		TotalcmdExePath[MAX_PATH - 1] = L'\0'; // Windows XP fix: The string is truncated to nSize characters and is not null-terminated
		if (!GetEnvironmentVariableW(L"COMMANDER_INI", TotalcmdIniPath, MAX_PATH)) return GetLastError();

		TotalArgs[0] = L'\0';
		wcscat_s(TotalArgs, ULISTMAXARGS, L"/S=L \""); // switch to run TC as lister mode
		wcscat_s(TotalArgs, ULISTMAXARGS, (wchar_t *)locVTFPtr80->sViewFile.pSpec); // path to the file to view
		wcscat_s(TotalArgs, ULISTMAXARGS, L"\" /I=\"");
		wcscat_s(TotalArgs, ULISTMAXARGS, TotalcmdIniPath); // current TC ini file
		wcscat_s(TotalArgs, ULISTMAXARGS, L"\"");

		ShellExecuteW(NULL, L"open", TotalcmdExePath, TotalArgs, NULL, SW_RESTORE);
	}
	else return ERROR_BAD_LENGTH;

	return GetLastError();
}



///////////////////////////////////////////////////////////////////////////////////////////////////
HBITMAP GetVTFilePreview(const wchar_t* FileToLoad, const int width, const int height)
{
	if (!UlisterInstance.ViewerLibraryInstanceInc()) return NULL;

	HWND hViewWnd = CreateWindow(WNDCLASSNAME_SCCVIEWER, NULL, WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, 0, UlisterInstance.hInstWLX, NULL);
	if (!IsWindow(hViewWnd))
	{
		UlisterInstance.ViewerLibraryInstanceDec(UlisterOptions.keepinmemory);
		return NULL;
	}

	// TC SDK:
	// Return a handle to your window if load succeeds, NULL otherwise. If NULL is returned, Lister will try the next plugin.

	if (!LoadVTFile(hViewWnd, FileToLoad))
	{
		DestroyWindow(hViewWnd);
		UlisterInstance.ViewerLibraryInstanceDec(UlisterOptions.keepinmemory);
		return NULL;
	}

	HDC OutputDC, FormatDC;
	HBITMAP bitmap, oldbitmap;
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

	UlisterInstance.ViewerLibraryInstanceDec(UlisterOptions.keepinmemory);

	return bitmap;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef SMARTINIPATH
void CreateDefaultUlisterIni(wchar_t *_inipath)
{
	DWORD bytesWritten;
	const char defaultconfig[] = // ! MUST BE ARRAY !
"[ulister]\n"
"noloadtypes=1999\n"
"nopreviewtypes=1999\n"
"keepinmemory=1\n"
"\n"
"; optionsdir=%COMMANDER_PATH%\\Plugins\\wlx\\ulister\n"
"\n"
"; Ctrl-Shift-F1 - Thumbnails view\n"
"; Qtrl-Q - Quick View\n"
"; F3 - View\n"
"; Shift-F3 - multiple selected files viewing session\n"
"\n"
"; invert mouse wheel horizontal scroll\n"
"mwhscrollinvert=on\n"
"\n"
"; tooltip display time in milliseconds\n"
"tooltipsdelayms=3000\n"
"; tooltip transparency (0...255)\n"
"tooltipstransparency=244\n"
"\n"
"[clipboard]\n"
"; This options controls the clipboard formats that the viewer attempts to place on the clipboard\n"
"; skip|on|off\n"
"\n"
"ascii=on\n"
"rtf=on\n"
"\n"
"; turn it on!\n"
"unicode=on\n"
"\n"
"bitmap=on\n"
"windib=on\n"
"metafile=on\n"
"palette=on\n"
"\n"
"dragdrop=on\n"
"\n"
"; controls the format the spreadsheet or the database data takes when copied to the clipboard\n"
"; skip|rtf|tabs|optimizedtabs\n"
"spreadsheet=rtf\n"
"database=rtf\n"
"\n"
"[viewer]\n"
"; Display engine\n"
"; skip|draft|normal|preview|weblayout\n"
"wpdisplaymode=normal\n"
"htmldisplaymode=weblayout\n"
"emaildisplaymode=normal\n"
"\n"
"; skip|draft|normal|normalhidden\n"
"spreadsheetdisplaymode=normal\n"
"\n"
"; Extended for preview or weblayout mode of previous options.\n"
"; These settings are applied correctly only from the second launch of ulister after\n"
"; changes in the .ini file due to an bug in the Outside In Viewer library (workaround):\n"
"; skip|original|width|window\n"
"webprevwpfitmode=width\n"
"webprevhtmlfitmode=width\n"
"webprevemailfitmode=width\n"
"\n"
"; For bitmap and vector graphics\n"
"; skip|best|original|window|height|width|stretch|imagesize\n"
"vectorfitmode=window\n"
"bitmapfitmode=window\n"
;
	const DWORD cfglength = STRLEN(defaultconfig)-1;

	HANDLE hFile = CreateFileW(_inipath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return;
	WriteFile(hFile, defaultconfig, cfglength, &bytesWritten, NULL);
	CloseHandle(hFile);
}
#endif

void GetIniPathWLX(wchar_t *_inipath)
{
	wchar_t *pathposition;
	GetModuleFileNameW(UlisterInstance.hInstWLX, _inipath, MAX_PATH);
	_inipath[MAX_PATH - 1] = L'\0'; // Windows XP fix: The string is truncated to nSize characters and is not null-terminated
	if (pathposition = wcsrchr(_inipath, L'\\')) *pathposition = L'\0';
	wcscat_s(_inipath, MAX_PATH, ULISTERINI);
}

void GetIniPathCOMMANDER(wchar_t *_inipath)
{
	wchar_t *pathposition;
	DWORD retlength = GetEnvironmentVariableW(L"COMMANDER_INI", _inipath, MAX_PATH);
	if ((retlength < MAX_PATH) && retlength)
	{
		if (pathposition = wcsrchr(_inipath, L'\\')) *pathposition = L'\0';
		wcscat_s(_inipath, MAX_PATH, ULISTERINI);
	}
	else _inipath[0] = L'\0';
}

void GetIniPathAPPDATA(wchar_t *_inipath)
{
	DWORD retlength = GetEnvironmentVariableW(L"APPDATA", _inipath, MAX_PATH);
	if ((retlength + STRLEN(ULISTERINI)-1 < MAX_PATH) && retlength) wcscat_s(_inipath, MAX_PATH, ULISTERINI);
	else _inipath[0] = L'\0';
}

bool GetIniPath(wchar_t *_inipath)
// true  - ulister.ini was found
// false - ulister.ini does not exist
{
	bool isexist = false;

	// highest priority is to place ulister.ini in the plugin directory
	GetIniPathWLX(_inipath);

	if (GetFileAttributesW(_inipath) == INVALID_FILE_ATTRIBUTES)
	{
		// otherwise, look for ulister.ini in the same place as wincmd.ini
		GetIniPathCOMMANDER(_inipath);

		if (GetFileAttributesW(_inipath) == INVALID_FILE_ATTRIBUTES)
		{
			// lowest priority ulister.ini placed in %APPDATA%
			GetIniPathAPPDATA(_inipath);
			if (GetFileAttributesW(_inipath) != INVALID_FILE_ATTRIBUTES) isexist = true;
		}
		else isexist = true;
	}
	else isexist = true;

#ifdef SMARTINIPATH
	// last chance:
	// try to create default ulister.ini in the plugin directory
	if (!isexist)
	{
		GetIniPathWLX(_inipath);
		CreateDefaultUlisterIni(_inipath);
		if (GetFileAttributesW(_inipath) != INVALID_FILE_ATTRIBUTES) isexist = true;
	}
#endif

	return isexist;
}
///////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////
void InitUlister()
{
	const wchar_t *ULISTERSECTION = L"uLister";
	const wchar_t *OIT_DATA_PATH = L"OIT_DATA_PATH";

	wchar_t inioptdir[MAX_PATH];
	wchar_t oitdatapath[MAX_PATH];

	wchar_t buf[INT64STRMAXBUF];

	GetPrivateProfileStringW(ULISTERSECTION, L"keepinmemory", L"1", buf, INT64STRMAXBUF, UlisterOptions.inipath);
	if (_wcsicmp(buf, L"1") == 0) UlisterOptions.keepinmemory = true; else UlisterOptions.keepinmemory = false;

	GetPrivateProfileStringW(ULISTERSECTION, L"mwhscrollinvert", AON, buf, INT64STRMAXBUF, UlisterOptions.inipath);
	if (_wcsicmp(buf, AON) == 0) UlisterOptions.mwhscrollinvert = true; else UlisterOptions.mwhscrollinvert = false;

	GetPrivateProfileStringW(ULISTERSECTION, L"optionsdir", L"", inioptdir, MAX_PATH, UlisterOptions.inipath);

	GetPrivateProfileStringW(ULISTERSECTION, L"noloadtypes", L"", UlisterOptions.ininoloadtypes, ULISTMAXBUF, UlisterOptions.inipath);
	GetPrivateProfileStringW(ULISTERSECTION, L"onlyloadtypes", L"", UlisterOptions.inionlyloadtypes, ULISTMAXBUF, UlisterOptions.inipath);
	GetPrivateProfileStringW(ULISTERSECTION, L"nopreviewtypes", L"", UlisterOptions.ininopreviewtypes, ULISTMAXBUF, UlisterOptions.inipath);
	GetPrivateProfileStringW(ULISTERSECTION, L"onlypreviewtypes", L"", UlisterOptions.inionlypreviewtypes, ULISTMAXBUF, UlisterOptions.inipath);

	if (wcslen(inioptdir) > 0) {
		ExpandEnvironmentStringsW(inioptdir, oitdatapath, MAX_PATH);
		SetEnvironmentVariableW(OIT_DATA_PATH, oitdatapath);
	}

	GetPrivateProfileStringW(ULISTERSECTION, L"tooltipsdelayms", L"3000", buf, INT64STRMAXBUF, UlisterOptions.inipath);
	UlisterOptions.BalloonTipTimer = (UINT)wcstol(buf, NULL, 10);
	GetPrivateProfileStringW(ULISTERSECTION, L"tooltipstransparency", L"244", buf, INT64STRMAXBUF, UlisterOptions.inipath);
	UlisterOptions.BalloonTransparency = (UINT)wcstol(buf, NULL, 10);
}



///////////////////////////////////////////////////////////////////////////////////////////////////
__int8 ReadIniClipbOpt(const wchar_t *optionname)
{
	wchar_t buf[INT64STRMAXBUF];
	__int8 result;

	GetPrivateProfileStringW(CLIPBOARDSECTION, optionname, ASKIP, buf, INT64STRMAXBUF, UlisterOptions.inipath);
	if (_wcsicmp(buf, AON) == 0) result = Opt::ON;
	else if (_wcsicmp(buf, AOFF) == 0) result = Opt::OFF;
	else result = Opt::SKIP;

	return result;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
VTDWORD ReadIniClipbSubFormat(const wchar_t *optionname)
{
	// Spreadsheet Or Database Copy-Paste
	wchar_t buf[INT64STRMAXBUF];
	VTDWORD result;

	GetPrivateProfileStringW(CLIPBOARDSECTION, optionname, ASKIP, buf, INT64STRMAXBUF, UlisterOptions.inipath);
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

	GetPrivateProfileStringW(VIEWERSECTION, optionname, ASKIP, buf, INT64STRMAXBUF, UlisterOptions.inipath);
	if (_wcsicmp(buf, L"draft") == 0) result = SCCVW_WPMODE_DRAFT;
	else if (_wcsicmp(buf, L"normal") == 0) result = SCCVW_WPMODE_NORMAL;
	else if (_wcsicmp(buf, L"preview") == 0) result = SCCVW_WPMODE_PREVIEW;
	else if (_wcsicmp(buf, L"weblayout") == 0) result = SCCVW_WPMODE_WEBLAYOUT;
	else result = Opt::SKIP;

	return result;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
VTDWORD ReadIniViewOptWebPrevFitMode(const wchar_t *optionname)
{
	// A.7.3 SCCID_WPFITMODE / SCCID_HTMLFITMODE / SCCID_EMAILFITMODE
	wchar_t buf[INT64STRMAXBUF];
	VTDWORD result;

	GetPrivateProfileStringW(VIEWERSECTION, optionname, ASKIP, buf, INT64STRMAXBUF, UlisterOptions.inipath);
	if (_wcsicmp(buf, L"original") == 0) result = SCCVW_FITMODE_ORIGINAL;
	else if (_wcsicmp(buf, L"width") == 0) result = SCCVW_FITMODE_WINDOWWIDTH;
	else if (_wcsicmp(buf, L"window") == 0) result = SCCVW_FITMODE_WINDOW;
	else result = Opt::SKIP;

	return result;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
VTDWORD ReadIniViewOptGraphicFitMode(const wchar_t *optionname)
{
	// A.5.11 SCCID_VECFITMODE, A.5.4 SCCID_BMPFITMODE
	wchar_t buf[INT64STRMAXBUF];
	VTDWORD result;

	GetPrivateProfileStringW(VIEWERSECTION, optionname, ASKIP, buf, INT64STRMAXBUF, UlisterOptions.inipath);
	if (_wcsicmp(buf, L"best") == 0) result = SCCVW_FITMODE_BEST;
	else if (_wcsicmp(buf, L"original") == 0) result = SCCVW_FITMODE_ORIGINAL;
	else if (_wcsicmp(buf, L"window") == 0) result = SCCVW_FITMODE_WINDOW;
	else if (_wcsicmp(buf, L"height") == 0) result = SCCVW_FITMODE_WINDOWHEIGHT;
	else if (_wcsicmp(buf, L"width") == 0) result = SCCVW_FITMODE_WINDOWWIDTH;
	else if (_wcsicmp(buf, L"stretch") == 0) result = SCCVW_FITMODE_STRETCHWINDOW;
	else if (_wcsicmp(buf, L"imagesize") == 0) result = SCCVW_FITMODE_IMAGESIZE;
	else result = Opt::SKIP;

	return result;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
__int8 ReadIniViewOptSpreadsheetDisplayMode(const wchar_t *optionname)
{
	wchar_t buf[INT64STRMAXBUF];
	__int8 result;

	GetPrivateProfileStringW(VIEWERSECTION, optionname, ASKIP, buf, INT64STRMAXBUF, UlisterOptions.inipath);
	if (_wcsicmp(buf, L"draft") == 0) result = UlisterSSDisplayMode::DRAFT;
	else if (_wcsicmp(buf, L"normal") == 0) result = UlisterSSDisplayMode::NORMAL;
	else if (_wcsicmp(buf, L"normalhidden") == 0) result = UlisterSSDisplayMode::NORMALHIDDEN;
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

	VTOptions.VTClipboard.SSCLIPBOARDSUBFORMAT.Option = ReadIniClipbSubFormat(L"spreadsheet");
	VTOptions.VTClipboard.DBCLIPBOARDSUBFORMAT.Option = ReadIniClipbSubFormat(L"database");
}



///////////////////////////////////////////////////////////////////////////////////////////////////
void InitViewerOpts()
{
	VTOptions.VTViewer.WPDISPLAYMODE.Option = ReadIniViewOptDisplay(L"wpdisplaymode");
	VTOptions.VTViewer.HTMLDISPLAYMODE.Option = ReadIniViewOptDisplay(L"htmldisplaymode");
	VTOptions.VTViewer.EMAILDISPLAYMODE.Option = ReadIniViewOptDisplay(L"emaildisplaymode");

	VTOptions.VTViewer.WEBPREVWPFITMODE.Option = ReadIniViewOptWebPrevFitMode(L"webprevwpfitmode");
	VTOptions.VTViewer.WEBPREVHTMLFITMODE.Option = ReadIniViewOptWebPrevFitMode(L"webprevhtmlfitmode");
	VTOptions.VTViewer.WEBPREVEMAILFITMODE.Option = ReadIniViewOptWebPrevFitMode(L"webprevemailfitmode");

	VTOptions.VTViewer.VECTORFITMODE.Option = ReadIniViewOptGraphicFitMode(L"vectorfitmode");
	VTOptions.VTViewer.BITMAPFITMODE.Option = ReadIniViewOptGraphicFitMode(L"bitmapfitmode");

	VTOptions.VTViewer.SPREADSHEETDISPLAYMODE.Option = ReadIniViewOptSpreadsheetDisplayMode(L"spreadsheetdisplaymode");
}



///////////////////////////////////////////////////////////////////////////////////////////////////
void IniParse()
{
	GetIniPath(UlisterOptions.inipath);
	InitUlister();
	InitClipboardOpts();
	InitViewerOpts();

	RequestFormatsTxt();
}



///////////////////////////////////////////////////////////////////////////////////////////////////
bool GetLibPathVT(wchar_t *libpath, const wchar_t *libname, const int ntlev)
{
// OUT: build libpath and (!) check if it exists;   true=OK

	GetModuleFileNameW(UlisterInstance.hInstWLX, libpath, MAX_PATH);

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
HINSTANCE LoadLibVT(const wchar_t *libname)
{
	HINSTANCE lib = NULL;
	wchar_t path[MAX_PATH];

	// if WinXP, first try to load library from "XPdist*"
	if (UlisterInstance.NTLevel == WindowsNTLevel::WinNT5)
	{
		if (GetLibPathVT(path, libname, WindowsNTLevel::WinNT5))
		{
			lib = LoadLibraryExW(path, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
			if (lib == NULL) // DLL exist, but can't load...
				ErrMsgIssue(FileErrIssue::CantLoad, path, GetLastError());

			return lib;
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

	return lib;
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
		L"1. Check Outside In dlls in plugin dir.\n"
		L"2. Install the required version of Microsoft Visual C++ Redistributable.\n"
		L"See readme.txt, install section.", issuename, path, dwError, dwError, UlisterOptions.inipath);

	MessageBoxW(NULL, buf, title, MB_OK);
}



///////////////////////////////////////////////////////////////////////////////////////////////////

ALLMYDATA::ALLMYDATA() : BalloonTip(BALLOONTIP_TIMER_MSG)
{
	TListerWindow = NULL;
	OriginalTListerWindowProc = NULL;

	waWindow = NULL;

	OriginalSccviewerWindowProc = NULL;
	SccviewerWindow = NULL;

	OriginalSccdisplayWindowProc = NULL;
	SccdisplayWindow = NULL;
}



///////////////////////////////////////////////////////////////////////////////////////////////////

void clsUlisterInstance::Init(const HINSTANCE _hInst)
{
	hInstWLX = _hInst;

	hViewerLibrary = NULL;
	NumInstancesViewLib = 0;

	hFileIdentLibrary = NULL;
	NumInstancesFileIdentLib = 0;

	if (REGCurrentBuildNumber() < WINDOWS7BETABUILDNUMBER)
		NTLevel = WindowsNTLevel::WinNT5;
	else
		NTLevel = WindowsNTLevel::WinNT6;
}

clsUlisterInstance::~clsUlisterInstance()
{
	if (hViewerLibrary) FreeLibrary(hViewerLibrary);
	if (hFileIdentLibrary) FreeLibrary(hFileIdentLibrary);
}

void clsUlisterInstance::ViewerLibraryInstanceDec(bool _keepinmemory)
{
	// unload the "SCCVW.DLL" if needed

#if defined (__ULISTDEBUGMSG) && defined(__ULISTDEBUGKEEPINMEMORY)
	std::wstring msgW = L"Viewer Library Instance Before Decrement (SCCVW.DLL) Num=" + ToStrW(NumInstancesViewLib) + L", keepinmemory=" + ToStrW(_keepinmemory);
	OutputDebugStringW(msgW.c_str());
#endif

	if (NumInstancesViewLib > 0) NumInstancesViewLib--; else return;

	if ((hViewerLibrary != NULL) && !_keepinmemory && (NumInstancesViewLib == 0))
	{
		FreeLibrary(hViewerLibrary);
		hViewerLibrary = NULL;
#if defined (__ULISTDEBUGMSG) && defined(__ULISTDEBUGKEEPINMEMORY)
		OutputDebugStringW(L"FreeLibrary (SCCVW.DLL)");
#endif
	}
}

HINSTANCE clsUlisterInstance::ViewerLibraryInstanceInc()
{
	// load the "SCCVW.DLL" if needed

	if (!hViewerLibrary) hViewerLibrary = LoadLibVT(L"SCCVW.DLL");
	if (!hViewerLibrary) return NULL;

	// When a developer uses the Windows LoadLibrary call to load sccvw.dll,
	// the DLL registers a Window Class named SCCVIEWER.
	// The developer can then create windows of this class.

	NumInstancesViewLib++;
#if defined (__ULISTDEBUGMSG) && defined(__ULISTDEBUGKEEPINMEMORY)
	std::wstring msgW = L"Viewer Library Instance After Increment (SCCVW.DLL) Num=" + ToStrW(NumInstancesViewLib);
	OutputDebugStringW(msgW.c_str());
#endif
	return hViewerLibrary;
}

void clsUlisterInstance::FileIdentInstanceDec(bool _keepinmemory)
{
	// unload the "SCCFI.DLL" if needed

#if defined (__ULISTDEBUGMSG) && defined(__ULISTDEBUGKEEPINMEMORY)
	std::wstring msgW = L"File Identification Instance Before Decrement (SCCFI.DLL) Num=" + ToStrW(NumInstancesFileIdentLib) + L", keepinmemory=" + ToStrW(_keepinmemory);
	OutputDebugStringW(msgW.c_str());
#endif

	if (NumInstancesFileIdentLib > 0) NumInstancesFileIdentLib--; else return;

	if ((hFileIdentLibrary != NULL) && !_keepinmemory && (NumInstancesFileIdentLib == 0))
	{
		FreeLibrary(hFileIdentLibrary);
		hFileIdentLibrary = NULL;
#if defined (__ULISTDEBUGMSG) && defined(__ULISTDEBUGKEEPINMEMORY)
		OutputDebugStringW(L"FreeLibrary (SCCFI.DLL)");
#endif
	}
}

HINSTANCE clsUlisterInstance::FileIdentInstanceInc()
{
	// load the "SCCFI.DLL" if needed

	if (!hFileIdentLibrary) hFileIdentLibrary = LoadLibVT(L"SCCFI.DLL");
	if (!hFileIdentLibrary) return NULL;

	NumInstancesFileIdentLib++;
#if defined (__ULISTDEBUGMSG) && defined(__ULISTDEBUGKEEPINMEMORY)
	std::wstring msgW = L"File Identification Instance After Increment (SCCFI.DLL) Num=" + ToStrW(NumInstancesFileIdentLib);
	OutputDebugStringW(msgW.c_str());
#endif
	return hFileIdentLibrary;
}



///////////////////////////////////////////////////////////////////////////////////////////////////

clsSSViewModeOption::clsSSViewModeOption() { Option = UlisterSSDisplayMode::SKIP; }

VTBOOL clsSSViewModeOption::FilterSkipHiddenCells(VTBOOL val) const
{
	if (Option == UlisterSSDisplayMode::SKIP) return val;
	return (Option == UlisterSSDisplayMode::NORMALHIDDEN) ? TRUE : FALSE;
}

VTBOOL clsSSViewModeOption::FilterSkipDraft(VTBOOL val) const
{
	if (Option == UlisterSSDisplayMode::SKIP) return val;
	return (Option == UlisterSSDisplayMode::DRAFT) ? TRUE : FALSE;
}



///////////////////////////////////////////////////////////////////////////////////////////////////

clsVTDWORDOption::clsVTDWORDOption() { Option = Opt::SKIP; }
VTDWORD clsVTDWORDOption::FilterSkip(VTDWORD val) const { if (Option == Opt::SKIP) return val; else return Option; }



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

clsBalloonTip::clsBalloonTip(UINT_PTR _IDTimerEvent)
{
	nIDEvent = _IDTimerEvent;
	hMsgWnd = NULL;
	hParentWnd = NULL;
	Offset_X = 0;
	Offset_Y = 0;
	TargetWidth = 0;
	TargetHeight = 0;
}

void clsBalloonTip::InitPosition(HWND hWnd, int _X, int _Y, int _Width, int _Height)
{
	hParentWnd = hWnd;
	Offset_X = _X;
	Offset_Y = _Y;
	TargetWidth = _Width;
	TargetHeight = _Height;
};

bool clsBalloonTip::ShowTemporaryMessage(LPCWSTR InfoText, const BYTE Transparency, const UINT Timer_ms)
// BE CAREFUL! InfoText IS ONLY PTR, NOT A BUFFER! You can add malloc() with memcpy() in ShowTemporaryMessage and free() in ~destructor.
// return true - OK
{
	if (!hParentWnd) return false;
	if (hMsgWnd) DestroyWindow(hMsgWnd); // Remove the old window if it is still hanging

	// SS_LEFTNOWORDWRAP
	hMsgWnd = CreateWindowExW (
		WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_LAYERED,
		L"STATIC", InfoText,
		WS_POPUP | SS_CENTER | WS_BORDER,
		Offset_X, Offset_Y,
		TargetWidth, TargetHeight,
		hParentWnd, // Parent
		(HMENU)NULL,
		GetModuleHandle(NULL), // Instance
		NULL); // lParam of WM_CREATE

	if (hMsgWnd)
	{
		Move(); // apply position limits
		SetLayeredWindowAttributes(hMsgWnd, 0, Transparency, LWA_ALPHA);
		Show();
		SetTimer(hParentWnd, nIDEvent, Timer_ms, NULL);
		return true;
	}
	else
	{
		DestroyTemporaryMessage();
		return false;
	}
}

void clsBalloonTip::DestroyTemporaryMessage()
{
	if ( IsWindow(hParentWnd) ) KillTimer(hParentWnd, nIDEvent);
	if (hMsgWnd) DestroyWindow(hMsgWnd);
	hMsgWnd = NULL; // don't touch the hParentWnd!
}

void clsBalloonTip::Move()
// move and resize with parent window client
{
	if (hMsgWnd && hParentWnd)
	{
		int toX, toY, limitedWidth, limitedHeight;
		PositionLimits(&toX, &toY, &limitedWidth, &limitedHeight);

		SetWindowPos(hMsgWnd, NULL,
			toX + Offset_X,
			toY + Offset_Y,
			limitedWidth, limitedHeight,
			SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void clsBalloonTip::PositionLimits(int *_X, int *_Y, int *_Width, int *_Height)
// to get absolute window coordinate without "Windows 10 Invisible Resizing Borders" and "Drop Shadows" you must call DwmGetWindowAttribute with DWMWA_EXTENDED_FRAME_BOUNDS instead GetWindowRect.
// however this function may not be implemented in older Windows versions.
// we will use old school GetClientRect and ClientToScreen instead
{
#if defined (__ULISTDEBUGMSG) && defined(__ULISTDEBUGBALLOON)
	OutputDebugStringW(L"*** PositionLimits ***");
#endif

	RECT rectParentClient; GetClientRect(hParentWnd, &rectParentClient);
	POINT pointParentClientTopLeft = { 0, 0 }; ClientToScreen(hParentWnd, &pointParentClientTopLeft);

	// absolute coordinate:
	// rectParentClient.top = pointParentClientTopLeft.y; <-never used
	// rectParentClient.left = pointParentClientTopLeft.x; <-never used
	// *_X = rectParentClient.left;
	// *_Y = rectParentClient.top;

	*_X = pointParentClientTopLeft.x;
	*_Y = pointParentClientTopLeft.y;
	rectParentClient.right = rectParentClient.right + pointParentClientTopLeft.x;
	rectParentClient.bottom = rectParentClient.bottom + pointParentClientTopLeft.y;

#if defined (__ULISTDEBUGMSG) && defined(__ULISTDEBUGBALLOON)
	std::wstring msgW = L"parent client absolute (" + ToStrW(pointParentClientTopLeft.x) + L", " + ToStrW(pointParentClientTopLeft.y) + L", " + ToStrW(rectParentClient.right) + L", " + ToStrW(rectParentClient.bottom) + L")";
	OutputDebugStringW(msgW.c_str());
#endif

	RECT rectMsgWindow; GetWindowRect(hMsgWnd, &rectMsgWindow);
	if (rectMsgWindow.left == Offset_X && rectMsgWindow.top == Offset_Y && !IsWindowVisible(hMsgWnd))
	{
		// it's impossible!
		// GetWindowRect fails due just created window is HIDDEN.

#if defined (__ULISTDEBUGMSG) && defined(__ULISTDEBUGBALLOON)
		OutputDebugStringW(L"GetWindowRect fails. Correcting coordinates.");
#endif

		rectMsgWindow.left += pointParentClientTopLeft.x;
		rectMsgWindow.top += pointParentClientTopLeft.y;
		rectMsgWindow.right += pointParentClientTopLeft.x;
		rectMsgWindow.bottom += pointParentClientTopLeft.y;
	}

#if defined (__ULISTDEBUGMSG) && defined(__ULISTDEBUGBALLOON)
	msgW = L"msg absolute (" + ToStrW(rectMsgWindow.left) + L", " + ToStrW(rectMsgWindow.top) + L", " + ToStrW(rectMsgWindow.right) + L", " + ToStrW(rectMsgWindow.bottom) + L")";
	OutputDebugStringW(msgW.c_str());
#endif

	union { int scrollbarHeight; int scrollbarWidth; };

	scrollbarHeight = GetSystemMetrics(SM_CYHSCROLL);
	*_Height = rectParentClient.bottom - rectMsgWindow.top - scrollbarHeight; // max available
	scrollbarWidth = GetSystemMetrics(SM_CXVSCROLL);
	*_Width = rectParentClient.right - rectMsgWindow.left - scrollbarWidth;

#if defined (__ULISTDEBUGMSG) && defined(__ULISTDEBUGBALLOON)
	msgW = L"max: newwidth=" + ToStrW(*_Width);
	OutputDebugStringW(msgW.c_str());
	msgW = L"max: newheight=" + ToStrW(*_Height);
	OutputDebugStringW(msgW.c_str());
#endif

	*_Height = (*_Height < 0) ? 0 : *_Height; // 0 if negative
	*_Width = (*_Width < 0) ? 0 : *_Width;

#if defined (__ULISTDEBUGMSG) && defined(__ULISTDEBUGBALLOON)
	msgW = L"positive: newwidth=" + ToStrW(*_Width);
	OutputDebugStringW(msgW.c_str());
	msgW = L"positive: newheight=" + ToStrW(*_Height);
	OutputDebugStringW(msgW.c_str());
#endif

	*_Height = (*_Height < TargetHeight) ? *_Height : TargetHeight; // min
	*_Width = (*_Width < TargetWidth) ? *_Width : TargetWidth;

#if defined (__ULISTDEBUGMSG) && defined(__ULISTDEBUGBALLOON)
	msgW = L"min: newwidth=" + ToStrW(*_Width);
	OutputDebugStringW(msgW.c_str());
	msgW = L"min: newheight=" + ToStrW(*_Height);
	OutputDebugStringW(msgW.c_str());
#endif
}

void clsBalloonTip::Show() { if (hMsgWnd) { ShowWindow(hMsgWnd, SW_SHOWNOACTIVATE); UpdateWindow(hMsgWnd); } }
void clsBalloonTip::Hide() { if (hMsgWnd) ShowWindow(hMsgWnd, SW_HIDE); }



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

		VTDWORD WebPrevWPfitmode;
		VTDWORD WebPrevHTMLfitmode;
		VTDWORD WebPrevEMAILfitmode;

		VTDWORD Vectorfitmode;
		VTDWORD Bitmapfitmode;

		VTDWORD ArcSortOrder;

		VTBOOL SpreadsheetDraftMode;
		VTBOOL SpreadsheetHiddenCells;
	};

	// unicode clipboard:
	locOptionSpec.dwId = SCCID_TOCLIPBOARD;
	locOptionSpec.pData = &ClipFormat;
	SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	ClipFormat = _VTOptions->VTClipboard.Get_SCCVW_CLIPFORMAT(ClipFormat);
	SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// drag-and-drop copying:
	locOptionSpec.dwId = SCCID_OLEFLAGS;
	//locOptionSpec.pData = &OLEFlags;
	SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	OLEFlags = _VTOptions->VTClipboard.Get_SCCVW_OLE(OLEFlags);
	SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// spreadsheet copying:
	locOptionSpec.dwId = SCCID_SSCLIPBOARD;
	//locOptionSpec.pData = &SpreadsheetClipboard;
	SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	SpreadsheetClipboard = _VTOptions->VTClipboard.SSCLIPBOARDSUBFORMAT.FilterSkip(SpreadsheetClipboard);
	SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// database copying:
	locOptionSpec.dwId = SCCID_DBCLIPBOARD;
	//locOptionSpec.pData = &DatabaseClipboard;
	SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	DatabaseClipboard = _VTOptions->VTClipboard.DBCLIPBOARDSUBFORMAT.FilterSkip(DatabaseClipboard);
	SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// word processor display engine:
	locOptionSpec.dwId = SCCID_WPDISPLAYMODE;
	//locOptionSpec.pData = &WPdisplaymode;
	SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	WPdisplaymode = _VTOptions->VTViewer.WPDISPLAYMODE.FilterSkip(WPdisplaymode);
	SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// HTML display engine:
	locOptionSpec.dwId = SCCID_HTMLDISPLAYMODE;
	//locOptionSpec.pData = &HTMLdisplaymode;
	SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	HTMLdisplaymode = _VTOptions->VTViewer.HTMLDISPLAYMODE.FilterSkip(HTMLdisplaymode);
	SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// email display engine:
	locOptionSpec.dwId = SCCID_EMAILDISPLAYMODE;
	//locOptionSpec.pData = &EMAILdisplaymode;
	SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	EMAILdisplaymode = _VTOptions->VTViewer.EMAILDISPLAYMODE.FilterSkip(EMAILdisplaymode);
	SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	/*
	It's not working propertly due Outside In Viewer library internal bug.
	The last call SendMessage::SCCVW_SETOPTION always sets the settings for all document types, ignoring the SCCID_WPFITMODE, SCCID_HTMLFITMODE, and SCCID_EMAILFITMODE types.
	However, after calling SendMessage::SCCVW_SETOPTION, these settings are stored inside the library (in the .oit directory).
	Only settings later read by the library from the internal .oit storage work correctly.
	A workaround would be to have SendMessage::SCCVW_SETOPTION settings sent to the library only if they have been changed in the ini file.
	Thus, only the first call after the changes, due to the effect of SendMessage::SCCVW_SETOPTION, the view will be incorrect.

	// size of word processor pages when using weblayout/preview mode:
	locOptionSpec.dwId = SCCID_WPFITMODE;
	//locOptionSpec.pData = &WebPrevWPfitmode;
	SendMessage(mydata->oiWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	WebPrevWPfitmode = _VTOptions->VTViewer.WEBPREVWPFITMODE.FilterSkip(WebPrevWPfitmode);
	SendMessage(mydata->oiWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// size of HTML pages when using weblayout/preview mode:
	locOptionSpec.dwId = SCCID_HTMLFITMODE;
	//locOptionSpec.pData = &WebPrevHTMLfitmode;
	SendMessage(mydata->oiWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	WebPrevHTMLfitmode = _VTOptions->VTViewer.WEBPREVHTMLFITMODE.FilterSkip(WebPrevHTMLfitmode);
	SendMessage(mydata->oiWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// size of EMAIL pages when using weblayout/preview mode:
	locOptionSpec.dwId = SCCID_EMAILFITMODE;
	//locOptionSpec.pData = &WebPrevEMAILfitmode;
	SendMessage(mydata->oiWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	WebPrevEMAILfitmode = _VTOptions->VTViewer.WEBPREVEMAILFITMODE.FilterSkip(WebPrevEMAILfitmode);
	SendMessage(mydata->oiWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
	*/

	/******************** WORKAROUND ********************/

	// size of word processor pages when using weblayout/preview mode:
	locOptionSpec.dwId = SCCID_WPFITMODE;
	//locOptionSpec.pData = &WebPrevWPfitmode;
	SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	if (_VTOptions->VTViewer.WEBPREVWPFITMODE.Option != Opt::SKIP && _VTOptions->VTViewer.WEBPREVWPFITMODE.Option != WebPrevWPfitmode)
	{
		WebPrevWPfitmode = _VTOptions->VTViewer.WEBPREVWPFITMODE.Option;
		SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
	}

	// size of HTML pages when using weblayout/preview mode:
	locOptionSpec.dwId = SCCID_HTMLFITMODE;
	//locOptionSpec.pData = &WebPrevHTMLfitmode;
	SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	if (_VTOptions->VTViewer.WEBPREVHTMLFITMODE.Option != Opt::SKIP && _VTOptions->VTViewer.WEBPREVHTMLFITMODE.Option != WebPrevHTMLfitmode)
	{
		WebPrevHTMLfitmode = _VTOptions->VTViewer.WEBPREVHTMLFITMODE.Option;
		SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
	}

	// size of EMAIL pages when using weblayout/preview mode:
	locOptionSpec.dwId = SCCID_EMAILFITMODE;
	//locOptionSpec.pData = &WebPrevEMAILfitmode;
	SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	if (_VTOptions->VTViewer.WEBPREVEMAILFITMODE.Option != Opt::SKIP && _VTOptions->VTViewer.WEBPREVEMAILFITMODE.Option != WebPrevEMAILfitmode)
	{
		WebPrevEMAILfitmode = _VTOptions->VTViewer.WEBPREVEMAILFITMODE.Option;
		SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
	}

	/****************************************************/

	// vector display engine:
	locOptionSpec.dwId = SCCID_VECFITMODE;
	//locOptionSpec.pData = &Vectorfitmode;
	SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	Vectorfitmode = _VTOptions->VTViewer.VECTORFITMODE.FilterSkip(Vectorfitmode);
	SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// bitmap display engine:
	locOptionSpec.dwId = SCCID_BMPFITMODE;
	//locOptionSpec.pData = &Bitmapfitmode;
	SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	Bitmapfitmode = _VTOptions->VTViewer.BITMAPFITMODE.FilterSkip(Bitmapfitmode);
	SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// spreadsheet display engine-1:
	locOptionSpec.dwId = SCCID_SSDRAFTMODE;
	//locOptionSpec.pData = &SpreadsheetDraftMode;
	SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	SpreadsheetDraftMode = _VTOptions->VTViewer.SPREADSHEETDISPLAYMODE.FilterSkipDraft(SpreadsheetDraftMode);
	SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// spreadsheet display engine-2:
	locOptionSpec.dwId = SCCID_SSSHOWHIDDENCELLS;
	//locOptionSpec.pData = &SpreadsheetHiddenCells;
	SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	SpreadsheetHiddenCells = _VTOptions->VTViewer.SPREADSHEETDISPLAYMODE.FilterSkipHiddenCells(SpreadsheetHiddenCells);
	SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	/****************************************************/

	// reset to default non-INI options:

	// archive display engine:
	locOptionSpec.dwId = SCCID_ARCSORTORDER;
	//locOptionSpec.pData = &ArcSortOrder;
	ArcSortOrder = SCCVW_SORT_NAME;
	SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

}
