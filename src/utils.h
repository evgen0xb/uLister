/*
The plugin is provided as-is and without any warranty under the GPLv3 license.
*/

#ifndef ULISTERUTILS
#define ULISTERUTILS

#include <windows.h>
#include <sccvw.h>

#include "ulister.h"
#include "init.h"



namespace FileErrIssue {
	enum Type {
		CantFind = 0,
		CantLoad
	};
} // VS2005 fix

namespace UlisterZoom {
	enum Type {
		ZOUT = -1,
		ZRESET = 0,
		ZIN = 1
	};
} // VS2005 fix

namespace UlisterNextMode {
	enum Type {
		MPREV = -1,
		MNEXT = 1
	};
} // VS2005 fix



#if defined (__ULISTDEBUGMSG)
// for using with Sysinternals Debug Output Viewer

#ifndef __ULISTEMPLATELIB
#define __ULISTEMPLATELIB
// C++17 and earlier
// https://stackoverflow.com/questions/5100718/integer-to-hex-string-in-c
// WARNING: this will not work for single byte because char is always threated as char
#include <sstream>
#include <iomanip> // CRITICAL: Required for std::setfill and std::setw
template< typename T > std::string ToHexA(T i)
{
	std::stringstream stream;
	stream << "0x"
		<< std::setfill('0') << std::setw(sizeof(T) * 2)
		<< std::hex << i;
	return stream.str();
}
template< typename T > std::wstring ToHexW(T i)
{
	std::wstringstream stream;
	stream << L"0x"
		<< std::setfill(L'0') << std::setw(sizeof(T) * 2)
		<< std::hex << i;
	return stream.str();
}
template< typename T > std::string ToStrA(T i)
{
	std::stringstream stream;
	stream << i;
	return stream.str();
}
template< typename T > std::wstring ToStrW(T i)
{
	std::wstringstream stream;
	stream << i;
	return stream.str();
}
#endif
#endif



// VS2005 fix:
#define member_size(type, member) sizeof(((type *)0)->member)

#define STRLEN(s) (sizeof(s)/sizeof(s[0])) // ! MUST BE ARRAY !

#define WINDOWS7BETABUILDNUMBER 7000



VTWORD GetVTFileType(const wchar_t* FileToLoad, __VTTYPENAMEBUF &pOutTypeName);
bool IsVTFileTypeAllowed(const VTWORD wType, const wchar_t* onlyload, const wchar_t* noload);

bool LoadVTFile(const HWND hViewWnd, const wchar_t* FileToLoad);
DWORD ViewThisFileHandler(const LPARAM lParam);
void SendVTOptions(const ALLMYDATA *mydata, const clsVTOptions *_VTOptions);

HBITMAP GetVTFilePreview(const wchar_t* FileToLoad, const int width, const int height);

void CreateDefaultUlisterIni(wchar_t *_inipath);
void CreatFormatsTxt(const wchar_t* path);

//bool GetLibPathVT(wchar_t *libpath, const wchar_t *libname, const int ntlev);

//HINSTANCE LoadLibVT(const wchar_t *libname);

//unsigned long long REGCurrentBuildNumber();

VTDWORD GetDisplayEngineVT(const HWND hWnd);
wchar_t* DisplayEngineName(const VTDWORD dwType);

void ChangeViewMode(const HWND hWnd, const int dir);
void ZoomBitmapVecFont(const HWND hWnd, const int dir);


void AddFileInfo(ALLMYDATA *mydata); // TODO mydata->AddFileInfo???



#endif
