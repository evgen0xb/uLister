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

//OLD PreprocessorDefinitions: WIN32;NDEBUG;ULISTER_EXPORTS;_WINDOWS;_USRDLL;

#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined (_M_AMD64) || defined(_AMD64_)
#define ULISTER64
#endif

#include <sccvw.h>

#define ULISTMAXBUF 1024
#define VTMAXSEARCHBUF 80
#define INT64STRMAXBUF 24

// VS2005 fix:
#define member_size(type, member) sizeof(((type *)0)->member)

struct ALLMYDATA
{
	HWND ListerWindow;
	HWND waWindow; // WTF-naming
	
	WNDPROC OriginalSccviewerWindowProc;
	HWND SccviewerWindow;

	WNDPROC OriginalSccdisplayWindowProc;
	HWND SccdisplayWindow;

	ALLMYDATA();
};

#ifndef ULISTEROPTIONS
#define ULISTEROPTIONS
struct clsUlisterOptions
{
	wchar_t inipath[MAX_PATH];
	wchar_t	ininoloadtypes[ULISTMAXBUF];
	wchar_t	inionlyloadtypes[ULISTMAXBUF];
	wchar_t	ininopreviewtypes[ULISTMAXBUF];
	wchar_t	inionlypreviewtypes[ULISTMAXBUF];
	int		keepinmemory;
	bool	mwhscrollinvert;
};

class clsUlisterInstance
{
public:
	HINSTANCE	hInst;
	HANDLE		hViewerLibrary;
	int			numInstances;
	int			NTLevel;

	void Init(const HINSTANCE _hInst);
};

#endif

namespace WindowsNTLevel {
	enum Type {
		WinNT5=0,
		WinNT6
	};
} // VS2005 fix

namespace FileErrIssue {
	enum Type {
		CantFind = 0,
		CantLoad
	};
} // VS2005 fix

namespace Opt { // Outside In Technology option representation
	enum Type {
		SKIP = -1, // don't change
		OFF = 0, // set FALSE
		ON = 1 // set TRUE
	};
} // VS2005 fix

namespace UlisterZoom {
	enum Type {
		ZOUT = -1,
		ZRESET = 0,
		ZIN = 1
	};
} // VS2005 fix

#ifndef VTOPTIONSCLIPBOARD
#define VTOPTIONSCLIPBOARD

class clsVTDWORDOption // simpest VTDWORD-wrapper for extra value 'SKIP'
{
public:
	VTDWORD Option;
	clsVTDWORDOption();
	VTDWORD FilterSkip(VTDWORD val) const;
};

class clsVTOptionsClipboard
{
public:

	clsVTDWORDOption SSCLIPBOARDSUBFORMAT;
	clsVTDWORDOption DBCLIPBOARDSUBFORMAT;
	__int8 FORMAT_TEXT;
	__int8 FORMAT_RTF;
	__int8 FORMAT_UNICODE;
	__int8 FORMAT_WINBITMAP;
	__int8 FORMAT_WINDIB;
	__int8 FORMAT_WINMETAFILE;
	__int8 FORMAT_WINPALETTE;
	__int8 OLE_ENABLEDRAGDROP;

	clsVTOptionsClipboard();
	VTDWORD Get_SCCVW_CLIPFORMAT(VTDWORD ClipFormat) const;
	VTDWORD Get_SCCVW_OLE(VTDWORD OLEFlags) const;

}; // clsVTOptionsClipboard
#endif

#ifndef VTOPTIONSVIEWER
#define VTOPTIONSVIEWER

class clsVTOptionsViewer
{
public:

	clsVTDWORDOption WPDISPLAYMODE;
	clsVTDWORDOption HTMLDISPLAYMODE;
	clsVTDWORDOption EMAILDISPLAYMODE;

	clsVTDWORDOption WEBPREVWPFITMODE;
	clsVTDWORDOption WEBPREVHTMLFITMODE;
	clsVTDWORDOption WEBPREVEMAILFITMODE;

	clsVTDWORDOption VECTORFITMODE;
	clsVTDWORDOption BITMAPFITMODE;

}; // clsVTOptionsViewer
#endif

#ifndef VTOPTIONS
#define VTOPTIONS
class clsVTOptions
{
public:
	clsVTOptionsClipboard	VTClipboard;
	clsVTOptionsViewer		VTViewer;
};
#endif

#define WINDOWS7BETABUILDNUMBER 7000

void IniParse();
HINSTANCE LoadLibVT(const wchar_t *libname);
void ZoomBitmapVecFont(const HWND hWnd, const int dir);
DWORD ViewThisFileHandler(const LPARAM lParam);
LRESULT CALLBACK ParentWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SccviewerWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND CreateLister(HWND ParentWin);
VTWORD GetVTFileType(const wchar_t* FileToLoad);
bool IsVTFileTypeAllowed(const wchar_t* FileToLoad, const wchar_t* onlyload, const wchar_t* noload);
HBITMAP GetVTFilePreview(const wchar_t* FileToLoad, const int width, const int height);
void LoadVTFile(HWND hViewWnd, const wchar_t* FileToLoad);
void ErrMsgIssue(const int issuetype, const wchar_t *path, const DWORD dwError);
unsigned long long REGCurrentBuildNumber();
void SendVTOptions(const ALLMYDATA *mydata, const clsVTOptions *_VTOptions);
//void SetSccdisplayChildWndProc(HWND waWnd);
LRESULT CALLBACK SccdisplayWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
