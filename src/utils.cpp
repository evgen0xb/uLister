/* 
The plugin is provided as-is and without any warranty under the GPLv3 license.
*/

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>

#include "ulister.h"
#include "utils.h"
#include "init.h"



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



const char *AFIINIT				= "FIInit";
const char *AFIDEINIT			= "FIDeInit";
const char *AFIGETFIRSTID		= "FIGetFirstId";
const char *AFIGETNEXTID		= "FIGetNextId";
const char *AFIIDFILEEX			= "FIIdFileEx";



extern const char *WNDCLASSNAME_SCCVIEWER;



void ErrMsgIssue(const int issuetype, const wchar_t *path, const DWORD dwError);



///////////////////////////////////////////////////////////////////////////////////////////////////



VTWORD GetVTFileType(const wchar_t* FileToLoad, __VTTYPENAMEBUF &pOutTypeName)
// return:
// Type Number (VTWORD)
// pOutTypeName: Type Name
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
			FIErrorCode = FIIdFileEx(IOTYPE_UNICODEPATH, FileToLoad, dwFlags, &wVTFileType, pOutTypeName, sizeof(pOutTypeName));
			if (FIErrorCode != SCCERR_OK) pOutTypeName[0] = '\0';
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
				__VTTYPENAMEBUF buf;
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



///////////////////////////////////////////////////////////////////////////////////////////////////



bool IsVTFileTypeAllowed(const VTWORD wType, const wchar_t* onlyload, const wchar_t* noload)
{
	// TRUE = OK
	// FALSE = refuse

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

	// if WinXP (and VISTA), first try to load library from "XPdist*"
	if (UlisterInstance.WindowsBuildNumber < WINDOWS7BETABUILDNUMBER)
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



void AddFileInfo(ALLMYDATA *mydata) // TODO mydata->AddFileInfo???
{

	wchar_t buf[ULISTMAXBUF];
	swprintf_s(buf, ULISTMAXBUF,
		L"File: %s\r\n\r\n"
		L"Format type: %u  -  %S\r\n\r\n"
		L"Display engine: %s\r\n",
		mydata->LoadedFileInfo.pPath,
		mydata->LoadedFileInfo.wType, mydata->LoadedFileInfo.pTypeName,
		DisplayEngineName(GetDisplayEngineVT(mydata->SccviewerWindow)));

	mydata->InfoWindow.Init(buf, INFOWINDOWWIDTH, INFOWINDOWHEIGHT);

}

ALLMYDATA::ALLMYDATA() : ToolTip(TOOLTIP_TIMER_MSG)
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

	WindowsBuildNumber = REGCurrentBuildNumber();
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



clsLoadedFileInfo::clsLoadedFileInfo() { pPath = NULL; pTypeName = NULL; }

clsLoadedFileInfo::~clsLoadedFileInfo() { if (pPath) free(pPath); if (pTypeName) free(pTypeName); }

void clsLoadedFileInfo::Init(LPCWSTR _pPath, const VTWORD _wType, LPCSTR _pTypeName)
{
	pPath = _wcsdup(_pPath);
	wType = _wType;
	pTypeName = _strdup(_pTypeName);
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



///////////////////////////////////////////////////////////////////////////////////////////////////



VTDWORD GetDisplayEngineVT(const HWND hWnd)
{
	// SCCVWTYPE_NONE		1  /* no file open in this view */
	// SCCVWTYPE_UNKNOWN	2  /* unknown section type */
	// SCCVWTYPE_WP			3  /* word processor section */
	// SCCVWTYPE_SS			4  /* spreadsheet section */
	// SCCVWTYPE_DB			5  /* database section */
	// SCCVWTYPE_HEX		6  /* hex view of any file */
	// SCCVWTYPE_IMAGE		7  /* bitmap image */
	// SCCVWTYPE_ARCHIVE	8  /* archive */
	// SCCVWTYPE_VECTOR		9	Vector graphics display engine
	// SCCVWTYPE_SOUND		10 ***** /* sound file */ *****
	// SCCVWTYPE_HTML		11 /* html file */
	// SCCVWTYPE_EMAIL		12 /* email file */

	SCCVWDISPLAYINFO40 locDisplayInfo;
	locDisplayInfo.dwSize = sizeof(SCCVWDISPLAYINFO40);
	SendMessage(hWnd, SCCVW_GETDISPLAYINFO, 0, (LPARAM)(PSCCVWDISPLAYINFO40)&locDisplayInfo);
	return locDisplayInfo.dwType;
}



///////////////////////////////////////////////////////////////////////////////////////////////////



wchar_t* DisplayEngineName(const VTDWORD dwType)
{
	wchar_t* name[] =
	{
		L"COMMON UNKNOWN",
		L"No file open",
		L"Unknown type",
		L"Word Processor",
		L"Spreadsheet",
		L"Database",
		L"Hexadecimal view",
		L"Bitmap image",
		L"Archive",
		L"Vector graphics",
		L"Sound file",
		L"HTML document",
		L"email"
	};
	const unsigned int count = sizeof(name) / sizeof(name[0]) - 1;
	return name[(dwType > count) ? 0 : dwType];
}



///////////////////////////////////////////////////////////////////////////////////////////////////



void ChangeViewMode(const HWND hWnd, const int dir)
{
	// dir =  1		- next view mode
	// dir = -1		- prev view mode

	LPCWSTR AUNK = L"Unknown";
	LPCWSTR ADRAFT = L"Draft";
	LPCWSTR ANORMAL = L"Normal";
	LPCWSTR APREVIEW = L"Preview";
	LPCWSTR AWEBLAY = L"Weblayout";
	LPCWSTR AHIDDEN = L"Hidden";
	LPCWSTR ANONE = L"None";
	LPCWSTR ANAME = L"Name";
	LPCWSTR ASIZE = L"Size";
	LPCWSTR ADATE = L"Date";
	LPCWSTR A0 = L"0\u00B0";
	LPCWSTR A90 = L"90\u00B0";
	LPCWSTR A180 = L"180\u00B0";
	LPCWSTR A270 = L"270\u00B0";

	LPCWSTR VIEWMODENAME = AUNK;

	ALLMYDATA *mydata;
	VTDWORD DispEng;
	mydata = (ALLMYDATA *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	union
	{
		VTDWORD viewmode;
		VTDWORD bitmaprotation;
		VTDWORD arcsortorder;
		VTBOOL spreadsheetdraft;
	};

	SCCVWOPTIONSPEC40 locOptionSpec;
	locOptionSpec.dwSize = sizeof(SCCVWOPTIONSPEC40);
	locOptionSpec.dwFlags = SCCVWOPTION_CURRENT;
	locOptionSpec.pData = &viewmode;

	DispEng = GetDisplayEngineVT(mydata->SccviewerWindow); // call only from user-level defined messages!!!
	if (DispEng == SCCVWTYPE_WP || DispEng == SCCVWTYPE_HTML || DispEng == SCCVWTYPE_EMAIL)
	{
		// word processor: draft->normal->preview->weblayout

		if (DispEng == SCCVWTYPE_WP) locOptionSpec.dwId = SCCID_WPDISPLAYMODE;
		else locOptionSpec.dwId = (DispEng == SCCVWTYPE_HTML) ? SCCID_HTMLDISPLAYMODE : SCCID_EMAILDISPLAYMODE;

		SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		if (dir == UlisterNextMode::MNEXT) { viewmode++; if (viewmode > SCCVW_WPMODE_WEBLAYOUT) viewmode = SCCVW_WPMODE_WEBLAYOUT; }
		else { viewmode--; if (viewmode < SCCVW_WPMODE_DRAFT) viewmode = SCCVW_WPMODE_DRAFT; }

		SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

		if (viewmode == SCCVW_WPMODE_DRAFT) VIEWMODENAME = ADRAFT;
		else if (viewmode == SCCVW_WPMODE_NORMAL) VIEWMODENAME = ANORMAL;
		else if (viewmode == SCCVW_WPMODE_PREVIEW) VIEWMODENAME = APREVIEW;
		else if (viewmode == SCCVW_WPMODE_WEBLAYOUT) VIEWMODENAME = AWEBLAY;
		else VIEWMODENAME = AUNK;

		mydata->ToolTip.InitPosition(mydata->TListerWindow, TOOLTIP_XOFFS, TOOLTIP_YOFFS, TOOLTIP_WIDTH, TOOLTIP_HEIGHT);
		mydata->ToolTip.ShowTemporaryMessage(VIEWMODENAME, UlisterOptions.ToolTipTransparency, UlisterOptions.ToolTipTimer);
	}
	else if (DispEng == SCCVWTYPE_SS)
	{
		// SCCID_SSSHOWGRIDLINES ???

		// spreadsheet: draft->normal->normal with hidden rows and columns displayed

		VTBOOL spreadsheethiddencells;

		//locOptionSpec.pData = &spreadsheetdraft;
		locOptionSpec.dwId = SCCID_SSDRAFTMODE;
		SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		locOptionSpec.pData = &spreadsheethiddencells;
		locOptionSpec.dwId = SCCID_SSSHOWHIDDENCELLS;
		SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		if (dir == UlisterNextMode::MNEXT)
		{
			if (spreadsheethiddencells == FALSE && spreadsheetdraft == TRUE) { spreadsheetdraft = FALSE; VIEWMODENAME = ANORMAL; } // draft->normal
			else if (spreadsheethiddencells == FALSE && spreadsheetdraft == FALSE) { spreadsheethiddencells = TRUE; VIEWMODENAME = AHIDDEN; } // normal->normal with hidden rows and columns displayed
			else if (spreadsheethiddencells == TRUE && spreadsheetdraft == FALSE) { VIEWMODENAME = AHIDDEN; } // normal with hidden rows and columns displayed->nothing
			else { spreadsheethiddencells = FALSE; spreadsheetdraft = TRUE; VIEWMODENAME = ADRAFT; } // reset to draft
		}
		else
		{
			if (spreadsheethiddencells == TRUE && spreadsheetdraft == FALSE) { spreadsheethiddencells = FALSE; VIEWMODENAME = ANORMAL; } // normal with hidden rows and columns displayed->normal
			else if (spreadsheethiddencells == FALSE && spreadsheetdraft == FALSE) { spreadsheetdraft = TRUE; VIEWMODENAME = ADRAFT; } // normal->draft
			else if (spreadsheethiddencells == FALSE && spreadsheetdraft == TRUE) { VIEWMODENAME = ADRAFT; } // draft->nothing
			else { spreadsheethiddencells = TRUE; spreadsheetdraft = FALSE; VIEWMODENAME = AHIDDEN; } // reset to normal with hidden rows and columns displayed
		}

		//locOptionSpec.pData = &spreadsheethiddencells;
		//locOptionSpec.dwId = SCCID_SSSHOWHIDDENCELLS;
		SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		locOptionSpec.pData = &spreadsheetdraft;
		locOptionSpec.dwId = SCCID_SSDRAFTMODE;
		SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		mydata->ToolTip.InitPosition(mydata->TListerWindow, TOOLTIP_XOFFS, TOOLTIP_YOFFS, TOOLTIP_WIDTH, TOOLTIP_HEIGHT);
		mydata->ToolTip.ShowTemporaryMessage(VIEWMODENAME, UlisterOptions.ToolTipTransparency, UlisterOptions.ToolTipTimer);
	}
	else if (DispEng == SCCVWTYPE_ARCHIVE)
	{
		// NONE->NAME->SIZE->DATE

		//locOptionSpec.pData = &arcsortorder;
		locOptionSpec.dwId = SCCID_ARCSORTORDER;
		SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		if (dir == UlisterNextMode::MNEXT) { arcsortorder++; if (arcsortorder > SCCVW_SORT_DATE) arcsortorder = SCCVW_SORT_DATE; }
		else { arcsortorder--; if (arcsortorder < SCCVW_SORT_NONE) arcsortorder = SCCVW_SORT_NONE; }

		SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
		// This option is saved in the .oit directory and does need to be reset to SCCVW_SORT_NAME when loading an archive file (see SendVTOptions function).

		if (arcsortorder == SCCVW_SORT_NONE) VIEWMODENAME = ANONE;
		else if (arcsortorder == SCCVW_SORT_NAME) VIEWMODENAME = ANAME;
		else if (arcsortorder == SCCVW_SORT_SIZE) VIEWMODENAME = ASIZE;
		else if (arcsortorder == SCCVW_SORT_DATE) VIEWMODENAME = ADATE;
		else VIEWMODENAME = AUNK;

		mydata->ToolTip.InitPosition(mydata->TListerWindow, TOOLTIP_XOFFS, TOOLTIP_YOFFS, TOOLTIP_WIDTH, TOOLTIP_HEIGHT);
		mydata->ToolTip.ShowTemporaryMessage(VIEWMODENAME, UlisterOptions.ToolTipTransparency, UlisterOptions.ToolTipTimer);
	}
	else if (DispEng == SCCVWTYPE_IMAGE)
	{
		// SCCID_ANTIALIAS ??? SCCVW_ANTIALIAS_OFF | SCCVW_ANTIALIAS_ALL ???

		// rotate 0->90->180->270->0->...

		//locOptionSpec.pData = &bitmaprotation;
		locOptionSpec.dwId = SCCID_BMPROTATION;
		SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		if (dir == UlisterNextMode::MNEXT) { bitmaprotation = bitmaprotation + 90; if (bitmaprotation > SCCVW_ROTATION_270) bitmaprotation = SCCVW_ROTATION_NONE; }
		else { bitmaprotation = bitmaprotation - 90; if (bitmaprotation > SCCVW_ROTATION_270) bitmaprotation = SCCVW_ROTATION_270; } // unsigned int (VTDWORD) overflow hack

		SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
		// It seems that this option is not saved in the .oit directory and does not need to be reset to SCCVW_ROTATION_NONE when loading an image file.

		if (bitmaprotation == SCCVW_ROTATION_NONE) VIEWMODENAME = A0;
		else if (bitmaprotation == SCCVW_ROTATION_90) VIEWMODENAME = A90;
		else if (bitmaprotation == SCCVW_ROTATION_180) VIEWMODENAME = A180;
		else if (bitmaprotation == SCCVW_ROTATION_270) VIEWMODENAME = A270;
		else VIEWMODENAME = AUNK;

		mydata->ToolTip.InitPosition(mydata->TListerWindow, TOOLTIP_XOFFS, TOOLTIP_YOFFS, TOOLTIP_WIDTH, TOOLTIP_HEIGHT);
		mydata->ToolTip.ShowTemporaryMessage(VIEWMODENAME, UlisterOptions.ToolTipTransparency, UlisterOptions.ToolTipTimer);
	}
	/*
	else if (DispEng == SCCVWTYPE_VECTOR)
	{
	// SCCID_VECSHOWFULLSCREEN ???
	// SCCID_STROKE_TEXT ???
	}
	else if (DispEng == SCCVWTYPE_DB)
	{
	// SCCID_DBDRAFTMODE ???
	// SCCID_DBSHOWGRIDLINES ???
	}
	else if (DispEng == SCCVWTYPE_HEX)
	{
	// ??? nothing
	}
	*/
}



///////////////////////////////////////////////////////////////////////////////////////////////////



void ZoomBitmapVecFont(const HWND hWnd, const int dir)
{
	// dir =  1		- zoom in
	// dir = -1		- zoom out
	// dir =  0		- zoom reset to 100% (146% in Russian Federation)

	ALLMYDATA *mydata;
	VTDWORD DispEng;
	mydata = (ALLMYDATA *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	SCCVWOPTIONSPEC40 locOptionSpec;
	VTDWORD zoom;
	locOptionSpec.dwSize = sizeof(SCCVWOPTIONSPEC40);
	locOptionSpec.dwFlags = SCCVWOPTION_CURRENT;
	locOptionSpec.pData = &zoom;

	DispEng = GetDisplayEngineVT(mydata->SccviewerWindow); // call only from user-level defined messages!!!
	if (DispEng == SCCVWTYPE_IMAGE)
	{
		locOptionSpec.dwId = SCCID_BMPZOOMEVENT;
		if (dir == UlisterZoom::ZRESET) zoom = SCCVW_ZOOM_RESET;
		else zoom = (dir == UlisterZoom::ZIN) ? SCCVW_ZOOM_IN : SCCVW_ZOOM_OUT;
		SendMessage(hWnd, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
	}
	else if (DispEng == SCCVWTYPE_VECTOR)
	{
		locOptionSpec.dwId = SCCID_VECZOOMEVENT;
		if (dir == UlisterZoom::ZRESET) zoom = SCCVW_ZOOM_RESET;
		else zoom = (dir == UlisterZoom::ZIN) ? SCCVW_ZOOM_IN : SCCVW_ZOOM_OUT;
		SendMessage(hWnd, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
	}
	else if (DispEng == SCCVWTYPE_WP || DispEng == SCCVWTYPE_HTML || DispEng == SCCVWTYPE_EMAIL)
	{
		// oracle bug: SCCID_FONTSCALINGFACTOR not working if SCCVW_WPMODE_PREVIEW or SCCVW_WPMODE_WEBLAYOUT mode of Word Processor / HTML / EMAIL!
		// From A.10.5 SCCID_FONTSCALINGFACTOR Note:
		// For word processor documents, this only affects normal and draft modes.

		VTDWORD WPdisplaymode;
		locOptionSpec.dwId = SCCID_WPDISPLAYMODE;
		locOptionSpec.pData = &WPdisplaymode;
		SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		VTDWORD HTMLdisplaymode;
		locOptionSpec.dwId = SCCID_HTMLDISPLAYMODE;
		locOptionSpec.pData = &HTMLdisplaymode;
		SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		VTDWORD EMAILdisplaymode;
		locOptionSpec.dwId = SCCID_EMAILDISPLAYMODE;
		locOptionSpec.pData = &EMAILdisplaymode;
		SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		locOptionSpec.pData = &zoom;
		locOptionSpec.dwId = SCCID_FONTSCALINGFACTOR;
		SendMessage(hWnd, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
		if (dir == UlisterZoom::ZRESET) zoom = 100; // percent
		else zoom = (dir == UlisterZoom::ZIN) ? zoom * 10 / 8 : zoom * 8 / 10;
		SendMessage(hWnd, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

		// workaround:
		if (DispEng == SCCVWTYPE_WP && (WPdisplaymode == SCCVW_WPMODE_PREVIEW || WPdisplaymode == SCCVW_WPMODE_WEBLAYOUT) ||
			DispEng == SCCVWTYPE_HTML && (HTMLdisplaymode == SCCVW_WPMODE_PREVIEW || HTMLdisplaymode == SCCVW_WPMODE_WEBLAYOUT) ||
			DispEng == SCCVWTYPE_EMAIL && (EMAILdisplaymode == SCCVW_WPMODE_PREVIEW || EMAILdisplaymode == SCCVW_WPMODE_WEBLAYOUT))
		{
			// temporarily switch to draft mode
			zoom = SCCVW_WPMODE_DRAFT;

			ShowWindow(hWnd, SW_HIDE);

			locOptionSpec.dwId = SCCID_WPDISPLAYMODE;
			SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

			locOptionSpec.dwId = SCCID_HTMLDISPLAYMODE;
			SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

			locOptionSpec.dwId = SCCID_EMAILDISPLAYMODE;
			SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

			// and switch back

			locOptionSpec.dwId = SCCID_WPDISPLAYMODE;
			locOptionSpec.pData = &WPdisplaymode;
			SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

			locOptionSpec.dwId = SCCID_HTMLDISPLAYMODE;
			locOptionSpec.pData = &HTMLdisplaymode;
			SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

			locOptionSpec.dwId = SCCID_EMAILDISPLAYMODE;
			locOptionSpec.pData = &EMAILdisplaymode;
			SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

			ShowWindow(hWnd, SW_SHOW);
		}
	}
	else if (DispEng == SCCVWTYPE_SS || DispEng == SCCVWTYPE_DB ||
		DispEng == SCCVWTYPE_HEX || DispEng == SCCVWTYPE_ARCHIVE)
	{
		locOptionSpec.dwId = SCCID_FONTSCALINGFACTOR;
		SendMessage(hWnd, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
		if (dir == UlisterZoom::ZRESET) zoom = 100; // percent
		else zoom = (dir == UlisterZoom::ZIN) ? zoom * 10 / 8 : zoom * 8 / 10;
		SendMessage(hWnd, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
	}
}
