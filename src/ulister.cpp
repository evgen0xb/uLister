/* 
The plugin is provided as-is and without any warranty under the GPLv3 license.
*/

#include <windows.h>
#include <stdio.h>
#include <map>
#include "ulister.h"
#include "total.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
clsUlisterInstance	UlisterInstance;
clsUlisterOptions	UlisterOptions;
clsVTOptions		VTOptions;

const char *ANOTFOUND = "Not found:";
const wchar_t *WNOTFOUND = L"Not found:";
const int MAXSEARCH = VTMAXSEARCHBUF - 1;

// You need to remember the search string for each uLister window in order to correctly display the message about what exactly was NOT found,
// since the FORWARD/BACK search string is independent in each window.
std::map<HWND, char*> SearchStringPerWindowA;		// SearchStringW + SearchStringA + SearchParameter - into ALLMYDATA (TODO)
std::map<HWND, wchar_t*> SearchStringPerWindowW;

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

		UlisterInstance.Init(hinst);
		IniParse();

		break;

	case DLL_PROCESS_DETACH:

#if defined (__ULISTDEBUGMSG)
		OutputDebugStringA("ULISTER::DLL_PROCESS_DETACH");
#endif

		std::map<HWND, wchar_t*>::iterator itW; // VS2005 fix
		for (itW = SearchStringPerWindowW.begin(); itW != SearchStringPerWindowW.end(); ++itW)
			free(itW->second);

		std::map<HWND, char*>::iterator itA; // VS2005 fix
		for (itA = SearchStringPerWindowA.begin(); itA != SearchStringPerWindowA.end(); ++itA)
			free(itA->second);

		// < + CALL UlisterInstance::~clsUlisterInstance(); >

		break;
	}
	return TRUE;
} // DllMain



///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) HWND __stdcall ListLoadW(HWND ParentWin, wchar_t* FileToLoad, int ShowFlags)
{

#if defined (__ULISTDEBUGMSG)
	std::wstring msgW = L"ListLoadW (" + std::wstring(FileToLoad) + L", ParentWin=" + ToHexW(ParentWin) + L")";
	OutputDebugStringW(msgW.c_str());
#endif

	__VTTYPENAMEBUF pTypeName;
	VTWORD wType = GetVTFileType(FileToLoad, pTypeName);

	if (!IsVTFileTypeAllowed(wType, UlisterOptions.inionlyloadtypes, UlisterOptions.ininoloadtypes))
	{
#if defined (__ULISTDEBUGMSG)
		OutputDebugStringW(L"ListLoadW := NULL; // IsVTFileTypeAllowed");
#endif
		return NULL;
	}

	if (!UlisterInstance.ViewerLibraryInstanceInc()) return NULL;

	HWND hViewWnd = CreateListerWindow(ParentWin);
	if (!IsWindow(hViewWnd)) return NULL;

	ALLMYDATA *mydata = (ALLMYDATA *)GetWindowLongPtr(hViewWnd, GWLP_USERDATA);
	if (mydata)
	{
		if (!LoadVTFile(mydata->SccviewerWindow, FileToLoad))
		{
			// TC SDK:
			// Return a handle to your window if load succeeds, NULL otherwise. If NULL is returned, Lister will try the next plugin.

			ListCloseWindow(hViewWnd);
#if defined (__ULISTDEBUGMSG)
			OutputDebugStringW(L"ListLoadW := NULL; // LoadVTFile");
#endif
			return NULL;
		}

		mydata->LoadedFileInfo.Init(FileToLoad, wType, pTypeName);
		SendVTOptions(mydata, &VTOptions);
		AddFileInfo(mydata);

		//OutputDebugStringW(mydata->LoadedFileInfo.pPath);
		//OutputDebugStringA(mydata->LoadedFileInfo.pTypeName);

		//SetSccdisplayChildWndProc(hViewWnd);
	}
	else
	{
		return NULL;
	}

#if defined (__ULISTDEBUGMSG)
	msgW = L"ListLoadW := (HWND) " + ToHexW(hViewWnd) + L";";
	OutputDebugStringW(msgW.c_str());
#endif
	return hViewWnd;
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
	std::wstring msgW = L"ListLoadNextW (" + std::wstring(FileToLoad) + L", ParentWin=" + ToHexW(ParentWin) +
		L", ListWin=" + ToHexW(ListWin) + L")";
	OutputDebugStringW(msgW.c_str());
#endif

	__VTTYPENAMEBUF pTypeName;
	VTWORD wType = GetVTFileType(FileToLoad, pTypeName);

	if (!IsVTFileTypeAllowed(wType, UlisterOptions.inionlyloadtypes, UlisterOptions.ininoloadtypes))
	{
#if defined (__ULISTDEBUGMSG)
		OutputDebugStringW(L"ListLoadNextW := LISTPLUGIN_ERROR; // IsVTFileTypeAllowed");
#endif
		return LISTPLUGIN_ERROR; // error return cause auto call ListCloseWindow()
	}

	ALLMYDATA *mydata = (ALLMYDATA *)GetWindowLongPtr(ListWin, GWLP_USERDATA);
	if (mydata)
	{
		// TC SDK:
		// Return LISTPLUGIN_OK if load succeeds, LISTPLUGIN_ERROR otherwise.
		// FAKE: If LISTPLUGIN_ERROR is returned, Lister will try to load the file with the normal ListLoad function
		// (also with other plugins). -- Lister will call ListCloseWindow() first!

		if (!LoadVTFile(mydata->SccviewerWindow, FileToLoad))
		{
#if defined (__ULISTDEBUGMSG)
			OutputDebugStringW(L"ListLoadNextW := LISTPLUGIN_ERROR; // LoadVTFile");
#endif
			return LISTPLUGIN_ERROR; // error return cause auto call ListCloseWindow()
		}

		mydata->LoadedFileInfo.Init(FileToLoad, wType, pTypeName);
		SendVTOptions(mydata, &VTOptions);
		AddFileInfo(mydata);

		//SetSccdisplayChildWndProc(mydata->waWindow);
	}
	else return LISTPLUGIN_ERROR; // actually, mydata is always valid in ListLoadNextW()

#if defined (__ULISTDEBUGMSG)
	OutputDebugStringW(L"ListLoadNextW := LISTPLUGIN_OK;");
#endif
	return LISTPLUGIN_OK;
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
		ALLMYDATA *mydata;
		mydata = (ALLMYDATA *)GetWindowLongPtr(ListWin, GWLP_USERDATA);
		if (mydata)
		{
			SendMessage(mydata->SccviewerWindow, SCCVW_SAVEOPTIONS, 0, 0L);
			SendMessage(mydata->SccviewerWindow, SCCVW_CLOSEFILE, 0, 0L);
			DestroyWindow(mydata->SccviewerWindow);
			DestroyWindow(mydata->waWindow);

			mydata->BalloonTip.DestroyTemporaryMessage();

			// WARNING!
			// Calling ListCloseWindow doesn't necessarily mean the parent window will be "closed".
			// It's necessary to return the original address of the window procedure to the parent window, since it may be used later by another plugin.
			// TODO: ALLMYDATA struct->class with Load/Unload members?
			if (mydata->OriginalTListerWindowProc) SetWindowLongPtr(mydata->TListerWindow, GWLP_WNDPROC, (LONG_PTR)mydata->OriginalTListerWindowProc);

			UlisterInstance.ViewerLibraryInstanceDec(UlisterOptions.keepinmemory);

			delete mydata;

			if (SearchStringPerWindowW.count(ListWin) != 0)
			{
				free(SearchStringPerWindowW[ListWin]);
				SearchStringPerWindowW.erase(ListWin);
			}
			if (SearchStringPerWindowA.count(ListWin) != 0)
			{
				free(SearchStringPerWindowA[ListWin]);
				SearchStringPerWindowA.erase(ListWin);
			}


		}
	}
} // ListCloseWindow



///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport)int __stdcall ListSearchText(HWND ListWin, char* SearchString, int SearchParameter) { // ASCII
// reserved for Windows 98 SE future support maybe
#pragma warning( push )
#pragma warning( disable : 4996 )

	bool WindowWithoutSearchStringYet;
	char *WindSearchStr;
	ALLMYDATA *mydata;

	mydata = (ALLMYDATA *)GetWindowLongPtr(ListWin, GWLP_USERDATA);
	if (mydata) {

		// force internal search engine to ASCII (default)
		SCCVWOPTIONSPEC40 locOptionSpec;
		VTDWORD SystemFlags;
		locOptionSpec.dwSize = sizeof(SCCVWOPTIONSPEC40);
		locOptionSpec.dwFlags = SCCVWOPTION_CURRENT;
		locOptionSpec.dwId = SCCID_SYSTEMFLAGS;
		locOptionSpec.pData = &SystemFlags;
		SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
		SystemFlags = SystemFlags & (~SCCVW_SYSTEM_UNICODE); // reset the unicode bit
		SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

		WindowWithoutSearchStringYet = (SearchStringPerWindowA.count(ListWin) == 0);

		if ((SearchParameter & lcs_findfirst) || WindowWithoutSearchStringYet) {
			if (WindowWithoutSearchStringYet) // new search string
			{
				// VS2005 fix WindSearchStr = (char *)malloc(sizeof(SCCVWSEARCHINFO40::siText));
				WindSearchStr = (char *)malloc(member_size(SCCVWSEARCHINFO40, siText));
				strncpy(WindSearchStr, SearchString, MAXSEARCH);
				WindSearchStr[MAXSEARCH] = 0; // fix potencial overflow
				SearchStringPerWindowA[ListWin] = WindSearchStr;
			}
			else // update search string
			{
				WindSearchStr = SearchStringPerWindowA[ListWin];
				strncpy(WindSearchStr, SearchString, MAXSEARCH);
				WindSearchStr[MAXSEARCH] = 0; // fix potencial overflow
			}
		}
		else // get search string
		{
			WindSearchStr = SearchStringPerWindowA[ListWin];
		}

		if ((SearchParameter & lcs_findfirst) || WindowWithoutSearchStringYet) {

			SCCVWSEARCHINFO40 locSearchInfo40;

			locSearchInfo40.dwSize = sizeof(SCCVWSEARCHINFO40);
			// original used strncpy, supress error about unsafe strcopy and using strncpy_s instead 
			strncpy(locSearchInfo40.siText, WindSearchStr, MAXSEARCH);
			locSearchInfo40.siText[MAXSEARCH] = 0;
			locSearchInfo40.siTextLen = (VTWORD) strlen(locSearchInfo40.siText);

			locSearchInfo40.siType = (SearchParameter & lcs_matchcase) ? SCCVW_SEARCHCASE : SCCVW_SEARCHNOCASE;
			locSearchInfo40.siFrom = SCCVW_SEARCHCURRENT;
			locSearchInfo40.siDirection = (SearchParameter & lcs_backwards) ? SCCVW_SEARCHBACK : SCCVW_SEARCHFORWARD;

			//Updated for find to work
			//if(SendMessage(mydata->oiWindow,SCCVW_SEARCH,0,(LPARAM)(PSCCVWSEARCHINFO40)&locSearchInfo)!=0)
			if (SendMessage(mydata->SccviewerWindow, SCCVW_SEARCH, 0, (LPARAM)(PSCCVWSEARCHINFO80)&locSearchInfo40) != 0) // hack!
				MessageBox(mydata->SccviewerWindow, WindSearchStr, ANOTFOUND, MB_OK);
		}
		else
			if (SearchParameter & lcs_backwards) {
				if (SendMessage(mydata->SccviewerWindow, SCCVW_SEARCHNEXT, SCCVW_SEARCHBACK, 0) != 0)
					MessageBox(mydata->SccviewerWindow, WindSearchStr, ANOTFOUND, MB_OK);
			}
			else
				if (SendMessage(mydata->SccviewerWindow, SCCVW_SEARCHNEXT, SCCVW_SEARCHFORWARD, 0) != 0)
					MessageBox(mydata->SccviewerWindow, WindSearchStr, ANOTFOUND, MB_OK);
	}
	return LISTPLUGIN_OK;
#pragma warning( pop ) 
} // ListSearchText



///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport)int __stdcall ListSearchTextW(HWND ListWin, WCHAR* SearchStringW, int SearchParameter) { // UTF16
#pragma warning( push )
#pragma warning( disable : 4996 )

	bool WindowWithoutSearchStringYet;
	wchar_t *WindSearchStr;
	ALLMYDATA *mydata;
	
	mydata = (ALLMYDATA *) GetWindowLongPtrW(ListWin, GWLP_USERDATA);
	if (mydata) {
		
		// force internal search engine to UNICODE
		SCCVWOPTIONSPEC40 locOptionSpec;
		VTDWORD SystemFlags;
		locOptionSpec.dwSize = sizeof(SCCVWOPTIONSPEC40);
		locOptionSpec.dwFlags = SCCVWOPTION_CURRENT;
		locOptionSpec.dwId = SCCID_SYSTEMFLAGS;
		locOptionSpec.pData = &SystemFlags;
		SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
		SystemFlags = SystemFlags | SCCVW_SYSTEM_UNICODE; // set the unicode bit
		SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

		WindowWithoutSearchStringYet = (SearchStringPerWindowW.count(ListWin) == 0);

		if ((SearchParameter & lcs_findfirst) || WindowWithoutSearchStringYet) {
			if (WindowWithoutSearchStringYet) // new search string
			{
				// VS2005 fix WindSearchStr = (wchar_t *) malloc(sizeof(SCCVWSEARCHINFO80::siText));
				WindSearchStr = (wchar_t *) malloc(member_size(SCCVWSEARCHINFO80, siText));
				wcsncpy(WindSearchStr, SearchStringW, MAXSEARCH);
				WindSearchStr[MAXSEARCH] = 0; // fix potencial overflow
				SearchStringPerWindowW[ListWin] = WindSearchStr;
			}
			else // update search string
			{
				WindSearchStr = SearchStringPerWindowW[ListWin];
				wcsncpy(WindSearchStr, SearchStringW, MAXSEARCH);
				WindSearchStr[MAXSEARCH] = 0; // fix potencial overflow
			}
		}
		else // get search string
		{
			WindSearchStr = SearchStringPerWindowW[ListWin];
		}

		if ((SearchParameter & lcs_findfirst) || WindowWithoutSearchStringYet) {

			SCCVWSEARCHINFO80 locSearchInfo80;

			locSearchInfo80.dwSize = sizeof(SCCVWSEARCHINFO80);
			wcsncpy((wchar_t*)locSearchInfo80.siText, WindSearchStr, MAXSEARCH);
			locSearchInfo80.siText[MAXSEARCH] = 0;
			locSearchInfo80.siTextLen = (VTWORD) wcslen((wchar_t*)locSearchInfo80.siText);

			locSearchInfo80.siType = (SearchParameter & lcs_matchcase) ? SCCVW_SEARCHCASE : SCCVW_SEARCHNOCASE;
			locSearchInfo80.siFrom = SCCVW_SEARCHCURRENT;
			locSearchInfo80.siDirection = (SearchParameter & lcs_backwards) ? SCCVW_SEARCHBACK : SCCVW_SEARCHFORWARD;

			if (SendMessageW(mydata->SccviewerWindow, SCCVW_SEARCH, 0, (LPARAM)(PSCCVWSEARCHINFO80)&locSearchInfo80) != 0)
				MessageBoxW(mydata->SccviewerWindow, WindSearchStr, WNOTFOUND, MB_OK);
		}
		else
			if (SearchParameter & lcs_backwards) {
				if (SendMessageW(mydata->SccviewerWindow, SCCVW_SEARCHNEXT, SCCVW_SEARCHBACK, 0) != 0)
					MessageBoxW(mydata->SccviewerWindow, WindSearchStr, WNOTFOUND, MB_OK);
			}
			else
				if (SendMessageW(mydata->SccviewerWindow, SCCVW_SEARCHNEXT, SCCVW_SEARCHFORWARD, 0) != 0)
					MessageBoxW(mydata->SccviewerWindow, WindSearchStr, WNOTFOUND, MB_OK);
	}
	return LISTPLUGIN_OK;
#pragma warning( pop ) 
} // ListSearchTextW



///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport)int __stdcall ListPrint(HWND ListWin, char* FileToPrint, char* DefPrinter, int PrintFlags, RECT* Margins)
{
	ALLMYDATA *mydata;
	mydata = (ALLMYDATA *)GetWindowLongPtr(ListWin, GWLP_USERDATA);
	if (mydata)
		SendMessage(mydata->SccviewerWindow, SCCVW_PRINT, 0, 0);
	return LISTPLUGIN_OK;
} // ListPrint



///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport)int __stdcall ListSendCommand(HWND ListWin, int Command, int Parameter)
{
	ALLMYDATA *mydata;
	mydata = (ALLMYDATA *)GetWindowLongPtr(ListWin, GWLP_USERDATA);
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
	VTWORD wType = GetVTFileType(FileToLoad, pTypeName);
	if (!IsVTFileTypeAllowed(wType, UlisterOptions.inionlypreviewtypes, UlisterOptions.ininopreviewtypes)) return NULL;
	HBITMAP bitmap = GetVTFilePreview(FileToLoad, width, height);
	return bitmap;
} // ListGetPreviewBitmapW



///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport)HBITMAP __stdcall ListGetPreviewBitmap(char* FileToLoad, int width, int height, char* contentbuf, int contentbuflen)
{
	wchar_t path[MAX_PATH] = L"";
	MultiByteToWideChar(CP_ACP, 0, FileToLoad, -1, path, MAX_PATH);
	return ListGetPreviewBitmapW(path, width, height, contentbuf, contentbuflen);
} // ListGetPreviewBitmap
