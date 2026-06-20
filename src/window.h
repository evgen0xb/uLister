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
	//clsUlisterInstance *pUlisterInstance;
	//clsUlisterOptions *pUlisterOptions;
	//clsVTOptions *pVTOptions;
	// TODO ------> clsSharedPluginInstance
	clsSharedPluginInstance *pSharedPluginInstance;


	clsVTWindowInstance();

	bool VTLoad(const HWND ParentWin, const wchar_t *FileToLoad, clsSharedPluginInstance *_pSharedPluginInstance);
	void VTUnload();

	void SendVTOptions();
	void ChangeViewMode(const int dir);
	void ZoomBitmapVecFont(const int dir);

	VTDWORD GetDisplayEngineVT();
	static wchar_t* DisplayEngineName(const VTDWORD dwType);

private:

	bool CreateListerWindow(const HWND ParentWin, const HINSTANCE hInst);
	void AddFileInfo();

};

#endif
