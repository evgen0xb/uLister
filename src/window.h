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

	clsToolTip ToolTip;

	clsLoadedFileInfo LoadedFileInfo;
	clsInfoWindow InfoWindow;

	// Common for all Viewer Technology windows:
	clsSharedPluginInstance *pSharedPluginInstance;

	bool isFullScreenMode;

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

	void ChangeFullScrMode();

private:

	bool CreateListerWindow(const HWND ParentWin, const HINSTANCE hInst);
	void AddFileInfo();

	wchar_t WindSearchStrW[VTMAXSEARCHBUF];
	char WindSearchStrA[VTMAXSEARCHBUF];

	void SetVTSearchUnicode();
	void SetVTSearchANSI();

	static HWND FindButtonN(const HWND hParent, const int targetIndex);

	HMENU FindSubMenuByName(const HMENU hMenu, const wchar_t* targetName, const bool isSubMenu);

	// full screen:
	void EnterToFullScreenMode();
	void ExitFromFullScrMode_QuickView();
	void ExitFromFullScrMode_Orig();

	RECT rectFSOrigPosition;
	HWND hWndFSOrigParent;
	LONG lngFSOrigWindowStyle;
	HMENU hFSOrigMenu;
};

#endif
