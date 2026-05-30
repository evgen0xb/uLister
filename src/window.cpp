/* 
The plugin is provided as-is and without any warranty under the GPLv3 license.
*/

// VS 2005 FIX
#define WM_MOUSEHWHEEL                  0x020E
#define WM_MOUSEWHEEL                   0x020A
#define GET_WHEEL_DELTA_WPARAM(wParam)  ((short)HIWORD(wParam))

#include <windows.h>
#include <CommCtrl.h>
#include <float.h>
#include <lomenu.h>
#include "ulister.h"
//#include <string>

extern clsUlisterInstance	UlisterInstance;
extern clsUlisterOptions	UlisterOptions;
extern clsVTOptions			VTOptions;

const char *WNDCLASSNAME_WAWC			= "WAwc";
const char *WNDCLASSNAME_SCCVIEWER		= "SCCVIEWER";
const char *WNDCLASSNAME_SCCDISPLAY		= "SCCDISPLAY";



///////////////////////////////////////////////////////////////////////////////////////////////////
VTDWORD GetDisplayEngineVT(const HWND hWnd)
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
	SendMessage(hWnd, SCCVW_GETDISPLAYINFO, 0, (LPARAM)(PSCCVWDISPLAYINFO40)&locDisplayInfo);
	return locDisplayInfo.dwType;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
void ZoomBitmapVecFont(const HWND hWnd, const int dir)
{
	// dir =  1		- zoom in
	// dir = -1		- zoom out
	// dir =  0		- zoom reset to 100%

	ALLMYDATA *mydata;
	VTDWORD DispEng;
	mydata = (ALLMYDATA *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	SCCVWOPTIONSPEC40 locOptionSpec;
	VTDWORD zoom;
	locOptionSpec.dwSize = sizeof(SCCVWOPTIONSPEC40);
	locOptionSpec.dwFlags = SCCVWOPTION_CURRENT;
	locOptionSpec.pData = &zoom;

	DispEng = GetDisplayEngineVT(mydata->SccviewerWindow); // call only from user-level defined messages!!!
	if (DispEng == SCCVWTYPE_IMAGE)
	{
		locOptionSpec.dwId = SCCID_BMPZOOMEVENT;
		if (dir == UlisterZoom::ZRESET) zoom = SCCVW_ZOOM_RESET;
		else zoom = (dir == UlisterZoom::ZIN) ? SCCVW_ZOOM_IN : SCCVW_ZOOM_OUT;
		SendMessage(hWnd, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
	}
	else if (DispEng == SCCVWTYPE_VECTOR)
	{
		locOptionSpec.dwId = SCCID_VECZOOMEVENT;
		if (dir == UlisterZoom::ZRESET) zoom = SCCVW_ZOOM_RESET;
		else zoom = (dir == UlisterZoom::ZIN) ? SCCVW_ZOOM_IN : SCCVW_ZOOM_OUT;
		SendMessage(hWnd, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
	}
	else if (DispEng == SCCVWTYPE_WP || DispEng == SCCVWTYPE_HTML || DispEng == SCCVWTYPE_EMAIL)
	{
		// oracle bug: SCCID_FONTSCALINGFACTOR not working if SCCVW_WPMODE_PREVIEW or SCCVW_WPMODE_WEBLAYOUT mode of Word Processor / HTML / EMAIL!
		// From A.10.5 SCCID_FONTSCALINGFACTOR Note:
		// For word processor documents, this only affects normal and draft modes.

		VTDWORD WPdisplaymode;
		locOptionSpec.dwId = SCCID_WPDISPLAYMODE;
		locOptionSpec.pData = &WPdisplaymode;
		SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		VTDWORD HTMLdisplaymode;
		locOptionSpec.dwId = SCCID_HTMLDISPLAYMODE;
		locOptionSpec.pData = &HTMLdisplaymode;
		SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		VTDWORD EMAILdisplaymode;
		locOptionSpec.dwId = SCCID_EMAILDISPLAYMODE;
		locOptionSpec.pData = &EMAILdisplaymode;
		SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		locOptionSpec.pData = &zoom;
		locOptionSpec.dwId = SCCID_FONTSCALINGFACTOR;
		SendMessage(hWnd, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
		if (dir == UlisterZoom::ZRESET) zoom = 100; // percent
		else zoom = (dir == UlisterZoom::ZIN) ? zoom * 10 / 8 : zoom * 8 / 10;
		SendMessage(hWnd, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

		// workaround:
		if (DispEng == SCCVWTYPE_WP && (WPdisplaymode == SCCVW_WPMODE_PREVIEW || WPdisplaymode == SCCVW_WPMODE_WEBLAYOUT) ||
			DispEng == SCCVWTYPE_HTML && (HTMLdisplaymode == SCCVW_WPMODE_PREVIEW || HTMLdisplaymode == SCCVW_WPMODE_WEBLAYOUT) ||
			DispEng == SCCVWTYPE_EMAIL && (EMAILdisplaymode == SCCVW_WPMODE_PREVIEW || EMAILdisplaymode == SCCVW_WPMODE_WEBLAYOUT))
		{ 
			// temporarily switch to draft mode
			zoom = SCCVW_WPMODE_DRAFT;

			ShowWindow(hWnd, SW_HIDE);

			locOptionSpec.dwId = SCCID_WPDISPLAYMODE;
			SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

			locOptionSpec.dwId = SCCID_HTMLDISPLAYMODE;
			SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

			locOptionSpec.dwId = SCCID_EMAILDISPLAYMODE;
			SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

			// and switch back

			locOptionSpec.dwId = SCCID_WPDISPLAYMODE;
			locOptionSpec.pData = &WPdisplaymode;
			SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

			locOptionSpec.dwId = SCCID_HTMLDISPLAYMODE;
			locOptionSpec.pData = &HTMLdisplaymode;
			SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

			locOptionSpec.dwId = SCCID_EMAILDISPLAYMODE;
			locOptionSpec.pData = &EMAILdisplaymode;
			SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

			ShowWindow(hWnd, SW_SHOW);
		}
	}
	else if (DispEng == SCCVWTYPE_SS || DispEng == SCCVWTYPE_DB ||
		DispEng == SCCVWTYPE_HEX || DispEng == SCCVWTYPE_ARCHIVE)
	{
		locOptionSpec.dwId = SCCID_FONTSCALINGFACTOR;
		SendMessage(hWnd, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
		if (dir == UlisterZoom::ZRESET) zoom = 100; // percent
		else zoom = (dir == UlisterZoom::ZIN) ? zoom * 10 / 8 : zoom * 8 / 10;
		SendMessage(hWnd, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
	}
}



///////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK ParentWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	_control87(MCW_EM, MCW_EM);
	ALLMYDATA *mydata;
	mydata = (ALLMYDATA *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if (mydata)
		switch (message) {
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
				int _keepinmemory = UlisterOptions.keepinmemory; IniParse(); UlisterOptions.keepinmemory = _keepinmemory;
				SendVTOptions(mydata, &VTOptions);
				return 0;
			}

			PostMessage(mydata->ListerWindow, WM_KEYDOWN, lParam, 0); // TC Lister Handler: (Ctrl+F/F7, F3/Shift+F3, ESC)
			break;
		
		// WM_MOUSEHWHEEL/WM_MOUSEWHEEL mesages are available ONLY in SCCDISPLAY child window!

		case WM_TIMER:
			// update WindowProc adress of SCCDISPLAY child window if the sheet change (after event on SCCSECLIST child window)

			// to hook MOUSEHWHEEL:

			//	TLister Window
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
			if (GetKeyState(VK_CONTROL) < 0 && DisplayEngineType != SCCVWTYPE_IMAGE && DisplayEngineType != SCCVWTYPE_VECTOR)
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
HWND CreateListerWindow(HWND ParentWin)
{
	HWND        hViewWnd, waWnd;
	RECT		r;
	WNDCLASS	wc;
	ALLMYDATA	*mydata;

	if (!UlisterInstance.ViewerLibraryInstanceInc()) return NULL;

	mydata = new ALLMYDATA();

	mydata->ListerWindow = ParentWin;

	bool quickview = WS_CHILD & GetWindowLongPtr(ParentWin, GWL_STYLE);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)ParentWindowProc;
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
	waWnd = CreateWindow	(WNDCLASSNAME_WAWC,		NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, r.left, r.top, r.right - r.left, r.bottom - r.top, ParentWin,	0, UlisterInstance.hInstWLX, NULL);
	mydata->waWindow = waWnd;

	GetClientRect(waWnd, &r);
	hViewWnd = CreateWindow(WNDCLASSNAME_SCCVIEWER,	NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, r.left, r.top, r.right - r.left, r.bottom - r.top, waWnd,		0, UlisterInstance.hInstWLX, NULL);
	mydata->SccviewerWindow = hViewWnd;

	if (!IsWindow(hViewWnd)) return NULL;
	mydata->OriginalSccviewerWindowProc = (WNDPROC)SetWindowLongPtr(hViewWnd, GWLP_WNDPROC, (LONG_PTR)SccviewerWindowProc);
	// exception with Delphi 12 SetWindowLongPtr(hViewWnd, GWLP_USERDATA,(long) mydata);
	// exception with Delphi 12 SetWindowLongPtr(waWnd, GWLP_USERDATA,(long) mydata);
	SetWindowLongPtr(hViewWnd, GWLP_USERDATA, (LONG_PTR)mydata);
	SetWindowLongPtr(waWnd, GWLP_USERDATA, (LONG_PTR)mydata);
	
	if (!quickview) SetFocus(hViewWnd);
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


	