/* 
The plugin is provided as-is and without any warranty under the GPLv3 license.
*/

#include <windows.h>
#include <stdio.h>
#include <CommCtrl.h>
#include <float.h>
#include <lomenu.h>
#include <sccvw.h>

#include "total.h"

#include "ulister.h"
#include "window.h"
#include "config.h"
#include "utils.h"
#include "infowindow.h"
#include "tooltip.h"
#include "sharedinstance.h"



const char *WNDCLASSNAME_WAWC			= "WAwc";
const char *WNDCLASSNAME_SCCVIEWER		= "SCCVIEWER";
const char *WNDCLASSNAME_SCCDISPLAY		= "SCCDISPLAY";



#define CLASSNAMEMAXBUF		64

// must be less then SCCVW_DEFAULTMENUMAX:
#define ID_CUSTOM_FULLSCR	101
#define ID_CUSTOM_FILEINF	102
#define ID_CUSTOM_DRGNDRP	103
#define ID_CUSTOM_FINDTXT	104
#define ID_CUSTOM_SCRLBAR	105

const wchar_t *WFULLSCR = L"Full Screen";
const wchar_t *WFILEINF = L"File Info";
const wchar_t *WOPTIONS = L"Options";
const wchar_t *WDRAGNDR = L"Drag'n'Drop";
const wchar_t *WFIND	= L"Find";
const wchar_t *WSCRLBAR = L"Scroll Bars";

const char *ANOTFOUND = "Not found:";
const wchar_t *WNOTFOUND = L"Not found:";
const int MAXSEARCH = VTMAXSEARCHBUF - 1;



LRESULT CALLBACK SccdisplayWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);



///////////////////////////////////////////////////////////////////////////////////////////////////



// This message is sent from the view window to the developer when another file should be viewed.
// Currently, this occurs is when the user double - clicks or hits return on a file entry in an Archive view and on a hyperlink to a referenced document.
// In the case of the archive formats, the display engine decompresses the file then sends a SCCVW_VIEWTHISFILE message to the developer.
DWORD ViewThisFileHandler(const LPARAM lParam) // 4.74 SCCVW_VIEWTHISFILE
#define ULISTMAXARGS (MAX_PATH * 2 + 15) // [/S=L "" /I=""] + ini_path + target_path + '\0'
{
	union
	{
		PSCCVWVIEWTHISFILE40    locVTFPtr40;
		PSCCVWVIEWTHISFILE80    locVTFPtr80;
	}; locVTFPtr40 = (PSCCVWVIEWTHISFILE40)lParam; // locVTFPtr80 = (PSCCVWVIEWTHISFILE80)lParam;

	if (locVTFPtr40->sViewFile.dwSpecType == IOTYPE_ANSIPATH)
	{
		char TotalcmdExePath[MAX_PATH];
		char TotalcmdIniPath[MAX_PATH];
		char TotalArgs[ULISTMAXARGS];

		if (!GetModuleFileNameA(NULL, TotalcmdExePath, MAX_PATH)) return ERROR_EXE_MARKED_INVALID;
		TotalcmdExePath[MAX_PATH - 1] = '\0'; // Windows XP fix: The string is truncated to nSize characters and is not null-terminated
		if (!GetEnvironmentVariableA("COMMANDER_INI", TotalcmdIniPath, MAX_PATH)) return GetLastError();

		TotalArgs[0] = '\0';
		strcat_s(TotalArgs, ULISTMAXARGS, "/S=L \""); // switch to run TC as lister mode
		strcat_s(TotalArgs, ULISTMAXARGS, (char *)locVTFPtr40->sViewFile.pSpec); // path to the file to view
		strcat_s(TotalArgs, ULISTMAXARGS, "\" /I=\"");
		strcat_s(TotalArgs, ULISTMAXARGS, TotalcmdIniPath); // current TC ini file
		strcat_s(TotalArgs, ULISTMAXARGS, "\"");

		ShellExecuteA(NULL, "open", TotalcmdExePath, TotalArgs, NULL, SW_RESTORE);
	}
	else if (locVTFPtr80->sViewFile.dwSpecType == IOTYPE_UNICODEPATH)
	{
		wchar_t TotalcmdExePath[MAX_PATH];
		wchar_t TotalcmdIniPath[MAX_PATH];
		wchar_t TotalArgs[ULISTMAXARGS];

		if (!GetModuleFileNameW(NULL, TotalcmdExePath, MAX_PATH)) return ERROR_EXE_MARKED_INVALID;
		TotalcmdExePath[MAX_PATH - 1] = L'\0'; // Windows XP fix: The string is truncated to nSize characters and is not null-terminated
		if (!GetEnvironmentVariableW(L"COMMANDER_INI", TotalcmdIniPath, MAX_PATH)) return GetLastError();

		TotalArgs[0] = L'\0';
		wcscat_s(TotalArgs, ULISTMAXARGS, L"/S=L \""); // switch to run TC as lister mode
		wcscat_s(TotalArgs, ULISTMAXARGS, (wchar_t *)locVTFPtr80->sViewFile.pSpec); // path to the file to view
		wcscat_s(TotalArgs, ULISTMAXARGS, L"\" /I=\"");
		wcscat_s(TotalArgs, ULISTMAXARGS, TotalcmdIniPath); // current TC ini file
		wcscat_s(TotalArgs, ULISTMAXARGS, L"\"");

		ShellExecuteW(NULL, L"open", TotalcmdExePath, TotalArgs, NULL, SW_RESTORE);
	}
	else return ERROR_BAD_LENGTH;

	return GetLastError();
}



///////////////////////////////////////////////////////////////////////////////////////////////////



LRESULT CALLBACK TListerWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	_control87(MCW_EM, MCW_EM);
	clsVTWindowInstance *mydata;
	mydata = (clsVTWindowInstance *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if (mydata)
	{
		switch (message)
		{
		case WM_SIZE:
			//OutputDebugStringA("WM_SIZE");
			mydata->ToolTip.Move();
			break;
		case WM_MOVE:
			//OutputDebugStringA("WM_MOVE");
			mydata->ToolTip.Move();
			break;
		case WM_TIMER:
			//OutputDebugStringA("WM_TIMER");
			if (wParam == mydata->ToolTip.nIDEvent) mydata->ToolTip.DestroyTemporaryMessage();
			break;
		case WM_ACTIVATE:
			if (LOWORD(wParam) == WA_INACTIVE) mydata->OEM_ReplaceButtonFROMBEGINING(reinterpret_cast<HWND>(lParam)); // lParam===hwndPrevious
			break;
		}
		return CallWindowProc(mydata->OriginalTListerWindowProc, hWnd, message, wParam, lParam); // Ghisler Handler
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}



///////////////////////////////////////////////////////////////////////////////////////////////////



LRESULT CALLBACK WAwcWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	_control87(MCW_EM, MCW_EM);
	clsVTWindowInstance *mydata;
	mydata = (clsVTWindowInstance *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if (mydata)
		switch (message)
		{
		case WM_SETFOCUS:
			SetFocus(mydata->SccviewerWindow);
			break;
		case WM_SIZE:
			InvalidateRect(hWnd, NULL, 0);
			if (IsWindow(mydata->SccviewerWindow)) {
				MoveWindow(mydata->SccviewerWindow, 0, 0, LOWORD(lParam), HIWORD(lParam), true);
				ShowWindow(mydata->SccviewerWindow, SW_SHOW);
			}
			break;
		case SCCVW_VIEWTHISFILE:
			return (ViewThisFileHandler(lParam) == 0) ? SCCVWERR_MESSAGEHANDLED : 0;
			break;
		case SCCVW_CONTEXTMENU:
			mydata->OEM_AddNewContextMenuItems();
			break;
		}
	return DefWindowProc(hWnd, message, wParam, lParam);
}



///////////////////////////////////////////////////////////////////////////////////////////////////



LRESULT CALLBACK SccviewerWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND _SccdisplayWindow;
	WNDPROC _SccdisplayWindowProc;

	_control87(MCW_EM, MCW_EM);
	clsVTWindowInstance *mydata;
	mydata = (clsVTWindowInstance *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (mydata)
	{
		// ***** Internal OIT Handler (OriginalSccdisplayWindowProc?): ***** 
		// ALL TYPES: MOUSEHWHEEL, Up/Down [PgDn/PgUp] - Vertical Scroll
		// Vector, Bitmap: CTRL+MOUSEHWHEEL - Zoom In/Out
		// Database, Spreadsheet, Vector presentation sheet (like VSD): Ctrl+PgDn/Ctrl+PgUp - next/prev sheet
		// Vector presentation: MOUSEHWHEEL, PgDn/PgUp, Up/Down - next/prev slide
		// ... SHIFT + Cursor Keys - selection

		switch (message)
		{
		case SCCVW_KEYDOWN:
			if ((GetKeyState(VK_CONTROL) < 0) && ((lParam == VK_OEM_PLUS) || (lParam == VK_ADD))) { mydata->ZoomBitmapVecFont(UlisterZoom::ZIN); return 0; }
			if ((GetKeyState(VK_CONTROL) < 0) && ((lParam == VK_OEM_MINUS) || (lParam == VK_SUBTRACT))) { mydata->ZoomBitmapVecFont(UlisterZoom::ZOUT); return 0; }
			if ((GetKeyState(VK_CONTROL) < 0) && ((lParam == VK_MULTIPLY) || (lParam == '8'))) { mydata->ZoomBitmapVecFont(UlisterZoom::ZRESET); return 0; }

			if ((GetKeyState(VK_CONTROL) < 0) && (GetKeyState(VK_SHIFT) < 0) && (lParam == 'R'))
			{
				// OutputDebugStringA("Reload ini-file");
				bool _keepinmemory = mydata->pSharedPluginInstance->UlisterOptions.keepinmemory;
				mydata->pSharedPluginInstance->ReloadPluginOptions();
				mydata->pSharedPluginInstance->UlisterOptions.keepinmemory = _keepinmemory;
				mydata->pSharedPluginInstance->VTOptions.SendVTOptions(mydata->SccviewerWindow);

				mydata->ToolTip.InitPosition(mydata->TListerWindow, TOOLTIP_XOFFS, TOOLTIP_YOFFS, TOOLTIP_WIDTH, TOOLTIP_HEIGHT);
				mydata->ToolTip.ShowTemporaryMessage(L"Reload", mydata->pSharedPluginInstance->UlisterOptions.ToolTipTransparency, mydata->pSharedPluginInstance->UlisterOptions.ToolTipTimer);

				return 0;
			}

			if ((GetKeyState(VK_CONTROL) < 0) && (lParam == 'M'))
			{
				// OutputDebugStringA("Prev/Next View Mode");
				if (GetKeyState(VK_SHIFT) < 0) mydata->ChangeViewMode(UlisterNextMode::MPREV);
				else mydata->ChangeViewMode(UlisterNextMode::MNEXT);
				return 0;
			}
			if ((GetKeyState(VK_CONTROL) < 0) && (lParam == 'I'))
			{
				//OutputDebugStringA("File Info Window");
				if (mydata->InfoWindow.CreateWnd(mydata->pSharedPluginInstance->UlisterInstance.hInstWLX, mydata->TListerWindow)) SetActiveWindow(mydata->InfoWindow.hwndFileInfo);
				else mydata->InfoWindow.Show();
				return 0;
			}
			if (lParam == VK_F11)
			{
				//OutputDebugStringA("F11 - Full Screen");
				mydata->FullScreen.ChangeFullScrMode();
				return 0;
			}
			if (lParam == VK_ESCAPE)
				if (mydata->FullScreen.isFullScrEnabled()) // only in full screen mode else post ESC end other KEY messages
				{
					//OutputDebugStringA("Exit Full Screen");
					mydata->FullScreen.ExitFromFullScrMode();
					return 0;
				}

			PostMessage(mydata->TListerWindow, WM_KEYDOWN, lParam, 0); // TC Lister Handler: (Ctrl+F/F7, F3/Shift+F3, ESC)
			break;
		
		// WM_MOUSEHWHEEL/WM_MOUSEWHEEL mesages are available ONLY in SCCDISPLAY child window!

		case WM_TIMER:
			// update WindowProc adress of SCCDISPLAY child window if the sheet change (after event on SCCSECLIST child window)

			// to hook MOUSEHWHEEL:

			//	TLister Window (WM_MOVE MESSAGES IS HERE!)
			//	|
			//	+-WAwc
			//		|
			//		+-SCCVIEWER
			//			|
			//			+-SCCDISPLAY (CHILD WITH MOUSEHWHEEL MESSAGES IS HERE!)
			//			+-SCCSCROLL
			//			+-SCCSCROLL
			//			+-SCCSECLIST

			_SccdisplayWindow = FindWindowExA(mydata->SccviewerWindow, NULL, WNDCLASSNAME_SCCDISPLAY, NULL); // get child window
			_SccdisplayWindowProc = (WNDPROC)GetWindowLongPtrA(_SccdisplayWindow, GWLP_WNDPROC);

			if (_SccdisplayWindowProc && reinterpret_cast<void*>(_SccdisplayWindowProc) != reinterpret_cast<void*>(SccdisplayWindowProc))
			{
				/*
				std::string msg;

				OutputDebugStringA("------------------------------------------------");
				OutputDebugStringA("sheet changed, update:");
				msg = "_SccdisplayWindow=" + std::to_string(reinterpret_cast<uintptr_t>(_SccdisplayWindow));
				OutputDebugStringA(msg.c_str());
				msg = "_SccdisplayWindowProc=" + std::to_string(reinterpret_cast<uintptr_t>(_SccdisplayWindowProc));
				OutputDebugStringA(msg.c_str());

				msg = "mydata->SccdisplayWindow=" + std::to_string(reinterpret_cast<uintptr_t>(mydata->SccdisplayWindow));
				OutputDebugStringA(msg.c_str());
				msg = "mydata->OriginalSccdisplayWindowProc=" + std::to_string(reinterpret_cast<uintptr_t>(mydata->OriginalSccdisplayWindowProc));
				OutputDebugStringA(msg.c_str());

				msg = "function SccdisplayWindowProc=" + std::to_string(reinterpret_cast<uintptr_t>(SccdisplayWindowProc));
				OutputDebugStringA(msg.c_str());
				OutputDebugStringA("------------------------------------------------");
				*/

				mydata->SccdisplayWindow = _SccdisplayWindow;
				mydata->OriginalSccdisplayWindowProc = (WNDPROC)SetWindowLongPtrA(_SccdisplayWindow, GWLP_WNDPROC, (LONG_PTR)SccdisplayWindowProc);

			}

			if (mydata->winkeyhack)
			{
				mydata->winkeyhack = false;
				mydata->FullScreen.EnterToFullScrMode();
			}


			break;
		case WM_COMMAND:
		{
			// handle custom menu actions
			switch (LOWORD(wParam))
			{
			case ID_CUSTOM_FULLSCR:
				//OutputDebugStringA("ID_CUSTOM_FULLSCR");
				mydata->FullScreen.ChangeFullScrMode();
				return 0;
			case ID_CUSTOM_FILEINF:
				//OutputDebugStringA("ID_CUSTOM_FILEINF");
				if (mydata->InfoWindow.CreateWnd(mydata->pSharedPluginInstance->UlisterInstance.hInstWLX, mydata->TListerWindow)) SetActiveWindow(mydata->InfoWindow.hwndFileInfo);
				else mydata->InfoWindow.Show();
				return 0;
			case ID_CUSTOM_DRGNDRP:
				//OutputDebugStringA("ID_CUSTOM_DRGNDRP");
				mydata->SetDragnDrop(!mydata->isDragnDropEnabled());
				return 0;
			case ID_CUSTOM_FINDTXT:
				//OutputDebugStringA("ID_CUSTOM_FINDTXT");
				SetFocus(mydata->TListerWindow);
				PostMessage(mydata->TListerWindow, WM_KEYDOWN, VK_F7, 0);
				PostMessage(mydata->TListerWindow, WM_KEYUP, VK_F7, 0);
				return 0;
			case ID_CUSTOM_SCRLBAR:
				//OutputDebugStringA("ID_CUSTOM_SCRLBAR");
				mydata->EnableScrollBar(!mydata->isScrollBarEnabled());
				return 0;
			}
			break;
		}
		} // switch
		return CallWindowProc(mydata->OriginalSccviewerWindowProc, hWnd, message, wParam, lParam); // OIT Handler
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}



///////////////////////////////////////////////////////////////////////////////////////////////////



LRESULT CALLBACK SccdisplayWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	_control87(MCW_EM, MCW_EM);
	HWND SccviewerWindow = GetAncestor(hWnd, GA_PARENT); // get parent window
	clsVTWindowInstance *mydata;
	VTDWORD DisplayEngineType;

	/*
	if (message != WM_MOUSEMOVE && message != WM_NCHITTEST && message != WM_SETCURSOR)
	{
		std::string msg = "SccdisplayWindowProc: message=" + std::to_string(message);
		OutputDebugStringA(msg.c_str());

		//std::string msg = "hWnd=" + std::to_string(reinterpret_cast<uintptr_t>(hWnd));
		//OutputDebugStringA(msg.c_str());

		msg = "SccviewerWindow=" + std::to_string(reinterpret_cast<uintptr_t>(SccviewerWindow));
		OutputDebugStringA(msg.c_str());
	}
	*/

	mydata = (clsVTWindowInstance *)GetWindowLongPtr(SccviewerWindow, GWLP_USERDATA);
	if (mydata)
	{
		switch (message)
		{
		case WM_MOUSEHWHEEL:
			if (mydata->pSharedPluginInstance->UlisterOptions.mwhscrollinvert)
				PostMessage(mydata->SccviewerWindow, SCCVW_HSCROLL, (GET_WHEEL_DELTA_WPARAM(wParam) > 0) ? SCCSB_LINELEFT : SCCSB_LINERIGHT, 0); // invert
			else
				PostMessage(mydata->SccviewerWindow, SCCVW_HSCROLL, (GET_WHEEL_DELTA_WPARAM(wParam) > 0) ? SCCSB_LINERIGHT : SCCSB_LINELEFT, 0);
			return 0;
		case WM_MOUSEWHEEL:
			// OutputDebugStringA("***WM_MOUSEWHEEL***");
			DisplayEngineType = mydata->GetDisplayEngineVT();
			if (GetKeyState(VK_CONTROL) < 0 && GetKeyState(VK_SHIFT) < 0)
			{
				// CTRL+SHIFT+MOUSEHWHEEL : Next/Prev View Mode (ex. for Word Processor, HTML and EMAIL: draft->normal->preview->weblayout->draft)
				if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) mydata->ChangeViewMode(UlisterNextMode::MPREV); else mydata->ChangeViewMode(UlisterNextMode::MNEXT);
				return 0;
			}
			else if (GetKeyState(VK_CONTROL) < 0 && DisplayEngineType != SCCVWTYPE_IMAGE && DisplayEngineType != SCCVWTYPE_VECTOR)
			{
				// CTRL+MOUSEHWHEEL : Zoom-In/Zoom-Out
				if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) mydata->ZoomBitmapVecFont(UlisterZoom::ZIN); else mydata->ZoomBitmapVecFont(UlisterZoom::ZOUT);
				return 0;
			}
			else if (GetKeyState(VK_SHIFT) < 0)
			{
				// SHIFT+MOUSEHWHEEL : horizontal scroll for spreadsheet and database
				if (mydata->pSharedPluginInstance->UlisterOptions.mwhscrollinvert)
					PostMessage(mydata->SccviewerWindow, SCCVW_HSCROLL, (GET_WHEEL_DELTA_WPARAM(wParam) > 0) ? SCCSB_LINELEFT : SCCSB_LINERIGHT, 0); // invert
				else
					PostMessage(mydata->SccviewerWindow, SCCVW_HSCROLL, (GET_WHEEL_DELTA_WPARAM(wParam) > 0) ? SCCSB_LINERIGHT : SCCSB_LINELEFT, 0);
				return 0;
			}
			// MOUSEHWHEEL : Up-Down scroll
			break;
		case WM_MBUTTONDOWN:
			//OutputDebugStringA("WM_MBUTTONDOWN");
			DisplayEngineType = mydata->GetDisplayEngineVT();
			if (wParam & MK_CONTROL) { mydata->ZoomBitmapVecFont(UlisterZoom::ZRESET); return 0; }
			break;
		case WM_SYSKEYDOWN:
			if ((wParam == VK_RETURN) && (GetKeyState(VK_MENU) < 0))
			{
				//OutputDebugStringA("Alt+Enter - Change Full Screen [English keyboard layout]");
				mydata->FullScreen.ChangeFullScrMode();
				return 0;
			}
		case WM_KEYDOWN: // 'WM_KEYDOWN' message <-- fix Right-Alt + Enter in international layout
			if ((wParam == VK_RETURN) && (GetKeyState(VK_MENU) < 0))
			{
				//OutputDebugStringA("Alt+Enter - Change Full Screen [Russian keyboard layout]");
				mydata->FullScreen.ChangeFullScrMode();
				return 0;
			}
		} // switch
		return CallWindowProc(mydata->OriginalSccdisplayWindowProc, hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}



///////////////////////////////////////////////////////////////////////////////////////////////////



clsVTWindowInstance::clsVTWindowInstance() : ToolTip(TOOLTIP_TIMER_MSG)
{
	TListerWindow = NULL;
	OriginalTListerWindowProc = NULL;

	waWindow = NULL;

	OriginalSccviewerWindowProc = NULL;
	SccviewerWindow = NULL;

	OriginalSccdisplayWindowProc = NULL;
	SccdisplayWindow = NULL;

	pSharedPluginInstance = NULL;

	WindSearchStrW[0] = L'\0';
	WindSearchStrA[0] = '\0';

	winkeyhack = false;
}



///////////////////////////////////////////////////////////////////////////////////////////////////



void clsVTWindowInstance::AddFileInfo()
{
	wchar_t buf[ULISTMAXBUF];
	swprintf_s(buf, ULISTMAXBUF,
		L"File: %s\r\n\r\n"
		L"Format type: %u  -  %S\r\n\r\n"
		L"Display engine: %s\r\n",
		LoadedFileInfo.pPath,
		LoadedFileInfo.wType, LoadedFileInfo.pTypeName,
		DisplayEngineName(GetDisplayEngineVT()));

	InfoWindow.Init(buf, INFOWINDOWWIDTH, INFOWINDOWHEIGHT);
}



///////////////////////////////////////////////////////////////////////////////////////////////////



bool clsVTWindowInstance::CreateListerWindow(const HWND ParentWin, const HINSTANCE hInst)
//	OUT:
// waWindow
// SccviewerWindow
// OriginalSccviewerWindowProc
// TListerWindow
// OriginalTListerWindowProc
//	RETURN:
// true - OK
// false - fail
{
	RECT		r;
	WNDCLASS	wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WAwcWindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WNDCLASSNAME_WAWC;
	RegisterClass(&wc);

	GetClientRect(ParentWin, &r);
	waWindow = CreateWindow(WNDCLASSNAME_WAWC, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, r.left, r.top, r.right - r.left, r.bottom - r.top, ParentWin, 0, hInst, NULL);
	if (!IsWindow(waWindow)) return false;

	GetClientRect(waWindow, &r);
	SccviewerWindow = CreateWindow(WNDCLASSNAME_SCCVIEWER, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, r.left, r.top, r.right - r.left, r.bottom - r.top, waWindow, 0, hInst, NULL);
	if (!IsWindow(SccviewerWindow)) { DestroyWindow(waWindow); return false; }

	TListerWindow = ParentWin;

	OriginalSccviewerWindowProc = (WNDPROC)SetWindowLongPtr(SccviewerWindow, GWLP_WNDPROC, (LONG_PTR)SccviewerWindowProc);
	OriginalTListerWindowProc = (WNDPROC)SetWindowLongPtr(ParentWin, GWLP_WNDPROC, (LONG_PTR)TListerWindowProc);

	//std::wstring msgW = L"CreateListerWindow: OriginalTListerWindowProc=" + ToHexW(mydata->OriginalTListerWindowProc);
	//OutputDebugStringW(msgW.c_str());

	// exception with Delphi 12 SetWindowLongPtr(hViewWnd, GWLP_USERDATA,(long) this);
	// exception with Delphi 12 SetWindowLongPtr(waWnd, GWLP_USERDATA,(long) this);
	SetWindowLongPtr(SccviewerWindow, GWLP_USERDATA, (LONG_PTR)this);
	SetWindowLongPtr(waWindow, GWLP_USERDATA, (LONG_PTR)this);
	SetWindowLongPtr(ParentWin, GWLP_USERDATA, (LONG_PTR)this);

	return true;
}



///////////////////////////////////////////////////////////////////////////////////////////////////



bool clsVTWindowInstance::VTLoad(const HWND ParentWin, const wchar_t *FileToLoad, clsSharedPluginInstance *_pSharedPluginInstance)
//	RETURN:
// true - OK
// false - fail
{
#if defined (__ULISTDEBUGMSG)
	std::wstring msgW = L"VTLoad (" + std::wstring(FileToLoad) + L", ParentWin=" + ToHexW(ParentWin) + L"); ParentWin=" + ToStrW(ParentWin) + L" (decimal)";
	OutputDebugStringW(msgW.c_str());
#endif

	if (!_pSharedPluginInstance) return false;
	pSharedPluginInstance = _pSharedPluginInstance;

	__VTTYPENAMEBUF pTypeName;
	VTWORD wType = pSharedPluginInstance->GetVTFileType(FileToLoad, pTypeName);

	if (!pSharedPluginInstance->IsVTFileTypeAllowed(wType, false))
	{
#if defined (__ULISTDEBUGMSG)
		OutputDebugStringW(L"VTLoad := false; // IsVTFileTypeAllowed");
#endif
		return false;
	}

	if (!waWindow)
	{
#if defined (__ULISTDEBUGMSG)
		OutputDebugStringW(L"VTLoad: ListLoadW call detected");
#endif
		if (!pSharedPluginInstance->UlisterInstance.ViewerLibraryInstanceInc()) return false;

		if (!CreateListerWindow(ParentWin, pSharedPluginInstance->UlisterInstance.hInstWLX)) return false;
	}
#if defined (__ULISTDEBUGMSG)
	else OutputDebugStringW(L"VTLoad: ListLoadNextW call detected");
#endif

	if (!pSharedPluginInstance->LoadVTFile(SccviewerWindow, FileToLoad))
	{
		// TC SDK:
		// Return a handle to your window if load succeeds, NULL otherwise. If NULL is returned, Lister will try the next plugin.

		VTUnload();
#if defined (__ULISTDEBUGMSG)
		OutputDebugStringW(L"VTLoad := false; // LoadVTFile");
#endif
		return false;
	}

	bool quickview = WS_CHILD & GetWindowLongPtr(ParentWin, GWL_STYLE);
	LoadedFileInfo.Init(FileToLoad, wType, pTypeName, quickview);
	pSharedPluginInstance->VTOptions.SendVTOptions(SccviewerWindow);
	AddFileInfo();

	//OutputDebugStringW(mydata->LoadedFileInfo.pPath);
	//OutputDebugStringA(mydata->LoadedFileInfo.pTypeName);

	//SetSccdisplayChildWndProc(hViewWnd);

	if (!quickview) SetFocus(SccviewerWindow);

#if defined (__ULISTDEBUGMSG)
	msgW = L"VTLoad := (HWND) " + ToHexW(waWindow) + L";";
	OutputDebugStringW(msgW.c_str());
#endif

	FullScreen.Init(ParentWin);

	// Windows logo key:
	if ((GetAsyncKeyState(VK_LWIN) < 0) || (GetAsyncKeyState(VK_RWIN) < 0))
	{
		//FullScreen.EnterToFullScrMode(); <-- too early!
		winkeyhack = true;
	}

	return true;
} // clsVTWindowInstance::VTLoad



///////////////////////////////////////////////////////////////////////////////////////////////////



void clsVTWindowInstance::VTUnload()
{
#if defined (__ULISTDEBUGMSG)
	OutputDebugStringW(L"VTUnload");
#endif

	FullScreen.ExitFromFullScrMode();

	// ListCloseWindow:

	SendMessage(SccviewerWindow, SCCVW_SAVEOPTIONS, 0, 0L);
	SendMessage(SccviewerWindow, SCCVW_CLOSEFILE, 0, 0L);
	DestroyWindow(SccviewerWindow);
	DestroyWindow(waWindow);

	ToolTip.DestroyTemporaryMessage();
	InfoWindow.Done(); // force?

	// WARNING!
	// Calling ListCloseWindow doesn't necessarily mean the parent window will be "closed".
	// It's necessary to return the original address of the window procedure to the parent window, since it may be used later by another plugin.
	if (OriginalTListerWindowProc) SetWindowLongPtr(TListerWindow, GWLP_WNDPROC, (LONG_PTR)OriginalTListerWindowProc);

	pSharedPluginInstance->UlisterInstance.ViewerLibraryInstanceDec(pSharedPluginInstance->UlisterOptions.keepinmemory);

	waWindow = NULL;
	OriginalTListerWindowProc = NULL;

} // clsVTWindowInstance::VTUnload



///////////////////////////////////////////////////////////////////////////////////////////////////



void clsVTWindowInstance::ChangeViewMode(const int dir)
{
	// dir =  1		- next view mode
	// dir = -1		- prev view mode

	LPCWSTR AUNK = L"Unknown";
	LPCWSTR ADRAFT = L"Draft";
	LPCWSTR ANORMAL = L"Normal";
	LPCWSTR APREVIEW = L"Preview";
	LPCWSTR AWEBLAY = L"Weblayout";
	LPCWSTR AHIDDEN = L"Hidden";
	LPCWSTR ANONE = L"None";
	LPCWSTR ANAME = L"Name";
	LPCWSTR ASIZE = L"Size";
	LPCWSTR ADATE = L"Date";
	LPCWSTR A0 = L"0\u00B0";
	LPCWSTR A90 = L"90\u00B0";
	LPCWSTR A180 = L"180\u00B0";
	LPCWSTR A270 = L"270\u00B0";

	LPCWSTR VIEWMODENAME = AUNK;

	VTDWORD DispEng;

	union
	{
		VTDWORD viewmode;
		VTDWORD bitmaprotation;
		VTDWORD arcsortorder;
		VTBOOL spreadsheetdraft;
	};

	SCCVWOPTIONSPEC40 locOptionSpec;
	locOptionSpec.dwSize = sizeof(SCCVWOPTIONSPEC40);
	locOptionSpec.dwFlags = SCCVWOPTION_CURRENT;
	locOptionSpec.pData = &viewmode;

	DispEng = GetDisplayEngineVT(); // call only from user-level defined messages!!!
	if (DispEng == SCCVWTYPE_WP || DispEng == SCCVWTYPE_HTML || DispEng == SCCVWTYPE_EMAIL)
	{
		// word processor: draft->normal->preview->weblayout

		if (DispEng == SCCVWTYPE_WP) locOptionSpec.dwId = SCCID_WPDISPLAYMODE;
		else locOptionSpec.dwId = (DispEng == SCCVWTYPE_HTML) ? SCCID_HTMLDISPLAYMODE : SCCID_EMAILDISPLAYMODE;

		SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		if (dir == UlisterNextMode::MNEXT) { viewmode++; if (viewmode > SCCVW_WPMODE_WEBLAYOUT) viewmode = SCCVW_WPMODE_WEBLAYOUT; }
		else { viewmode--; if (viewmode < SCCVW_WPMODE_DRAFT) viewmode = SCCVW_WPMODE_DRAFT; }

		SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

		if (viewmode == SCCVW_WPMODE_DRAFT) VIEWMODENAME = ADRAFT;
		else if (viewmode == SCCVW_WPMODE_NORMAL) VIEWMODENAME = ANORMAL;
		else if (viewmode == SCCVW_WPMODE_PREVIEW) VIEWMODENAME = APREVIEW;
		else if (viewmode == SCCVW_WPMODE_WEBLAYOUT) VIEWMODENAME = AWEBLAY;
		else VIEWMODENAME = AUNK;

		ToolTip.InitPosition(TListerWindow, TOOLTIP_XOFFS, TOOLTIP_YOFFS, TOOLTIP_WIDTH, TOOLTIP_HEIGHT);
		ToolTip.ShowTemporaryMessage(VIEWMODENAME, pSharedPluginInstance->UlisterOptions.ToolTipTransparency, pSharedPluginInstance->UlisterOptions.ToolTipTimer);
	}
	else if (DispEng == SCCVWTYPE_SS)
	{
		// SCCID_SSSHOWGRIDLINES ???

		// spreadsheet: draft->normal->normal with hidden rows and columns displayed

		VTBOOL spreadsheethiddencells;

		//locOptionSpec.pData = &spreadsheetdraft;
		locOptionSpec.dwId = SCCID_SSDRAFTMODE;
		SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		locOptionSpec.pData = &spreadsheethiddencells;
		locOptionSpec.dwId = SCCID_SSSHOWHIDDENCELLS;
		SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		if (dir == UlisterNextMode::MNEXT)
		{
			if (spreadsheethiddencells == FALSE && spreadsheetdraft == TRUE) { spreadsheetdraft = FALSE; VIEWMODENAME = ANORMAL; } // draft->normal
			else if (spreadsheethiddencells == FALSE && spreadsheetdraft == FALSE) { spreadsheethiddencells = TRUE; VIEWMODENAME = AHIDDEN; } // normal->normal with hidden rows and columns displayed
			else if (spreadsheethiddencells == TRUE && spreadsheetdraft == FALSE) { VIEWMODENAME = AHIDDEN; } // normal with hidden rows and columns displayed->nothing
			else { spreadsheethiddencells = FALSE; spreadsheetdraft = TRUE; VIEWMODENAME = ADRAFT; } // reset to draft
		}
		else
		{
			if (spreadsheethiddencells == TRUE && spreadsheetdraft == FALSE) { spreadsheethiddencells = FALSE; VIEWMODENAME = ANORMAL; } // normal with hidden rows and columns displayed->normal
			else if (spreadsheethiddencells == FALSE && spreadsheetdraft == FALSE) { spreadsheetdraft = TRUE; VIEWMODENAME = ADRAFT; } // normal->draft
			else if (spreadsheethiddencells == FALSE && spreadsheetdraft == TRUE) { VIEWMODENAME = ADRAFT; } // draft->nothing
			else { spreadsheethiddencells = TRUE; spreadsheetdraft = FALSE; VIEWMODENAME = AHIDDEN; } // reset to normal with hidden rows and columns displayed
		}

		//locOptionSpec.pData = &spreadsheethiddencells;
		//locOptionSpec.dwId = SCCID_SSSHOWHIDDENCELLS;
		SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		locOptionSpec.pData = &spreadsheetdraft;
		locOptionSpec.dwId = SCCID_SSDRAFTMODE;
		SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		ToolTip.InitPosition(TListerWindow, TOOLTIP_XOFFS, TOOLTIP_YOFFS, TOOLTIP_WIDTH, TOOLTIP_HEIGHT);
		ToolTip.ShowTemporaryMessage(VIEWMODENAME, pSharedPluginInstance->UlisterOptions.ToolTipTransparency, pSharedPluginInstance->UlisterOptions.ToolTipTimer);
	}
	else if (DispEng == SCCVWTYPE_ARCHIVE)
	{
		// NONE->NAME->SIZE->DATE

		//locOptionSpec.pData = &arcsortorder;
		locOptionSpec.dwId = SCCID_ARCSORTORDER;
		SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		if (dir == UlisterNextMode::MNEXT) { arcsortorder++; if (arcsortorder > SCCVW_SORT_DATE) arcsortorder = SCCVW_SORT_DATE; }
		else { arcsortorder--; if (arcsortorder < SCCVW_SORT_NONE) arcsortorder = SCCVW_SORT_NONE; }

		SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
		// This option is saved in the .oit directory and does need to be reset to SCCVW_SORT_NAME when loading an archive file (see clsVTOptionsViewer::SendVTViewOptions function).

		if (arcsortorder == SCCVW_SORT_NONE) VIEWMODENAME = ANONE;
		else if (arcsortorder == SCCVW_SORT_NAME) VIEWMODENAME = ANAME;
		else if (arcsortorder == SCCVW_SORT_SIZE) VIEWMODENAME = ASIZE;
		else if (arcsortorder == SCCVW_SORT_DATE) VIEWMODENAME = ADATE;
		else VIEWMODENAME = AUNK;

		ToolTip.InitPosition(TListerWindow, TOOLTIP_XOFFS, TOOLTIP_YOFFS, TOOLTIP_WIDTH, TOOLTIP_HEIGHT);
		ToolTip.ShowTemporaryMessage(VIEWMODENAME, pSharedPluginInstance->UlisterOptions.ToolTipTransparency, pSharedPluginInstance->UlisterOptions.ToolTipTimer);
	}
	else if (DispEng == SCCVWTYPE_IMAGE)
	{
		// SCCID_ANTIALIAS ??? SCCVW_ANTIALIAS_OFF | SCCVW_ANTIALIAS_ALL ???

		// rotate 0->90->180->270->0->...

		//locOptionSpec.pData = &bitmaprotation;
		locOptionSpec.dwId = SCCID_BMPROTATION;
		SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		if (dir == UlisterNextMode::MNEXT) { bitmaprotation = bitmaprotation + 90; if (bitmaprotation > SCCVW_ROTATION_270) bitmaprotation = SCCVW_ROTATION_NONE; }
		else { bitmaprotation = bitmaprotation - 90; if (bitmaprotation > SCCVW_ROTATION_270) bitmaprotation = SCCVW_ROTATION_270; } // unsigned int (VTDWORD) overflow hack

		SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
		// It seems that this option is not saved in the .oit directory and does not need to be reset to SCCVW_ROTATION_NONE when loading an image file.

		if (bitmaprotation == SCCVW_ROTATION_NONE) VIEWMODENAME = A0;
		else if (bitmaprotation == SCCVW_ROTATION_90) VIEWMODENAME = A90;
		else if (bitmaprotation == SCCVW_ROTATION_180) VIEWMODENAME = A180;
		else if (bitmaprotation == SCCVW_ROTATION_270) VIEWMODENAME = A270;
		else VIEWMODENAME = AUNK;

		ToolTip.InitPosition(TListerWindow, TOOLTIP_XOFFS, TOOLTIP_YOFFS, TOOLTIP_WIDTH, TOOLTIP_HEIGHT);
		ToolTip.ShowTemporaryMessage(VIEWMODENAME, pSharedPluginInstance->UlisterOptions.ToolTipTransparency, pSharedPluginInstance->UlisterOptions.ToolTipTimer);
	}
	/*
	else if (DispEng == SCCVWTYPE_VECTOR)
	{
	// SCCID_VECSHOWFULLSCREEN ???
	// SCCID_STROKE_TEXT ???
	}
	else if (DispEng == SCCVWTYPE_DB)
	{
	// SCCID_DBDRAFTMODE ???
	// SCCID_DBSHOWGRIDLINES ???
	}
	else if (DispEng == SCCVWTYPE_HEX)
	{
	// ??? nothing
	}
	*/
}



///////////////////////////////////////////////////////////////////////////////////////////////////



void clsVTWindowInstance::ZoomBitmapVecFont(const int dir)
{
	// dir =  1		- zoom in
	// dir = -1		- zoom out
	// dir =  0		- zoom reset to 100% (146% in Russian Federation)

	//clsVTWindowInstance *mydata;
	VTDWORD DispEng;
	//mydata = (clsVTWindowInstance *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	SCCVWOPTIONSPEC40 locOptionSpec;
	VTDWORD zoom;
	locOptionSpec.dwSize = sizeof(SCCVWOPTIONSPEC40);
	locOptionSpec.dwFlags = SCCVWOPTION_CURRENT;
	locOptionSpec.pData = &zoom;

	DispEng = GetDisplayEngineVT(); // call only from user-level defined messages!!!
	if (DispEng == SCCVWTYPE_IMAGE)
	{
		locOptionSpec.dwId = SCCID_BMPZOOMEVENT;
		if (dir == UlisterZoom::ZRESET) zoom = SCCVW_ZOOM_RESET;
		else zoom = (dir == UlisterZoom::ZIN) ? SCCVW_ZOOM_IN : SCCVW_ZOOM_OUT;
		SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
	}
	else if (DispEng == SCCVWTYPE_VECTOR)
	{
		locOptionSpec.dwId = SCCID_VECZOOMEVENT;
		if (dir == UlisterZoom::ZRESET) zoom = SCCVW_ZOOM_RESET;
		else zoom = (dir == UlisterZoom::ZIN) ? SCCVW_ZOOM_IN : SCCVW_ZOOM_OUT;
		SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
	}
	else if (DispEng == SCCVWTYPE_WP || DispEng == SCCVWTYPE_HTML || DispEng == SCCVWTYPE_EMAIL)
	{
		// oracle bug: SCCID_FONTSCALINGFACTOR not working if SCCVW_WPMODE_PREVIEW or SCCVW_WPMODE_WEBLAYOUT mode of Word Processor / HTML / EMAIL!
		// From A.10.5 SCCID_FONTSCALINGFACTOR Note:
		// For word processor documents, this only affects normal and draft modes.

		VTDWORD WPdisplaymode;
		locOptionSpec.dwId = SCCID_WPDISPLAYMODE;
		locOptionSpec.pData = &WPdisplaymode;
		SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		VTDWORD HTMLdisplaymode;
		locOptionSpec.dwId = SCCID_HTMLDISPLAYMODE;
		locOptionSpec.pData = &HTMLdisplaymode;
		SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		VTDWORD EMAILdisplaymode;
		locOptionSpec.dwId = SCCID_EMAILDISPLAYMODE;
		locOptionSpec.pData = &EMAILdisplaymode;
		SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		locOptionSpec.pData = &zoom;
		locOptionSpec.dwId = SCCID_FONTSCALINGFACTOR;
		SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
		if (dir == UlisterZoom::ZRESET) zoom = 100; // percent
		else zoom = (dir == UlisterZoom::ZIN) ? zoom * 10 / 8 : zoom * 8 / 10;
		SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

		// workaround:
		if (DispEng == SCCVWTYPE_WP && (WPdisplaymode == SCCVW_WPMODE_PREVIEW || WPdisplaymode == SCCVW_WPMODE_WEBLAYOUT) ||
			DispEng == SCCVWTYPE_HTML && (HTMLdisplaymode == SCCVW_WPMODE_PREVIEW || HTMLdisplaymode == SCCVW_WPMODE_WEBLAYOUT) ||
			DispEng == SCCVWTYPE_EMAIL && (EMAILdisplaymode == SCCVW_WPMODE_PREVIEW || EMAILdisplaymode == SCCVW_WPMODE_WEBLAYOUT))
		{
			// temporarily switch to draft mode
			zoom = SCCVW_WPMODE_DRAFT;

			ShowWindow(SccviewerWindow, SW_HIDE);

			locOptionSpec.dwId = SCCID_WPDISPLAYMODE;
			SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

			locOptionSpec.dwId = SCCID_HTMLDISPLAYMODE;
			SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

			locOptionSpec.dwId = SCCID_EMAILDISPLAYMODE;
			SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

			// and switch back

			locOptionSpec.dwId = SCCID_WPDISPLAYMODE;
			locOptionSpec.pData = &WPdisplaymode;
			SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

			locOptionSpec.dwId = SCCID_HTMLDISPLAYMODE;
			locOptionSpec.pData = &HTMLdisplaymode;
			SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

			locOptionSpec.dwId = SCCID_EMAILDISPLAYMODE;
			locOptionSpec.pData = &EMAILdisplaymode;
			SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

			ShowWindow(SccviewerWindow, SW_SHOW);
		}
	}
	else if (DispEng == SCCVWTYPE_SS || DispEng == SCCVWTYPE_DB ||
		DispEng == SCCVWTYPE_HEX || DispEng == SCCVWTYPE_ARCHIVE)
	{
		locOptionSpec.dwId = SCCID_FONTSCALINGFACTOR;
		SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
		if (dir == UlisterZoom::ZRESET) zoom = 100; // percent
		else zoom = (dir == UlisterZoom::ZIN) ? zoom * 10 / 8 : zoom * 8 / 10;
		SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
	}
}




VTDWORD clsVTWindowInstance::GetDisplayEngineVT()
{
	// SCCVWTYPE_NONE		1  /* no file open in this view */
	// SCCVWTYPE_UNKNOWN	2  /* unknown section type */
	// SCCVWTYPE_WP			3  /* word processor section */
	// SCCVWTYPE_SS			4  /* spreadsheet section */
	// SCCVWTYPE_DB			5  /* database section */
	// SCCVWTYPE_HEX		6  /* hex view of any file */
	// SCCVWTYPE_IMAGE		7  /* bitmap image */
	// SCCVWTYPE_ARCHIVE	8  /* archive */
	// SCCVWTYPE_VECTOR		9	Vector graphics display engine
	// SCCVWTYPE_SOUND		10 ***** /* sound file */ *****
	// SCCVWTYPE_HTML		11 /* html file */
	// SCCVWTYPE_EMAIL		12 /* email file */

	SCCVWDISPLAYINFO40 locDisplayInfo;
	locDisplayInfo.dwSize = sizeof(SCCVWDISPLAYINFO40);
	SendMessage(SccviewerWindow, SCCVW_GETDISPLAYINFO, 0, (LPARAM)(PSCCVWDISPLAYINFO40)&locDisplayInfo);
	return locDisplayInfo.dwType;
}



wchar_t* clsVTWindowInstance::DisplayEngineName(const VTDWORD dwType)
{
	wchar_t* name[] =
	{
		L"COMMON UNKNOWN",
		L"No file open",
		L"Unknown type",
		L"Word Processor",
		L"Spreadsheet",
		L"Database",
		L"Hexadecimal view",
		L"Bitmap image",
		L"Archive",
		L"Vector graphics",
		L"Sound file",
		L"HTML document",
		L"email"
	};
	const unsigned int count = sizeof(name) / sizeof(name[0]) - 1;
	return name[(dwType > count) ? 0 : dwType];
}



#pragma warning( push )
#pragma warning( disable : 4996 )

void clsVTWindowInstance::ListSearchTextWHandler(const WCHAR* SearchStringW, const int SearchParameter)
{
	//OutputDebugStringW(L"ListSearchTextW");
	//OutputDebugStringW(SearchStringW);

	bool WindowWithoutSearchStringYet = (WindSearchStrW[0] == L'\0');
	bool isSearchBack = SearchParameter & lcs_backwards;

	// TC doesn't store SearchStringW (or SearchString if ANSI) for each window separately; this parameter is global.
	// Therefore, one TC window can change SearchStringW in another TC window.
	// To ensure that each window has its own independent search string, the plugin needs to store its own WindSearchStrW search string for each window.
	// WindSearchStrW needs to be updated from SearchStringW only when lcs_findfirst is called (step 1).
	// When calling find next or find previous, you can't rely on SearchStringW; you must use your own copy of WindSearchStrW (from step 1).

	// The same applies to the case-sensitive search parameter lcs_matchcase! It's global.
	// Extract only lcs_backwards or lcs_findfirst from the SearchParameter argument, use your own stored window copy for the remaining flags.
	// The plugin does not need to store the lcs_matchcase flag for each window, because the Viewer Technology library does this.

	// If the "case sensitive" box is not checked in the TC search form, the string is transmitted in lowercase (i.e., small) letters -
	// yes, TC changes capital (uppercase) letters without permission!

	bool SearchStringWasChanged = false;

	SetVTSearchUnicode();

	if ((SearchParameter & lcs_findfirst) || WindowWithoutSearchStringYet)
	{
		//OutputDebugStringW(L"lcs_findfirst || WindowWithoutSearchStringYet");

		// truncating the search string (MAXSEARCH) to ensure compatibility with the Viewer Technology library
		SearchStringWasChanged = wcsncmp(SearchStringW, WindSearchStrW, MAXSEARCH) != 0; // for the forcefindfirst=on option // strncmp wcsncmp

		// update internal search string
		if (SearchStringWasChanged)
		{
			wcsncpy(WindSearchStrW, SearchStringW, MAXSEARCH);
			WindSearchStrW[MAXSEARCH] = L'\0'; // fix potencial overflow
		}

		// call Viewer Technology library with new search parameters
		SCCVWSEARCHINFO80 locSearchInfo80;

		locSearchInfo80.dwSize = sizeof(SCCVWSEARCHINFO80);
		wcsncpy((wchar_t*)locSearchInfo80.siText, WindSearchStrW, VTMAXSEARCHBUF);
		locSearchInfo80.siTextLen = (VTWORD)wcslen((wchar_t*)locSearchInfo80.siText);

		locSearchInfo80.siType = (SearchParameter & lcs_matchcase) ? SCCVW_SEARCHCASE : SCCVW_SEARCHNOCASE;
		// Now the "whole words only" checkbox means to start the search forward from the beginning of the document (or from the end if the search is reversed)
		locSearchInfo80.siFrom = (SearchParameter & lcs_wholewords) ?
			(isSearchBack ? SCCVW_SEARCHBOTTOM : SCCVW_SEARCHTOP) : SCCVW_SEARCHCURRENT;
		locSearchInfo80.siDirection = (isSearchBack) ? SCCVW_SEARCHBACK : SCCVW_SEARCHFORWARD;

		if (SendMessageW(SccviewerWindow, SCCVW_SEARCH, 0, (LPARAM)(PSCCVWSEARCHINFO80)&locSearchInfo80) != 0)
			MessageBoxW(SccviewerWindow, WindSearchStrW, WNOTFOUND, MB_OK);
	}
	else
		if (isSearchBack)
		{
			if (SendMessageW(SccviewerWindow, SCCVW_SEARCHNEXT, SCCVW_SEARCHBACK, 0) != 0)
				MessageBoxW(SccviewerWindow, WindSearchStrW, WNOTFOUND, MB_OK);
		}
		else
		{
			if (SendMessageW(SccviewerWindow, SCCVW_SEARCHNEXT, SCCVW_SEARCHFORWARD, 0) != 0)
				MessageBoxW(SccviewerWindow, WindSearchStrW, WNOTFOUND, MB_OK);
		}
	//OutputDebugStringW(WindSearchStrW);
}



void clsVTWindowInstance::ListSearchTextAHandler(const char* SearchString, const int SearchParameter)
{
	// reserved for Windows 98 SE future support maybe

	bool WindowWithoutSearchStringYet = (WindSearchStrA[0] == '\0');

	bool SearchStringWasChanged = false;

	SetVTSearchANSI();

	if ((SearchParameter & lcs_findfirst) || WindowWithoutSearchStringYet)
	{
		// truncating the search string (MAXSEARCH) to ensure compatibility with the Viewer Technology library
		SearchStringWasChanged = strncmp(SearchString, WindSearchStrA, MAXSEARCH) != 0; // for the forcefindfirst=on option

		// update internal search string
		if (SearchStringWasChanged)
		{
			strncpy(WindSearchStrA, SearchString, MAXSEARCH);
			WindSearchStrA[MAXSEARCH] = '\0'; // fix potencial overflow
		}

		// call Viewer Technology library with new search parameters
		SCCVWSEARCHINFO40 locSearchInfo40;

		locSearchInfo40.dwSize = sizeof(SCCVWSEARCHINFO40);
		strncpy(locSearchInfo40.siText, WindSearchStrA, VTMAXSEARCHBUF);
		locSearchInfo40.siTextLen = (VTWORD)strlen(locSearchInfo40.siText);

		locSearchInfo40.siType = (SearchParameter & lcs_matchcase) ? SCCVW_SEARCHCASE : SCCVW_SEARCHNOCASE;
		locSearchInfo40.siFrom = SCCVW_SEARCHCURRENT; // TODO: forcefindfirst; WindowWithoutSearchStringYet ---> SCCVW_SEARCHTOP???
		locSearchInfo40.siDirection = (SearchParameter & lcs_backwards) ? SCCVW_SEARCHBACK : SCCVW_SEARCHFORWARD;

		if (SendMessageW(SccviewerWindow, SCCVW_SEARCH, 0, (LPARAM)(PSCCVWSEARCHINFO80)&locSearchInfo40) != 0)
			MessageBoxA(SccviewerWindow, WindSearchStrA, ANOTFOUND, MB_OK);
	}
	else
		if (SearchParameter & lcs_backwards)
		{
			if (SendMessageW(SccviewerWindow, SCCVW_SEARCHNEXT, SCCVW_SEARCHBACK, 0) != 0)
				MessageBoxA(SccviewerWindow, WindSearchStrA, ANOTFOUND, MB_OK);
		}
		else
		{
			if (SendMessageW(SccviewerWindow, SCCVW_SEARCHNEXT, SCCVW_SEARCHFORWARD, 0) != 0)
				MessageBoxA(SccviewerWindow, WindSearchStrA, ANOTFOUND, MB_OK);
		}
}

#pragma warning( pop )



void clsVTWindowInstance::SetVTSearchUnicode()
{
	// force internal search engine to UNICODE
	SCCVWOPTIONSPEC40 locOptionSpec;
	VTDWORD SystemFlags;
	locOptionSpec.dwSize = sizeof(SCCVWOPTIONSPEC40);
	locOptionSpec.dwFlags = SCCVWOPTION_CURRENT;
	locOptionSpec.dwId = SCCID_SYSTEMFLAGS;
	locOptionSpec.pData = &SystemFlags;
	SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	SystemFlags = SystemFlags | SCCVW_SYSTEM_UNICODE; // set the unicode bit
	SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
}



void clsVTWindowInstance::SetVTSearchANSI()
{
	// force internal search engine to ASCII (default)
	SCCVWOPTIONSPEC40 locOptionSpec;
	VTDWORD SystemFlags;
	locOptionSpec.dwSize = sizeof(SCCVWOPTIONSPEC40);
	locOptionSpec.dwFlags = SCCVWOPTION_CURRENT;
	locOptionSpec.dwId = SCCID_SYSTEMFLAGS;
	locOptionSpec.pData = &SystemFlags;
	SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	SystemFlags = SystemFlags & (~SCCVW_SYSTEM_UNICODE); // reset the unicode bit
	SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
}



HWND clsVTWindowInstance::FindButtonN(const HWND hParent, const int targetIndex)
{
	int buttonCount = 0;
	HWND hChild = GetWindow(hParent, GW_CHILD);

	while (hChild)
	{
		char className[CLASSNAMEMAXBUF];
		GetClassNameA(hChild, className, sizeof(className));
		className[CLASSNAMEMAXBUF - 1] = '\0';

		if (strcmp(className, "Button") == 0)
		{
			buttonCount++;
			if (buttonCount == targetIndex) return hChild;
		}
		hChild = GetWindow(hChild, GW_HWNDNEXT);
	}
	return NULL;
}



void clsVTWindowInstance::OEM_ReplaceButtonFROMBEGINING(const HWND hwndParent)
// This procedure changes the text on the button from 'Whole words only' to 'FROM BEGINING'
{
	if (hwndParent)
	{
		char className[CLASSNAMEMAXBUF];
		GetClassNameA(hwndParent, className, STRLEN(className));
		className[CLASSNAMEMAXBUF - 1] = '\0';

		if (strcmp(className, "TSEARCHTEXT") == 0)
		{
			//OutputDebugStringA("GET IT: TSEARCHTEXT");
			const HWND hButton = FindButtonN(hwndParent, 8); // BUTTON N8 - 'Whole words only'
			if (hButton)
			{
				//OutputDebugStringA("GET IT: hButton");
				SetWindowTextA(hButton, "FROM BEGINING");
			}
		}
	}

}



void clsVTWindowInstance::OEM_AddNewContextMenuItems()
{
	// use the custom ID_CUSTOM_FULLSCR handler to enter Full Screen Mode
	// A.10.3 SCCID_DIALOGFLAGS -> SCCVW_DIALOG_NOADDSHOWFULLSCREEN: The dialog should not display the "Show Full Screen" menu option from the context menu.
	VTWORD VTDialogFlags;
	SCCVWOPTIONSPEC40 locOptionSpec;
	locOptionSpec.dwSize = sizeof(SCCVWOPTIONSPEC40);
	locOptionSpec.dwFlags = SCCVWOPTION_CURRENT;
	locOptionSpec.dwId = SCCID_DIALOGFLAGS;
	locOptionSpec.pData = &VTDialogFlags;
	VTDialogFlags = SCCVW_DIALOG_NOADDSHOWFULLSCREEN; // full screen mode is implemented for the vector graphics only - will use own?
	SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

	SCCVWDISPLAYINFO40 locSccvwDisplayInfo40;
	locSccvwDisplayInfo40.dwSize = sizeof(SCCVWDISPLAYINFO40);
	SendMessage(SccviewerWindow, SCCVW_GETDISPLAYINFO, 0, (LPARAM)(PSCCVWDISPLAYINFO40)&locSccvwDisplayInfo40);
	if (locSccvwDisplayInfo40.hMenu)
	{
		// short-circuit evaluation fix:
		bool retcode1 = DeleteMenu(reinterpret_cast<HMENU>(locSccvwDisplayInfo40.hMenu), ID_CUSTOM_FULLSCR, MF_BYCOMMAND);
		bool retcode2 = DeleteMenu(reinterpret_cast<HMENU>(locSccvwDisplayInfo40.hMenu), ID_CUSTOM_FINDTXT, MF_BYCOMMAND);
		bool retcode3 = DeleteMenu(reinterpret_cast<HMENU>(locSccvwDisplayInfo40.hMenu), ID_CUSTOM_FILEINF, MF_BYCOMMAND);
		if (retcode1 || retcode2 || retcode3) DeleteMenu(reinterpret_cast<HMENU>(locSccvwDisplayInfo40.hMenu), GetMenuItemCount(reinterpret_cast<HMENU>(locSccvwDisplayInfo40.hMenu)) - 1, MF_BYPOSITION); // MF_SEPARATOR

		AppendMenuW(reinterpret_cast<HMENU>(locSccvwDisplayInfo40.hMenu), MF_SEPARATOR, 0, NULL);
		AppendMenuW(reinterpret_cast<HMENU>(locSccvwDisplayInfo40.hMenu), MF_STRING | (FullScreen.isFullScrEnabled() ? MF_CHECKED : MF_UNCHECKED), ID_CUSTOM_FULLSCR, WFULLSCR);
		AppendMenuW(reinterpret_cast<HMENU>(locSccvwDisplayInfo40.hMenu), MF_STRING, ID_CUSTOM_FINDTXT, WFIND);
		AppendMenuW(reinterpret_cast<HMENU>(locSccvwDisplayInfo40.hMenu), MF_STRING, ID_CUSTOM_FILEINF, WFILEINF);
	}

	HMENU hMenuOptions = FindSubMenuByName(reinterpret_cast<HMENU>(locSccvwDisplayInfo40.hMenu), WOPTIONS, true); // get submenu handle
	if (hMenuOptions)
	{
		int menuidx = static_cast<int>(reinterpret_cast<INT_PTR>(FindSubMenuByName(reinterpret_cast<HMENU>(hMenuOptions), WDRAGNDR, false))); // but get menu item index here!
		//std::wstring msgW = L"menuidx=" + ToStrW(menuidx); OutputDebugStringW(msgW.c_str());
		if (menuidx) DeleteMenu(hMenuOptions, menuidx - 1, MF_BYPOSITION);
		AppendMenuW(hMenuOptions, MF_STRING | (isDragnDropEnabled() ? MF_CHECKED : MF_UNCHECKED), ID_CUSTOM_DRGNDRP, WDRAGNDR);

		menuidx = static_cast<int>(reinterpret_cast<INT_PTR>(FindSubMenuByName(reinterpret_cast<HMENU>(hMenuOptions), WSCRLBAR, false)));
		if (menuidx) DeleteMenu(hMenuOptions, menuidx - 1, MF_BYPOSITION);
		AppendMenuW(hMenuOptions, MF_STRING | (isScrollBarEnabled() ? MF_CHECKED : MF_UNCHECKED), ID_CUSTOM_SCRLBAR, WSCRLBAR);
	}


}



bool clsVTWindowInstance::isDragnDropEnabled()
{
	VTDWORD OLEFlags;

	SCCVWOPTIONSPEC40 locOptionSpec;
	locOptionSpec.dwSize = sizeof(SCCVWOPTIONSPEC40);
	locOptionSpec.dwFlags = SCCVWOPTION_CURRENT;
	locOptionSpec.dwId = SCCID_OLEFLAGS;
	locOptionSpec.pData = &OLEFlags;
	SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

	return (OLEFlags & SCCVW_OLE_ENABLEDRAGDROP);
}



void clsVTWindowInstance::SetDragnDrop(bool enable)
{
	VTDWORD OLEFlags = enable ? SCCVW_OLE_ENABLEDRAGDROP : 0;

	SCCVWOPTIONSPEC40 locOptionSpec;
	locOptionSpec.dwSize = sizeof(SCCVWOPTIONSPEC40);
	locOptionSpec.dwFlags = SCCVWOPTION_CURRENT;
	locOptionSpec.dwId = SCCID_OLEFLAGS;
	locOptionSpec.pData = &OLEFlags;
	SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
}



HMENU clsVTWindowInstance::FindSubMenuByName(const HMENU hMenu, const wchar_t* targetName, const bool isSubMenu)
// return HMENU if SubMenu exists
// return menu index+1 (use with MF_BYPOSITION) if menu item exist
// else return 0 (first menu index == 1 !!! not 0)
{
	for (int i = 0, count = GetMenuItemCount(hMenu); i < count; i++)
	{
		wchar_t menuText[CLASSNAMEMAXBUF];

		MENUITEMINFOW mii;
		ZeroMemory(&mii, sizeof(mii));
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_STRING | (isSubMenu ? MIIM_SUBMENU : 0);
		mii.dwTypeData = menuText;
		mii.cch = STRLEN(menuText);

		if (GetMenuItemInfoW(hMenu, i, TRUE, &mii))
			if (_wcsicmp(menuText, targetName) == 0)
				if (isSubMenu && mii.hSubMenu != NULL)
				{
					//std::wstring msgW = L"SubMenu idx: " + ToStrW(i) + L"\n"; OutputDebugStringW(msgW.c_str());
					return mii.hSubMenu;
				}
				else if (!isSubMenu && mii.hSubMenu == NULL)
				{
					//std::wstring msgW = L"Menu Item idx: " + ToStrW(i) + L"\n"; OutputDebugStringW(msgW.c_str());
					return reinterpret_cast<HMENU>(static_cast<INT_PTR>(i + 1));
				}
	}
	return NULL;
}



void clsVTWindowInstance::EnableScrollBar(bool enable)
{
	SCCVWOPTIONSPEC40 locOptionSpec;
	VTDWORD ScrollFlags;
	locOptionSpec.dwSize = sizeof(SCCVWOPTIONSPEC40);
	locOptionSpec.dwFlags = SCCVWOPTION_CURRENT;
	locOptionSpec.dwId = SCCID_SCROLLFLAGS;
	locOptionSpec.pData = &ScrollFlags;

	ScrollFlags = enable ? (SCCVW_HSCROLL_ALWAYS | SCCVW_VSCROLL_ALWAYS) : (SCCVW_HSCROLL_NEVER | SCCVW_VSCROLL_NEVER);
	SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
}



bool clsVTWindowInstance::isScrollBarEnabled()
{
	SCCVWOPTIONSPEC40 locOptionSpec;
	VTDWORD ScrollFlags;
	locOptionSpec.dwSize = sizeof(SCCVWOPTIONSPEC40);
	locOptionSpec.dwFlags = SCCVWOPTION_CURRENT;
	locOptionSpec.dwId = SCCID_SCROLLFLAGS;

	locOptionSpec.pData = &ScrollFlags;
	SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

	return (ScrollFlags == (SCCVW_HSCROLL_NEVER | SCCVW_VSCROLL_NEVER)) ? false : true;
}



/*
void SetSccdisplayChildWndProc(HWND waWnd)
{
	ALLMYDATA *mydata;
	mydata = (ALLMYDATA *)GetWindowLongPtr(waWnd, GWLP_USERDATA);

	mydata->SccdisplayWindow = FindWindowExA(mydata->SccviewerWindow, NULL, WNDCLASSNAME_SCCDISPLAY, NULL); // get child window
	if (mydata->SccdisplayWindow) mydata->OriginalSccdisplayWindowProc = (WNDPROC)SetWindowLongPtrA(mydata->SccdisplayWindow, GWLP_WNDPROC, (LONG_PTR)SccdisplayWindowProc);

	std::string msg;
	OutputDebugStringA("------------------------------------------------");
	OutputDebugStringA("GetSccdisplayChild:");
	msg = "SccdisplayWindow=" + std::to_string(reinterpret_cast<uintptr_t>(mydata->SccdisplayWindow));
	OutputDebugStringA(msg.c_str());

	HWND hParenWindow = GetAncestor(mydata->SccdisplayWindow, GA_PARENT);
	msg = "ParentWindow=" + std::to_string(reinterpret_cast<uintptr_t>(hParenWindow));
	OutputDebugStringA(msg.c_str());
	msg = "SccdisplayWindowProc=" + std::to_string(reinterpret_cast<uintptr_t>(SccdisplayWindowProc));
	OutputDebugStringA(msg.c_str());
	msg = "OriginalSccdisplayWindowProc=" + std::to_string(reinterpret_cast<uintptr_t>(mydata->OriginalSccdisplayWindowProc));
	OutputDebugStringA(msg.c_str());
	OutputDebugStringA("------------------------------------------------");
}
*/



///////////////////////////////////////////////////////////////////////////////////////////////////



clsFullScreen::clsFullScreen() { hWnd = NULL; }



void clsFullScreen::Init(HWND _hWnd)
{
	if (hWnd == _hWnd) return;

	hWndFSOrigParent = NULL;
	lngFSOrigWindowStyle = NULL;
	hFSOrigMenu = NULL;

	hWnd = _hWnd;
	isOrigStyleChild = WS_CHILD & GetWindowLongPtr(_hWnd, GWL_STYLE); // Or !GetParent(hWnd);
}



void clsFullScreen::EnterToFullScrMode()
{
	if (isFullScrEnabled()) return;

	hWndFSOrigParent = GetParent(hWnd);
	lngFSOrigWindowStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
	GetWindowRect(hWnd, &rectFSOrigPosition);

	if (hWndFSOrigParent) MapWindowPoints(NULL, hWndFSOrigParent, (LPPOINT)&rectFSOrigPosition, 2);

	HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO monitorInfo;
	ZeroMemory(&monitorInfo, sizeof(monitorInfo));
	monitorInfo.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(hMonitor, &monitorInfo);

	int screenX = monitorInfo.rcMonitor.left;
	int screenY = monitorInfo.rcMonitor.top;
	int screenW = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
	int screenH = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

	//std::wstring msgW = L"x=" + ToStrW(screenX) + L" y=" + ToStrW(screenY) + L" w=" + ToStrW(screenW) + L" h=" + ToStrW(screenH); OutputDebugStringW(msgW.c_str());

	SetParent(hWnd, NULL);
	SetWindowLongPtr(hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);

	hFSOrigMenu = GetMenu(hWnd);
	if (hFSOrigMenu) SetMenu(hWnd, NULL);

	SetWindowPos(hWnd, HWND_TOPMOST, screenX, screenY, screenW, screenH, SWP_FRAMECHANGED);
}



bool clsFullScreen::isFullScrEnabled()
{
	if (isOrigStyleChild) return !GetParent(hWnd);
	else return WS_EX_TOPMOST & GetWindowLongPtr(hWnd, GWL_EXSTYLE); // SetWindowPos(hWnd, HWND_TOPMOST ... if full screen
} 



void clsFullScreen::ExitFromFullScrMode()
{
	if (isFullScrEnabled())
		if (isOrigStyleChild) ExitFromFullScrMode_ChildWnd();
		else ExitFromFullScrMode_NormalWnd();
}



void clsFullScreen::ChangeFullScrMode()
{
	if (isFullScrEnabled()) ExitFromFullScrMode();
	else  EnterToFullScrMode();
}



void clsFullScreen::ExitFromFullScrMode_NormalWnd()
// without QuickView mode support
{
	SetWindowLongPtr(hWnd, GWL_STYLE, lngFSOrigWindowStyle);
	SetParent(hWnd, hWndFSOrigParent);

	if (hFSOrigMenu) { SetMenu(hWnd, hFSOrigMenu); hFSOrigMenu = NULL; }

	int oldW = rectFSOrigPosition.right - rectFSOrigPosition.left;
	int oldH = rectFSOrigPosition.bottom - rectFSOrigPosition.top;

	//std::wstring msgW = L"ox=" + ToStrW(rectFSOrigPosition.left) + L" oy=" + ToStrW(rectFSOrigPosition.top) + L" ow=" + ToStrW(oldW) + L" oh=" + ToStrW(oldH); OutputDebugStringW(msgW.c_str());

	SetWindowPos(hWnd, HWND_NOTOPMOST, rectFSOrigPosition.left, rectFSOrigPosition.top, oldW, oldH, SWP_FRAMECHANGED);
}



void clsFullScreen::ExitFromFullScrMode_ChildWnd()
// it's ugliest hack for total commander embedded quick view window.
// use it to redraw main TC window when exit from full screen mode.
{
	SetWindowLongPtr(hWnd, GWL_STYLE, lngFSOrigWindowStyle);
	SetParent(hWnd, hWndFSOrigParent);

	// for normal view mode support:
	if (hFSOrigMenu) { SetMenu(hWnd, hFSOrigMenu); hFSOrigMenu = NULL; }

	HWND hWndRoot = GetAncestor(hWndFSOrigParent, GA_ROOT);
	if (hWndRoot)
	{
		RECT rcRoot;
		GetWindowRect(hWndRoot, &rcRoot);
		int rootW = rcRoot.right - rcRoot.left;
		int rootH = rcRoot.bottom - rcRoot.top;

		if (IsZoomed(hWndRoot))
		{
			// TOTALCMD main window is Maximized (SetWindowPos for the root is blocked by the OS): Force Layout recalculation by sending messages WM_SIZE + WM_WINDOWPOSCHANGED directly
			RECT rcParent;
			GetClientRect(hWndFSOrigParent, &rcParent);
			int parentW = rcParent.right - rcParent.left;
			int parentH = rcParent.bottom - rcParent.top;

			SendMessage(hWndFSOrigParent, WM_SIZE, SIZE_RESTORED, MAKELPARAM(parentW, parentH)); // почесать за ушком

			// Create a dummy WindowPos structure to forcefully trigger the layout manager
			WINDOWPOS wp;
			ZeroMemory(&wp, sizeof(wp));
			wp.hwnd = hWndRoot;
			wp.cx = rootW;
			wp.cy = rootH;
			wp.flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED;

			SendMessage(hWndRoot, WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp); // погладить пузико
			SendMessage(hWndRoot, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rootW, rootH));
		}
		else
		{
			// TOTALCMD main window is not Maximized: Perform a proven 1-pixel micro-shift back and forth
			SetWindowPos(hWndRoot, NULL, 0, 0, rootW - 1, rootH, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
			SetWindowPos(hWndRoot, NULL, 0, 0, rootW, rootH, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
	} // hWndRoot

} // clsVTWindowInstance::ExitFromFullScrMode_QuickView
