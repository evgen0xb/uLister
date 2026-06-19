/* 
The plugin is provided as-is and without any warranty under the GPLv3 license.
*/

#include <windows.h>
#include <CommCtrl.h>
#include <float.h>
#include <lomenu.h>

#include "ulister.h"
#include "window.h"
#include "init.h"
#include "utils.h"



extern clsUlisterInstance	UlisterInstance;
extern clsUlisterOptions	UlisterOptions;
extern clsVTOptions			VTOptions;

const char *WNDCLASSNAME_WAWC			= "WAwc";
const char *WNDCLASSNAME_SCCVIEWER		= "SCCVIEWER";
const char *WNDCLASSNAME_SCCDISPLAY		= "SCCDISPLAY";



LRESULT CALLBACK SccdisplayWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);



///////////////////////////////////////////////////////////////////////////////////////////////////



LRESULT CALLBACK TListerWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	_control87(MCW_EM, MCW_EM);
	ALLMYDATA *mydata;
	mydata = (ALLMYDATA *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

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
		}
		return CallWindowProc(mydata->OriginalTListerWindowProc, hWnd, message, wParam, lParam); // Ghisler Handler
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}



///////////////////////////////////////////////////////////////////////////////////////////////////



LRESULT CALLBACK WAwcWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	_control87(MCW_EM, MCW_EM);
	ALLMYDATA *mydata;
	mydata = (ALLMYDATA *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

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
		}
	return DefWindowProc(hWnd, message, wParam, lParam);
}



///////////////////////////////////////////////////////////////////////////////////////////////////



LRESULT CALLBACK SccviewerWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND _SccdisplayWindow;
	WNDPROC _SccdisplayWindowProc;

	_control87(MCW_EM, MCW_EM);
	ALLMYDATA *mydata;
	mydata = (ALLMYDATA *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
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
			if ((GetKeyState(VK_CONTROL) < 0) && ((lParam == VK_OEM_PLUS) || (lParam == VK_ADD))) { ZoomBitmapVecFont(hWnd, UlisterZoom::ZIN); return 0; }
			if ((GetKeyState(VK_CONTROL) < 0) && ((lParam == VK_OEM_MINUS) || (lParam == VK_SUBTRACT))) { ZoomBitmapVecFont(hWnd, UlisterZoom::ZOUT); return 0; }
			if ((GetKeyState(VK_CONTROL) < 0) && ((lParam == VK_MULTIPLY) || (lParam == '8'))) { ZoomBitmapVecFont(hWnd, UlisterZoom::ZRESET); return 0; }

			if ((GetKeyState(VK_CONTROL) < 0) && (GetKeyState(VK_SHIFT) < 0) && (lParam == 'R'))
			{
				// OutputDebugStringA("Reload ini-file");
				bool _keepinmemory = UlisterOptions.keepinmemory; IniParse(); UlisterOptions.keepinmemory = _keepinmemory;
				SendVTOptions(mydata, &VTOptions);

				mydata->ToolTip.InitPosition(mydata->TListerWindow, TOOLTIP_XOFFS, TOOLTIP_YOFFS, TOOLTIP_WIDTH, TOOLTIP_HEIGHT);
				mydata->ToolTip.ShowTemporaryMessage(L"Reload", UlisterOptions.ToolTipTransparency, UlisterOptions.ToolTipTimer);

				return 0;
			}

			if ((GetKeyState(VK_CONTROL) < 0) && (lParam == 'M'))
			{
				// OutputDebugStringA("Prev/Next View Mode");
				if (GetKeyState(VK_SHIFT) < 0) ChangeViewMode(mydata->SccviewerWindow, UlisterNextMode::MPREV);
				else ChangeViewMode(mydata->SccviewerWindow, UlisterNextMode::MNEXT);
				return 0;
			}
			if ((GetKeyState(VK_CONTROL) < 0) && (lParam == 'I'))
			{
				//OutputDebugStringA("File Info Window");
				if (mydata->InfoWindow.CreateWnd(UlisterInstance.hInstWLX, mydata->TListerWindow)) SetActiveWindow(mydata->InfoWindow.hwndFileInfo);
				else mydata->InfoWindow.Show();
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
			break;
		}
		return CallWindowProc(mydata->OriginalSccviewerWindowProc, hWnd, message, wParam, lParam); // OIT Handler
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}



///////////////////////////////////////////////////////////////////////////////////////////////////



LRESULT CALLBACK SccdisplayWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	_control87(MCW_EM, MCW_EM);
	HWND SccviewerWindow = GetAncestor(hWnd, GA_PARENT); // get parent window
	ALLMYDATA *mydata;
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

	mydata = (ALLMYDATA *)GetWindowLongPtr(SccviewerWindow, GWLP_USERDATA);
	if (mydata)
	{
		switch (message)
		{
		case WM_MOUSEHWHEEL:
			if (UlisterOptions.mwhscrollinvert)
				PostMessage(mydata->SccviewerWindow, SCCVW_HSCROLL, (GET_WHEEL_DELTA_WPARAM(wParam) > 0) ? SCCSB_LINELEFT : SCCSB_LINERIGHT, 0); // invert
			else
				PostMessage(mydata->SccviewerWindow, SCCVW_HSCROLL, (GET_WHEEL_DELTA_WPARAM(wParam) > 0) ? SCCSB_LINERIGHT : SCCSB_LINELEFT, 0);
			return 0;
		case WM_MOUSEWHEEL:
			// OutputDebugStringA("***WM_MOUSEWHEEL***");
			DisplayEngineType = GetDisplayEngineVT(mydata->SccviewerWindow);
			if (GetKeyState(VK_CONTROL) < 0 && GetKeyState(VK_SHIFT) < 0)
			{
				// CTRL+SHIFT+MOUSEHWHEEL : Next/Prev View Mode (ex. for Word Processor, HTML and EMAIL: draft->normal->preview->weblayout->draft)
				if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) ChangeViewMode(mydata->SccviewerWindow, UlisterNextMode::MPREV); else ChangeViewMode(mydata->SccviewerWindow, UlisterNextMode::MNEXT);
				return 0;
			}
			else if (GetKeyState(VK_CONTROL) < 0 && DisplayEngineType != SCCVWTYPE_IMAGE && DisplayEngineType != SCCVWTYPE_VECTOR)
			{
				// CTRL+MOUSEHWHEEL : Zoom-In/Zoom-Out
				if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) ZoomBitmapVecFont(mydata->SccviewerWindow, UlisterZoom::ZIN); else ZoomBitmapVecFont(mydata->SccviewerWindow, UlisterZoom::ZOUT);
				return 0;
			}
			else if (GetKeyState(VK_SHIFT) < 0)
			{
				// SHIFT+MOUSEHWHEEL : horizontal scroll for spreadsheet and database
				if (UlisterOptions.mwhscrollinvert)
					PostMessage(mydata->SccviewerWindow, SCCVW_HSCROLL, (GET_WHEEL_DELTA_WPARAM(wParam) > 0) ? SCCSB_LINELEFT : SCCSB_LINERIGHT, 0); // invert
				else
					PostMessage(mydata->SccviewerWindow, SCCVW_HSCROLL, (GET_WHEEL_DELTA_WPARAM(wParam) > 0) ? SCCSB_LINERIGHT : SCCSB_LINELEFT, 0);
				return 0;
			}
			// MOUSEHWHEEL : Up-Down scroll
			break;
		case WM_MBUTTONDOWN:
			//OutputDebugStringA("WM_MBUTTONDOWN");
			DisplayEngineType = GetDisplayEngineVT(mydata->SccviewerWindow);
			if (wParam & MK_CONTROL) { ZoomBitmapVecFont(mydata->SccviewerWindow, UlisterZoom::ZRESET); return 0; }
			break;
		}
		return CallWindowProc(mydata->OriginalSccdisplayWindowProc, hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}



///////////////////////////////////////////////////////////////////////////////////////////////////



HWND CreateListerWindow(HWND ParentWin) // TODO + UlisterInstance.hInstWLX
{
	HWND        SccviewerWnd, waWnd;
	RECT		r;
	WNDCLASS	wc;

	bool quickview = WS_CHILD & GetWindowLongPtr(ParentWin, GWL_STYLE);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WAwcWindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = UlisterInstance.hInstWLX;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WNDCLASSNAME_WAWC;
	RegisterClass(&wc);

	GetClientRect(ParentWin, &r);
	waWnd = CreateWindow		(WNDCLASSNAME_WAWC,		NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, r.left, r.top, r.right - r.left, r.bottom - r.top, ParentWin,	0, UlisterInstance.hInstWLX, NULL);

	GetClientRect(waWnd, &r);
	SccviewerWnd = CreateWindow(WNDCLASSNAME_SCCVIEWER,	NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, r.left, r.top, r.right - r.left, r.bottom - r.top, waWnd,		0, UlisterInstance.hInstWLX, NULL);

	if (!IsWindow(SccviewerWnd)) return NULL;

	ALLMYDATA *mydata = new ALLMYDATA(); // if NULL ... TODO: *mydata = new (std::nothrow) MyClass(); if (mydata == NULL) ...

	mydata->TListerWindow = ParentWin;
	mydata->waWindow = waWnd;
	mydata->SccviewerWindow = SccviewerWnd;

	mydata->OriginalSccviewerWindowProc = (WNDPROC)SetWindowLongPtr(SccviewerWnd, GWLP_WNDPROC, (LONG_PTR)SccviewerWindowProc);
	mydata->OriginalTListerWindowProc = (WNDPROC)SetWindowLongPtr(ParentWin, GWLP_WNDPROC, (LONG_PTR)TListerWindowProc);

	//std::wstring msgW = L"CreateListerWindow: OriginalTListerWindowProc=" + ToHexW(mydata->OriginalTListerWindowProc);
	//OutputDebugStringW(msgW.c_str());

	// exception with Delphi 12 SetWindowLongPtr(hViewWnd, GWLP_USERDATA,(long) mydata);
	// exception with Delphi 12 SetWindowLongPtr(waWnd, GWLP_USERDATA,(long) mydata);
	SetWindowLongPtr(SccviewerWnd, GWLP_USERDATA, (LONG_PTR)mydata);
	SetWindowLongPtr(waWnd, GWLP_USERDATA, (LONG_PTR)mydata);
	SetWindowLongPtr(ParentWin, GWLP_USERDATA, (LONG_PTR)mydata);

	if (!quickview) SetFocus(SccviewerWnd);
	return waWnd;
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


	