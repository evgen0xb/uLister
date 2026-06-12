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
void ChangeViewMode(const HWND hWnd, const int dir)
{
	// dir =  1		- next view mode
	// dir = -1		- prev view mode

	LPCWSTR AUNK		= L"Unknown";
	LPCWSTR ADRAFT		= L"Draft";
	LPCWSTR ANORMAL		= L"Normal";
	LPCWSTR APREVIEW	= L"Preview";
	LPCWSTR AWEBLAY		= L"Weblayout";
	LPCWSTR AHIDDEN		= L"Hidden";
	LPCWSTR ANONE		= L"None";
	LPCWSTR ANAME		= L"Name";
	LPCWSTR ASIZE		= L"Size";
	LPCWSTR ADATE		= L"Date";
	LPCWSTR A0			= L"0\u00B0";
	LPCWSTR A90			= L"90\u00B0";
	LPCWSTR A180		= L"180\u00B0";
	LPCWSTR A270		= L"270\u00B0";

	LPCWSTR VIEWMODENAME = AUNK;

	ALLMYDATA *mydata;
	VTDWORD DispEng;
	mydata = (ALLMYDATA *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

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

	DispEng = GetDisplayEngineVT(mydata->SccviewerWindow); // call only from user-level defined messages!!!
	if (DispEng == SCCVWTYPE_WP || DispEng == SCCVWTYPE_HTML || DispEng == SCCVWTYPE_EMAIL)
	{
		// word processor: draft->normal->preview->weblayout

		if (DispEng == SCCVWTYPE_WP) locOptionSpec.dwId = SCCID_WPDISPLAYMODE;
		else locOptionSpec.dwId = (DispEng == SCCVWTYPE_HTML) ? SCCID_HTMLDISPLAYMODE : SCCID_EMAILDISPLAYMODE;

		SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		if (dir == UlisterNextMode::MNEXT) { viewmode++; if (viewmode > SCCVW_WPMODE_WEBLAYOUT) viewmode = SCCVW_WPMODE_WEBLAYOUT; }
		else { viewmode--; if (viewmode < SCCVW_WPMODE_DRAFT) viewmode = SCCVW_WPMODE_DRAFT; }

		SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

		if (viewmode == SCCVW_WPMODE_DRAFT) VIEWMODENAME = ADRAFT;
		else if (viewmode == SCCVW_WPMODE_NORMAL) VIEWMODENAME = ANORMAL;
		else if (viewmode == SCCVW_WPMODE_PREVIEW) VIEWMODENAME = APREVIEW;
		else if (viewmode == SCCVW_WPMODE_WEBLAYOUT) VIEWMODENAME = AWEBLAY;
		else VIEWMODENAME = AUNK;

		mydata->BalloonTip.InitPosition(mydata->TListerWindow, BALLOONTIP_XOFFS, BALLOONTIP_YOFFS, BALLOONTIP_WIDTH, BALLOONTIP_HEIGHT);
		mydata->BalloonTip.ShowTemporaryMessage(VIEWMODENAME, UlisterInstance.BalloonTipTimer);
	}
	else if (DispEng == SCCVWTYPE_SS)
	{
		// SCCID_SSSHOWGRIDLINES ???

		// spreadsheet: draft->normal->normal with hidden rows and columns displayed

		VTBOOL spreadsheethiddencells;

		//locOptionSpec.pData = &spreadsheetdraft;
		locOptionSpec.dwId = SCCID_SSDRAFTMODE;
		SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		locOptionSpec.pData = &spreadsheethiddencells;
		locOptionSpec.dwId = SCCID_SSSHOWHIDDENCELLS;
		SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

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
		SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		locOptionSpec.pData = &spreadsheetdraft;
		locOptionSpec.dwId = SCCID_SSDRAFTMODE;
		SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		mydata->BalloonTip.InitPosition(mydata->TListerWindow, BALLOONTIP_XOFFS, BALLOONTIP_YOFFS, BALLOONTIP_WIDTH, BALLOONTIP_HEIGHT);
		mydata->BalloonTip.ShowTemporaryMessage(VIEWMODENAME, UlisterInstance.BalloonTipTimer);
	}
	else if (DispEng == SCCVWTYPE_ARCHIVE)
	{
		// NONE->NAME->SIZE->DATE

		//locOptionSpec.pData = &arcsortorder;
		locOptionSpec.dwId = SCCID_ARCSORTORDER;
		SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		if (dir == UlisterNextMode::MNEXT) { arcsortorder++; if (arcsortorder > SCCVW_SORT_DATE) arcsortorder = SCCVW_SORT_DATE; }
		else { arcsortorder--; if (arcsortorder < SCCVW_SORT_NONE) arcsortorder = SCCVW_SORT_NONE; }

		SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
		// This option is saved in the .oit directory and does need to be reset to SCCVW_SORT_NAME when loading an archive file (see SendVTOptions function).

		if (arcsortorder == SCCVW_SORT_NONE) VIEWMODENAME = ANONE;
		else if (arcsortorder == SCCVW_SORT_NAME) VIEWMODENAME = ANAME;
		else if (arcsortorder == SCCVW_SORT_SIZE) VIEWMODENAME = ASIZE;
		else if (arcsortorder == SCCVW_SORT_DATE) VIEWMODENAME = ADATE;
		else VIEWMODENAME = AUNK;

		mydata->BalloonTip.InitPosition(mydata->TListerWindow, BALLOONTIP_XOFFS, BALLOONTIP_YOFFS, BALLOONTIP_WIDTH, BALLOONTIP_HEIGHT);
		mydata->BalloonTip.ShowTemporaryMessage(VIEWMODENAME, UlisterInstance.BalloonTipTimer);
	}
	else if (DispEng == SCCVWTYPE_IMAGE)
	{
		// SCCID_ANTIALIAS ??? SCCVW_ANTIALIAS_OFF | SCCVW_ANTIALIAS_ALL ???

		// rotate 0->90->180->270->0->...
		
		//locOptionSpec.pData = &bitmaprotation;
		locOptionSpec.dwId = SCCID_BMPROTATION;
		SendMessage(mydata->SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);

		if (dir == UlisterNextMode::MNEXT) { bitmaprotation = bitmaprotation + 90; if (bitmaprotation > SCCVW_ROTATION_270) bitmaprotation = SCCVW_ROTATION_NONE; }
		else { bitmaprotation = bitmaprotation - 90; if (bitmaprotation > SCCVW_ROTATION_270) bitmaprotation = SCCVW_ROTATION_270; } // unsigned int (VTDWORD) overflow hack

		SendMessage(mydata->SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
		// It seems that this option is not saved in the .oit directory and does not need to be reset to SCCVW_ROTATION_NONE when loading an image file.

		if (bitmaprotation == SCCVW_ROTATION_NONE) VIEWMODENAME = A0;
		else if (bitmaprotation == SCCVW_ROTATION_90) VIEWMODENAME = A90;
		else if (bitmaprotation == SCCVW_ROTATION_180) VIEWMODENAME = A180;
		else if (bitmaprotation == SCCVW_ROTATION_270) VIEWMODENAME = A270;
		else VIEWMODENAME = AUNK;

		mydata->BalloonTip.InitPosition(mydata->TListerWindow, BALLOONTIP_XOFFS, BALLOONTIP_YOFFS, BALLOONTIP_WIDTH, BALLOONTIP_HEIGHT);
		mydata->BalloonTip.ShowTemporaryMessage(VIEWMODENAME, UlisterInstance.BalloonTipTimer);
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
void ZoomBitmapVecFont(const HWND hWnd, const int dir)
{
	// dir =  1		- zoom in
	// dir = -1		- zoom out
	// dir =  0		- zoom reset to 100% (146% in Russian Federation)

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
			mydata->BalloonTip.Move();
			break;
		case WM_MOVE:
			//OutputDebugStringA("WM_MOVE");
			mydata->BalloonTip.Move();
			break;
		case WM_TIMER:
			//OutputDebugStringA("WM_TIMER");
			if (wParam == mydata->BalloonTip.nIDEvent) mydata->BalloonTip.DestroyTemporaryMessage();
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
				return 0;
			}

			if ((GetKeyState(VK_CONTROL) < 0) && (lParam == 'M'))
			{
				// OutputDebugStringA("Prev/Next View Mode");
				if (GetKeyState(VK_SHIFT) < 0) ChangeViewMode(mydata->SccviewerWindow, UlisterNextMode::MPREV);
				else ChangeViewMode(mydata->SccviewerWindow, UlisterNextMode::MNEXT);
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
HWND CreateListerWindow(HWND ParentWin)
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
	waWnd = CreateWindow	(WNDCLASSNAME_WAWC,		NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, r.left, r.top, r.right - r.left, r.bottom - r.top, ParentWin,	0, UlisterInstance.hInstWLX, NULL);

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


	