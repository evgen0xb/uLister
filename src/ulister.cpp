/* 
The plugin is provided as-is and without any warranty under the GPLv3 license.
*/

#include <windows.h>
#include "total.h"

#include "ulister.h"
#include "config.h"
#include "utils.h"
#include "window.h"
#include "sharedinstance.h"

/*
                    WLX
		+-------------------------------------------+
        |  clsSharedPluginInstance                  |
        |                                           |
        |   +----------------------------------+    |
        |   |  clsUlisterInstance              |    |
		|   +----------------------------------+    |
        |                                           |
		|   +----------------------------------+    |
		|   |  clsUlisterOptions               |    |
		|   +----------------------------------+    |
        |                                           |
        |   +----------------------------------+    |
        |   |  clsVTOptions                    |    |
		|   |                                  |    |
        |   |   +-------------------------+    |    |
        |   |   |  clsVTOptionsClipboard  |    |    |
        |   |   +-------------------------+    |    |
        |   |   +-------------------------+    |    |
        |   |   |  clsVTOptionsViewer     |    |    |
        |   |   +-------------------------+    |    |
		|   +----------------------------------+    |
		+-------------------------------------------+
                                           ^    ^
                                           |    |
                                           |    |                        WINDOW 1
                                           |    |                +----------------------------------+
                                           |    |                |   clsVTWindowInstance            |
                                           |    |                |                                  |
                                           |    |                |   +-------------------------+    |
                                           |    |                |   |  clsToolTip             |    |
                                           |    |                |   +-------------------------+    |
                                           |    |                |   +-------------------------+    |
                                           |    |                |   |  clsInfoWindow          |    |
                                           |    |                |   +-------------------------+    |
                                           |    |                |   +-------------------------+    |
                                           |    |                |   |  clsLoadedFileInfo      |    |
                                           |    |                |   +-------------------------+    |
                                           |    |                |                                  |
                                           |    |                |                                  |
                                           |    |                |   +-------------------------+    |
                                           |    +--------------------- *clsSharedPluginInstance|    |
                                           |                     |   +-------------------------+    |
                                           |                     +----------------------------------+
                                           |
                                           |                             WINDOW 2
                                           |                     +----------------------------------+
                                           |                     |                                  |
                                           |                     |                                  |
                                           |                     |                                  |
                                           |                     |                                  |
                                           +--------------------------                              |
                                                                 |                                  |
                                                                 +----------------------------------+

*/

clsSharedPluginInstance g_SharedPluginInstance;



// Use [same] _wcsicmp [etc] everyweare to optimize .text dll-section memory usage



///////////////////////////////////////////////////////////////////////////////////////////////////



BOOL APIENTRY DllMain(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:

#if defined (__ULISTDEBUGMSG)
		OutputDebugStringA("ULISTER::DLL_PROCESS_ATTACH");
#endif
		g_SharedPluginInstance.InitPlugin(hinst);
		break;

	case DLL_PROCESS_DETACH:

#if defined (__ULISTDEBUGMSG)
		OutputDebugStringA("ULISTER::DLL_PROCESS_DETACH");
#endif
		// < + CALL g_SharedPluginInstance::UlisterInstance::~clsUlisterInstance(); >

		break;
	}
	return TRUE;
} // DllMain



///////////////////////////////////////////////////////////////////////////////////////////////////



extern "C" __declspec(dllexport) HWND __stdcall ListLoadW(HWND ParentWin, wchar_t* FileToLoad, int ShowFlags)
{
#if defined (__ULISTDEBUGMSG)
	OutputDebugStringW(L"ListLoadW");
#endif
	clsVTWindowInstance *mydata = new clsVTWindowInstance();
	if (!mydata) return NULL;

	if (mydata->VTLoad(ParentWin, FileToLoad, &g_SharedPluginInstance)) return mydata->waWindow;
	else { delete mydata; return NULL; }

} // ListLoadW



///////////////////////////////////////////////////////////////////////////////////////////////////



extern "C" __declspec(dllexport) HWND __stdcall ListLoad(HWND ParentWin, char* FileToLoad, int ShowFlags)
{
	wchar_t path[MAX_PATH] = L"";
	MultiByteToWideChar(CP_ACP, 0, FileToLoad, -1, path, MAX_PATH);
	return ListLoadW(ParentWin, path, ShowFlags);
} // ListLoad



///////////////////////////////////////////////////////////////////////////////////////////////////



extern "C" __declspec(dllexport) int __stdcall ListLoadNextW(HWND ParentWin, HWND ListWin, wchar_t* FileToLoad, int ShowFlags)
{
#if defined (__ULISTDEBUGMSG)
	OutputDebugStringW(L"ListLoadNextW");
#endif
	clsVTWindowInstance *mydata = (clsVTWindowInstance *)GetWindowLongPtr(ListWin, GWLP_USERDATA);
	if (!mydata) return LISTPLUGIN_ERROR; // error return cause auto call ListCloseWindow()

	DestroyWindow(mydata->InfoWindow.hwndFileInfo); mydata->InfoWindow.Done();
	mydata->ToolTip.DestroyTemporaryMessage();

	if (mydata->VTLoad(ParentWin, FileToLoad, &g_SharedPluginInstance)) return LISTPLUGIN_OK;
	else return LISTPLUGIN_ERROR; // error return cause auto call ListCloseWindow()

	// TC SDK:
	// Return LISTPLUGIN_OK if load succeeds, LISTPLUGIN_ERROR otherwise.
	// FAKE: If LISTPLUGIN_ERROR is returned, Lister will try to load the file with the normal ListLoad function
	// (also with other plugins). -- Lister will call ListCloseWindow() first!

} // ListLoadNextW



///////////////////////////////////////////////////////////////////////////////////////////////////



extern "C" __declspec(dllexport) int __stdcall ListLoadNext(HWND ParentWin, HWND ListWin, char* FileToLoad, int ShowFlags)
{
	wchar_t path[MAX_PATH] = L"";
	MultiByteToWideChar(CP_ACP, 0, FileToLoad, -1, path, MAX_PATH);
	return ListLoadNextW(ParentWin, ListWin, path, ShowFlags);
} // ListLoadNext



///////////////////////////////////////////////////////////////////////////////////////////////////



extern "C" __declspec(dllexport)void __stdcall ListCloseWindow(HWND ListWin)
{
#if defined (__ULISTDEBUGMSG)
	std::wstring msgW = L"ListCloseWindow (ListWin=" + ToHexW(ListWin) + L")";
	OutputDebugStringW(msgW.c_str());
#endif

	// TC SDK:
	// FAKE: ListCloseWindow is called when a user closes lister, or loads a different file.
	// If ListCloseWindow isn't present, DestroyWindow() is called.

	if (IsWindow(ListWin))
	{
		clsVTWindowInstance *mydata = (clsVTWindowInstance *)GetWindowLongPtr(ListWin, GWLP_USERDATA);
		if (mydata)
		{
			mydata->VTUnload();
			delete mydata;
		}
	}
} // ListCloseWindow



///////////////////////////////////////////////////////////////////////////////////////////////////



extern "C" __declspec(dllexport)int __stdcall ListSearchText(HWND ListWin, char* SearchString, int SearchParameter) // ASCII
{
	// reserved for Windows 98 SE future support maybe

	// Note. ListSearchTextW never called with SearchStringW="\0" (i.e. empty search string)

	clsVTWindowInstance *mydata = (clsVTWindowInstance *)GetWindowLongPtrW(ListWin, GWLP_USERDATA);
	if (mydata) mydata->ListSearchTextAHandler(SearchString, SearchParameter);

	return LISTPLUGIN_OK;
} // ListSearchText



///////////////////////////////////////////////////////////////////////////////////////////////////



extern "C" __declspec(dllexport)int __stdcall ListSearchTextW(HWND ListWin, WCHAR* SearchStringW, int SearchParameter) // UTF16
{
	// Note. ListSearchTextW never called with SearchStringW="\0" (i.e. empty search string)

	clsVTWindowInstance *mydata = (clsVTWindowInstance *)GetWindowLongPtrW(ListWin, GWLP_USERDATA);
	if (mydata) mydata->ListSearchTextWHandler(SearchStringW, SearchParameter);

	return LISTPLUGIN_OK;
} // ListSearchTextW



///////////////////////////////////////////////////////////////////////////////////////////////////



extern "C" __declspec(dllexport)int __stdcall ListPrint(HWND ListWin, char* FileToPrint, char* DefPrinter, int PrintFlags, RECT* Margins)
{
	clsVTWindowInstance *mydata;
	mydata = (clsVTWindowInstance *)GetWindowLongPtr(ListWin, GWLP_USERDATA);
	if (mydata)
		SendMessage(mydata->SccviewerWindow, SCCVW_PRINT, 0, 0);
	return LISTPLUGIN_OK;
} // ListPrint



///////////////////////////////////////////////////////////////////////////////////////////////////



extern "C" __declspec(dllexport)int __stdcall ListSendCommand(HWND ListWin, int Command, int Parameter)
{
	clsVTWindowInstance *mydata;
	mydata = (clsVTWindowInstance *)GetWindowLongPtr(ListWin, GWLP_USERDATA);
	if (mydata)
		switch (Command) {
		case lc_copy:
			SendMessage(mydata->SccviewerWindow, SCCVW_COPYTOCLIP, 0, 0);
			return LISTPLUGIN_OK;
		case lc_selectall:
			SendMessage(mydata->SccviewerWindow, SCCVW_SELECTALL, 0, 0);
			return LISTPLUGIN_OK;
		}
	return LISTPLUGIN_ERROR;
} // ListSendCommand



///////////////////////////////////////////////////////////////////////////////////////////////////



extern "C" __declspec(dllexport)HBITMAP __stdcall ListGetPreviewBitmapW(wchar_t* FileToLoad, int width, int height, char* contentbuf, int contentbuflen)
{
	__VTTYPENAMEBUF pTypeName;
	VTWORD wType = g_SharedPluginInstance.GetVTFileType(FileToLoad, pTypeName);
	if (!g_SharedPluginInstance.IsVTFileTypeAllowed(wType, true)) return NULL;
	HBITMAP bitmap = g_SharedPluginInstance.GetVTFilePreview(FileToLoad, width, height);
	return bitmap;
} // ListGetPreviewBitmapW



///////////////////////////////////////////////////////////////////////////////////////////////////



extern "C" __declspec(dllexport)HBITMAP __stdcall ListGetPreviewBitmap(char* FileToLoad, int width, int height, char* contentbuf, int contentbuflen)
{
	wchar_t path[MAX_PATH] = L"";
	MultiByteToWideChar(CP_ACP, 0, FileToLoad, -1, path, MAX_PATH);
	return ListGetPreviewBitmapW(path, width, height, contentbuf, contentbuflen);
} // ListGetPreviewBitmap



///////////////////////////////////////////////////////////////////////////////////////////////////



clsLoadedFileInfo::clsLoadedFileInfo() { pPath = NULL; pTypeName = NULL; }

clsLoadedFileInfo::~clsLoadedFileInfo() { if (pPath) free(pPath); if (pTypeName) free(pTypeName); }

void clsLoadedFileInfo::Init(LPCWSTR _pPath, const VTWORD _wType, LPCSTR _pTypeName, const bool quickview)
{
	pPath = _wcsdup(_pPath);
	wType = _wType;
	pTypeName = _strdup(_pTypeName);
	isQuickviewMode = quickview;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
