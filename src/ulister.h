/*
The plugin is provided as-is and without any warranty under the GPLv3 license.
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
	
	HINSTANCE	hInstWLX;
	int			NTLevel;

	void Init(const HINSTANCE _hInst);
	~clsUlisterInstance();

	HINSTANCE ViewerLibraryInstanceInc();
	void ViewerLibraryInstanceDec(int _keepinmemory);

private:
	
	HINSTANCE	hViewerLibrary;
	int			numInstances;
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
HWND CreateListerWindow(HWND ParentWin);
VTWORD GetVTFileType(const wchar_t* FileToLoad);
bool IsVTFileTypeAllowed(const wchar_t* FileToLoad, const wchar_t* onlyload, const wchar_t* noload);
HBITMAP GetVTFilePreview(const wchar_t* FileToLoad, const int width, const int height);
bool LoadVTFile(HWND hViewWnd, const wchar_t* FileToLoad);
void ErrMsgIssue(const int issuetype, const wchar_t *path, const DWORD dwError);
unsigned long long REGCurrentBuildNumber();
void SendVTOptions(const ALLMYDATA *mydata, const clsVTOptions *_VTOptions);
//void SetSccdisplayChildWndProc(HWND waWnd);
LRESULT CALLBACK SccdisplayWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
