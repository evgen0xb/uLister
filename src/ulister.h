/*
The plugin is provided as-is and without any warranty under the GPLv3 license.
*/

//#define __ULISTDEBUGMSG
//#define __ULISTDEBUGKEEPINMEMORY
//#define __ULISTDEBUGBALLOON

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

#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined (_M_AMD64) || defined(_AMD64_)
#define ULISTER64
#endif

#include <sccvw.h>
#include "infowindow.h"



#define ULISTMAXBUF 1024
#define VTMAXSEARCHBUF 80
#define VTMAXTYPENAMEBUF 128
#define INT64STRMAXBUF 24

#define BALLOONTIPTIMER 3000
#define BALLOONTIP_TIMER_MSG 42 // and ID_TIMER_MSG=1 is used in the SCCVIEWER class window btw
#define BALLOONTIP_XOFFS 10
#define BALLOONTIP_YOFFS 10
#define BALLOONTIP_WIDTH 200
#define BALLOONTIP_HEIGHT 30
#define BALLOONTIP_TRANSPARENCY 244

#define INFOWINDOWWIDTH 400
#define INFOWINDOWHEIGHT 250

#define SMARTINIPATH

typedef char __VTTYPENAMEBUF[VTMAXTYPENAMEBUF];

// VS2005 fix:
#define member_size(type, member) sizeof(((type *)0)->member)

class clsLoadedFileInfo
{
public:
	wchar_t* pPath;
	VTWORD wType;
	char* pTypeName;

	clsLoadedFileInfo();
	~clsLoadedFileInfo();

	void Init(LPCWSTR _pPath, const VTWORD _wType, LPCSTR _pTypeName);
};

#define STRLEN(s) (sizeof(s)/sizeof(s[0])) // ! MUST BE ARRAY !

class clsBalloonTip
{
public:

	void InitPosition(HWND hWnd, int _X, int _Y, int _Width, int _Height);
	bool ShowTemporaryMessage(LPCWSTR InfoText, const BYTE Transparency, const UINT Timer_ms);
	void DestroyTemporaryMessage();
	void Move();

	void Show();
	void Hide();

	UINT_PTR nIDEvent;
	clsBalloonTip(UINT_PTR _IDEvent);
	//~clsBalloonTip();

private:

	HWND	hParentWnd;
	HWND	hMsgWnd;
	int		Offset_X;
	int		Offset_Y;
	int		TargetWidth;
	int		TargetHeight;

	void PositionLimits(int *_X, int *_Y, int *_Width, int *_Height);

};

struct ALLMYDATA
{
	HWND TListerWindow;
	WNDPROC OriginalTListerWindowProc;

	HWND waWindow; // WTF-naming
	
	WNDPROC OriginalSccviewerWindowProc;
	HWND SccviewerWindow;

	WNDPROC OriginalSccdisplayWindowProc;
	HWND SccdisplayWindow;

	clsBalloonTip BalloonTip;

	clsLoadedFileInfo LoadedFileInfo;
	clsInfoWindow InfoWindow;

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
	bool	keepinmemory;
	bool	mwhscrollinvert;

	UINT BalloonTipTimer;
	BYTE BalloonTransparency;
};

class clsUlisterInstance
{
public:
	
	HINSTANCE	hInstWLX;
	int			NTLevel;

	void Init(const HINSTANCE _hInst);
	~clsUlisterInstance();

	HINSTANCE ViewerLibraryInstanceInc();
	void ViewerLibraryInstanceDec(bool _keepinmemory);

	HINSTANCE FileIdentInstanceInc();
	void FileIdentInstanceDec(bool _keepinmemory);

private:
	
	HINSTANCE	hViewerLibrary;
	int			NumInstancesViewLib;

	HINSTANCE	hFileIdentLibrary;
	int			NumInstancesFileIdentLib;

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

namespace UlisterNextMode {
	enum Type {
		MPREV = -1,
		MNEXT = 1
	};
} // VS2005 fix

namespace UlisterSSDisplayMode {
	enum Type {
		SKIP = -1,
		DRAFT = 0,
		NORMAL,
		NORMALHIDDEN,
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

class clsSSViewModeOption
{
public:
	__int8 Option;
	clsSSViewModeOption();
	VTBOOL FilterSkipHiddenCells(VTBOOL val) const;
	VTBOOL FilterSkipDraft(VTBOOL val) const;
};


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

	clsSSViewModeOption SPREADSHEETDISPLAYMODE;

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
LRESULT CALLBACK TListerWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WAwcWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SccviewerWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SccdisplayWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND CreateListerWindow(HWND ParentWin);
VTWORD GetVTFileType(const wchar_t* FileToLoad, __VTTYPENAMEBUF &pOutTypeName);
bool IsVTFileTypeAllowed(const VTWORD wType, const wchar_t* onlyload, const wchar_t* noload);
HBITMAP GetVTFilePreview(const wchar_t* FileToLoad, const int width, const int height);
bool LoadVTFile(HWND hViewWnd, const wchar_t* FileToLoad);
void ErrMsgIssue(const int issuetype, const wchar_t *path, const DWORD dwError);
unsigned long long REGCurrentBuildNumber();
void SendVTOptions(const ALLMYDATA *mydata, const clsVTOptions *_VTOptions);
//void SetSccdisplayChildWndProc(HWND waWnd);
VTDWORD GetDisplayEngineVT(const HWND hWnd);
wchar_t* DisplayEngineName(const VTDWORD dwType);
extern "C" __declspec(dllexport)void __stdcall ListCloseWindow(HWND ListWin);
void AddFileInfo(ALLMYDATA *mydata);
void CreatFormatsTxt(const wchar_t* path);
