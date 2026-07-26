/*
The plugin is provided as-is and without any warranty under the GPLv3 license.
*/

#ifndef ULISTERWINDOW
#define ULISTERWINDOW

// VS 2005 FIX
#define WM_MOUSEHWHEEL                  0x020E
#define WM_MOUSEWHEEL                   0x020A
#define GET_WHEEL_DELTA_WPARAM(wParam)  ((short)HIWORD(wParam))

#include <windows.h>
#include <sccvw.h>



#include "ulister.h"
#include "infowindow.h"
#include "tooltip.h"
#include "sharedinstance.h"
#include "utils.h"



class clsFullScreen
{
public:

	clsFullScreen();
	void Init(HWND _hWnd);
	void EnterToFullScrMode();
	void ExitFromFullScrMode();
	void ChangeFullScrMode();
	bool isFullScrEnabled();

private:
	void ExitFromFullScrMode_ChildWnd();
	void ExitFromFullScrMode_NormalWnd();

	RECT rectFSOrigPosition;
	HWND hWndFSOrigParent;
	LONG lngFSOrigWindowStyle;
	HMENU hFSOrigMenu;

	HWND hWnd;
	bool isOrigStyleChild;
};



//#define VTMAXSEARCHBUF 80 // don't change!
#define VTMAXSEARCHBUF (member_size(SCCVWSEARCHINFO80, siText) / member_size(SCCVWSEARCHINFO80, siText[0]))



class clsVTWindowInstance
{
public:

	HWND TListerWindow;
	WNDPROC OriginalTListerWindowProc;

	HWND waWindow; // WTF-naming

	WNDPROC OriginalSccviewerWindowProc;
	HWND SccviewerWindow;

	WNDPROC OriginalSccdisplayWindowProc;
	HWND SccdisplayWindow;

	HWND SccscrollHorizWindow;

	clsFullScreen FullScreen;

	clsToolTip ToolTip;

	clsLoadedFileInfo LoadedFileInfo;
	clsInfoWindow InfoWindow;

	// Common for all Viewer Technology windows:
	clsSharedPluginInstance *pSharedPluginInstance;

	clsVTWindowInstance();

	bool VTLoad(const HWND ParentWin, const wchar_t *FileToLoad, clsSharedPluginInstance *_pSharedPluginInstance);
	void VTUnload();

	void ChangeViewMode(const int dir);
	void ZoomBitmapVecFont(const int dir);

	VTDWORD GetDisplayEngineVT();
	static wchar_t* DisplayEngineName(const VTDWORD dwType);

	void ListSearchTextWHandler(const WCHAR* SearchStringW, const int SearchParameter);
	void ListSearchTextAHandler(const char* SearchString, const int SearchParameter);

	void OEM_ReplaceButtonFROMBEGINING(const HWND hwndPrevious);
	void OEM_AddNewContextMenuItems();

	bool isDragnDropEnabled();
	void SetDragnDrop(bool enable);

	void EnableScrollBar(bool enable);
	bool isScrollBarEnabled();

	HWND FindHorizontalVTScrollbar(HWND hParentWnd);
	bool IsCursorOverHorizVTScrollbar();

private:

	bool CreateListerWindow(const HWND ParentWin, const HINSTANCE hInst);
	void AddFileInfo();

	wchar_t WindSearchStrW[VTMAXSEARCHBUF];
	char WindSearchStrA[VTMAXSEARCHBUF];

	void SetVTSearchUnicode();
	void SetVTSearchANSI();

	static HWND FindButtonN(const HWND hParent, const int targetIndex);
	static HMENU FindSubMenuByName(const HMENU hMenu, const wchar_t* targetName, const bool isSubMenu);

public:

	bool winkeyhack;

	// if multiple files loaded or quick view mode; until VTUnload():
	// VTLoad -> VTLoad -> VTLoad -> ... -> VTLoad -> {VTUnload}
	bool isfirstvtload;
};

#endif
