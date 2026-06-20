/*
The plugin is provided as-is and without any warranty under the GPLv3 license.
*/

#include <windows.h>
#include <stdio.h>

#include "ulister.h"
#include "ulisterinstance.h"
#include "utils.h"
#include "config.h"



namespace WindowsNTLevel {
	enum Type {
		WinNT5 = 0,
		WinNT6
	};
} // VS2005 fix



#ifdef ULISTER64
wchar_t *REDIST_NT6 = L"\\redist64\\";
wchar_t *REDIST_NT5 = L"\\XPdist64\\";
#else
wchar_t *REDIST_NT6 = L"\\redist32\\";
wchar_t *REDIST_NT5 = L"\\XPdist32\\";
#endif



const wchar_t ULISTERINI[] = L"\\ulister.ini"; // ! MUST BE ARRAY !



void clsUlisterInstance::InitUlister(const HINSTANCE _hInst)
{
	hInstWLX = _hInst;

	hViewerLibrary = NULL;
	NumInstancesViewLib = 0;

	hFileIdentLibrary = NULL;
	NumInstancesFileIdentLib = 0;

	GetRegCurrentBuildNumber();
	GetIniPath();
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



void clsUlisterInstance::GetIniPathWLX(wchar_t *_inipath)
{
	wchar_t *pathposition;
	GetModuleFileNameW(hInstWLX, _inipath, MAX_PATH);
	_inipath[MAX_PATH - 1] = L'\0'; // Windows XP fix: The string is truncated to nSize characters and is not null-terminated
	if (pathposition = wcsrchr(_inipath, L'\\')) *pathposition = L'\0';
	wcscat_s(_inipath, MAX_PATH, ULISTERINI);
}



void clsUlisterInstance::GetIniPathCOMMANDER(wchar_t *_inipath)
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



void clsUlisterInstance::GetIniPathAPPDATA(wchar_t *_inipath)
{
	DWORD retlength = GetEnvironmentVariableW(L"APPDATA", _inipath, MAX_PATH);
	if ((retlength + STRLEN(ULISTERINI) - 1 < MAX_PATH) && retlength) wcscat_s(_inipath, MAX_PATH, ULISTERINI);
	else _inipath[0] = L'\0';
}



bool clsUlisterInstance::GetIniPath()
// true  - ulister.ini was found
// false - ulister.ini does not exist
{
	bool isexist = false;

	// highest priority is to place ulister.ini in the plugin directory
	GetIniPathWLX(inipath);

	if (GetFileAttributesW(inipath) == INVALID_FILE_ATTRIBUTES)
	{
		// otherwise, look for ulister.ini in the same place as wincmd.ini
		GetIniPathCOMMANDER(inipath);

		if (GetFileAttributesW(inipath) == INVALID_FILE_ATTRIBUTES)
		{
			// lowest priority ulister.ini placed in %APPDATA%
			GetIniPathAPPDATA(inipath);
			if (GetFileAttributesW(inipath) != INVALID_FILE_ATTRIBUTES) isexist = true;
		}
		else isexist = true;
	}
	else isexist = true;

#ifdef SMARTINIPATH
	// last chance:
	// try to create default ulister.ini in the plugin directory
	if (!isexist)
	{
		GetIniPathWLX(inipath);
		CreateDefaultUlisterIni(inipath);
		if (GetFileAttributesW(inipath) != INVALID_FILE_ATTRIBUTES) isexist = true;
	}
#endif

	return isexist;
}




#ifdef SMARTINIPATH
void clsUlisterInstance::CreateDefaultUlisterIni(wchar_t *_inipath)
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
	const DWORD cfglength = STRLEN(defaultconfig) - 1;

	HANDLE hFile = CreateFileW(_inipath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return;
	WriteFile(hFile, defaultconfig, cfglength, &bytesWritten, NULL);
	CloseHandle(hFile);
}
#endif





void clsUlisterInstance::GetRegCurrentBuildNumber()
{
	char readbuf[INT64STRMAXBUF];
	DWORD readbytes = INT64STRMAXBUF - 1;

	HKEY hKey = NULL;
	WindowsBuildNumber = 0;
	LONG Stat; // fix VS2005

	Stat = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &hKey);
	if (Stat == ERROR_SUCCESS)
	{
		Stat = RegQueryValueExA(hKey, "CurrentBuildNumber", NULL, NULL, (LPBYTE)readbuf, &readbytes); // Windows XP SP0 support
		if (Stat == ERROR_SUCCESS)
		{
			readbuf[readbytes] = 0; // the return value of RegQueryValueExA may not contain the '\0' character at the end
			WindowsBuildNumber = _strtoui64(readbuf, NULL, 10); // VS2005 fix
		}
	}

	RegCloseKey(hKey);
}



bool clsUlisterInstance::GetLibPathVT(wchar_t *libpath, const wchar_t *libname, const int ntlev)
{
	// OUT: build libpath and (!) check if it exists;   true=OK

	GetModuleFileNameW(hInstWLX, libpath, MAX_PATH);

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



HINSTANCE clsUlisterInstance::LoadLibVT(const wchar_t *libname)
{
	HINSTANCE lib = NULL;
	wchar_t path[MAX_PATH];

	// if WinXP (and VISTA), first try to load library from "XPdist*"
	if (WindowsBuildNumber < WINDOWS7BETABUILDNUMBER)
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



void clsUlisterInstance::ErrMsgIssue(const int issuetype, const wchar_t *path, const DWORD dwError)
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
		L"See readme.txt, install section.", issuename, path, dwError, dwError, inipath);

	MessageBoxW(NULL, buf, title, MB_OK);
}

