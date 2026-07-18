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

	hFilterAccessLibrary = NULL;
	NumInstancesFilterAccessLib = 0;

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



void clsUlisterInstance::FilterAccessInstanceDec(bool _keepinmemory)
{
	// unload the "SCCFA.DLL" if needed

	if (NumInstancesFilterAccessLib > 0) NumInstancesFilterAccessLib--; else return;

	if ((hFilterAccessLibrary != NULL) && !_keepinmemory && (NumInstancesFilterAccessLib == 0))
	{
		FreeLibrary(hFilterAccessLibrary);
		hFilterAccessLibrary = NULL;
	}
}



HINSTANCE clsUlisterInstance::FilterAccessInstanceInc()
{
	// load the "SCCFA.DLL" if needed

	if (!hFilterAccessLibrary) hFilterAccessLibrary = LoadLibVT(L"SCCFA.DLL");
	if (!hFilterAccessLibrary) return NULL;

	NumInstancesFilterAccessLib++;
	return hFilterAccessLibrary;
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
"[ulister]\r\n"
"noloadtypes=1999\r\n"
"nopreviewtypes=1999\r\n"
"keepinmemory=1\r\n"
"\r\n"
"; optionsdir=%COMMANDER_PATH%\\Plugins\\wlx\\ulister\r\n"
"\r\n"
"; Ctrl-Shift-F1 - Thumbnails view\r\n"
"; Qtrl-Q - Quick View\r\n"
"; F3 - View\r\n"
"; Shift-F3 - multiple selected files viewing session\r\n"
"\r\n"
"; invert mouse wheel horizontal scroll\r\n"
"mwhscrollinvert=on\r\n"
"\r\n"
"; tooltip display time in milliseconds\r\n"
"tooltipsdelayms=3000\r\n"
"; tooltip transparency (0...255)\r\n"
"tooltipstransparency=244\r\n"
"\r\n"
"; auto disable/enable scroll bars enter/exit full-screen mode\r\n"
"smartscrollbar=on\r\n"
"\r\n"
"[clipboard]\r\n"
"; This options controls the clipboard formats that the viewer attempts to place on the clipboard\r\n"
"; skip|on|off\r\n"
"\r\n"
"ascii=on\r\n"
"rtf=on\r\n"
"\r\n"
"; turn it on!\r\n"
"unicode=on\r\n"
"\r\n"
"bitmap=on\r\n"
"windib=on\r\n"
"metafile=on\r\n"
"palette=on\r\n"
"\r\n"
"dragdrop=on\r\n"
"\r\n"
"; controls the format the spreadsheet or the database data takes when copied to the clipboard\r\n"
"; skip|rtf|tabs|optimizedtabs\r\n"
"spreadsheet=rtf\r\n"
"database=rtf\r\n"
"\r\n"
"[viewer]\r\n"
"; Display engine\r\n"
"; skip|draft|normal|preview|weblayout\r\n"
"wpdisplaymode=normal\r\n"
"htmldisplaymode=weblayout\r\n"
"emaildisplaymode=normal\r\n"
"\r\n"
"; skip|draft|normal|normalhidden\r\n"
"spreadsheetdisplaymode=normal\r\n"
"\r\n"
"; Extended for preview or weblayout mode of previous options.\r\n"
"; These settings are applied correctly only from the second launch of ulister after\r\n"
"; changes in the .ini file due to an bug in the Outside In Viewer library (workaround):\r\n"
"; skip|original|width|window\r\n"
"webprevwpfitmode=width\r\n"
"webprevhtmlfitmode=width\r\n"
"webprevemailfitmode=width\r\n"
"\r\n"
"; For bitmap and vector graphics\r\n"
"; skip|best|original|window|height|width|stretch|imagesize\r\n"
"vectorfitmode=window\r\n"
"bitmapfitmode=window\r\n"
"\r\n"
"[memory]\r\n"
"; buffers\r\n"
"readbuffersizekb=skip\r\n"
"mmapbuffersizekb=skip\r\n"
"tempbuffersizekb=skip\r\n"
"\r\n"
"; chunker memory\r\n"
"memorymode=skip\r\n"
;

/*
readbuffersizekb = 1024
mmapbuffersizekb = 32768
tempbuffersizekb = 8192
*/
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
#ifdef ULISTER64
	swprintf_s(buf, ULISTMAXBUF,
		L"\t%s\n\n"
		L"[%s].\n"
		L"Error code: 0x%08X   (%lu)\n"
		L"INI=[%s]\n\n"
		L"\t1. Check Outside In DLLs in plugin dir redist64.\n"
		L"\t2. Make sure that redist64 contains the 64-bit version\n\t    of the Outside In library.\n"
		L"\t3. Install the required version of Microsoft\n\t    Visual C++ Redistributable x64.\n\n"
		L"See readme.txt, install section.", issuename, path, dwError, dwError, inipath);
#else
	swprintf_s(buf, ULISTMAXBUF,
		L"\t%s\n\n"
		L"[%s].\n"
		L"Error code: 0x%08X   (%lu)\n"
		L"INI=[%s]\n\n"
		L"\t1. Check Outside In DLLs in plugin dir redist32.\n"
		L"\t2. Make sure that redist32 contains the 32-bit version\n\t    of the Outside In library.\n"
		L"\t3. Install the required version of Microsoft\n\t    Visual C++ Redistributable x86.\n\n"
		L"See readme.txt, install section.", issuename, path, dwError, dwError, inipath);
#endif

	MessageBoxW(NULL, buf, title, MB_OK);
}

