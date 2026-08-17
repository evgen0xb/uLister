/*
The plugin is provided as-is and without any warranty under the GPLv3 license.
*/

#ifndef UNICODE
#define UNICODE
#endif

// vs2005 fix
#define _WIN32_WINNT 0x0501
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

// vs2005 fix
#include <shellapi.h>
#include <stdlib.h>
#include <commdlg.h>

#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

#pragma comment(lib, "user32.lib")



#include "ulister.h"
#include "infowindow.h"
#include "utils.h"
#include "sharedinstance.h"
#include "window.h"



#define ID_BUTTON_EXPORT 101
#define ID_EDIT_TEXT     102

#define ID_CUSTOM_COPY      10001
#define ID_CUSTOM_SELECTALL 10002

#define SYSSTRMAXBUF	64
#define SYSSTRID_COPY	25
#define SYSSTRID_SELALL	29



const wchar_t FILEINFOWINDOWCLASS[] = L"FileInfoWindowClass";



void ShowSaveFileDialog(HWND hwndOwner);
LRESULT CALLBACK FileInfoWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK ButtonSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);



LPWSTR GetSystemString(UINT stringId)
{
	HMODULE hUser32 = GetModuleHandle(L"user32.dll");
	static WCHAR buffer[SYSSTRMAXBUF]; // "&Copy\tCtrl+C"
	if (hUser32 && LoadStringW(hUser32, stringId, buffer, STRLEN(buffer)) > 0) return buffer;

	// Fallback strings if resource loading fails
	if (stringId == 25) return (LPWSTR)L"Copy";
	else if (stringId == 29) return (LPWSTR)L"Select All";
	else return (LPWSTR)L"UNKNOWN";
}


clsInfoWindow::clsInfoWindow()
{
	hEdit = NULL;
	hButton = NULL;
	hWindowIcon = NULL;
	pText = NULL;
	minWindowWidth = 0;
	minWindowHeight = 0;

	hwndFileInfo = NULL;
	hwndParentWindow = NULL;
}



clsInfoWindow::~clsInfoWindow() { Done(); }



void clsInfoWindow::Done()
{
	if (pText) free(pText);
	if (hWindowIcon) DestroyIcon(hWindowIcon);
	hWindowIcon = NULL;
	pText = NULL;
}



void clsInfoWindow::Init(LPCWSTR textToDisplay, const int _minWindowWidth, const int _minWindowHeight)
{
	Done(); // ListLoadNextW memory leak fix
	pText = _wcsdup(textToDisplay);
	minWindowWidth = _minWindowWidth;
	minWindowHeight = _minWindowHeight;
}



void clsInfoWindow::CenterChildPosition(const HWND hwndParent, int *_X, int *_Y)
{
	// This function should support multiple monitors, but I haven't tested this configuration because I only have one monitor.

	RECT rcParent;
	GetWindowRect(hwndParent, &rcParent);
	RECT rcVisibleParent = rcParent;

	HMONITOR hMonitor = MonitorFromWindow(hwndParent, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi;
	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(MONITORINFO);

	bool hasMonitorInfo = GetMonitorInfo(hMonitor, &mi);

	if (hasMonitorInfo)
	{
		// Clip the parent window coordinates by the monitor's work area boundaries
		// This finds the intersection between the parent window and the screen (the actual visible part)
		if (rcVisibleParent.left   < mi.rcWork.left)   rcVisibleParent.left = mi.rcWork.left;
		if (rcVisibleParent.right  > mi.rcWork.right)  rcVisibleParent.right = mi.rcWork.right;
		if (rcVisibleParent.top    < mi.rcWork.top)    rcVisibleParent.top = mi.rcWork.top;
		if (rcVisibleParent.bottom > mi.rcWork.bottom) rcVisibleParent.bottom = mi.rcWork.bottom;
	}

	int visibleWidth = rcVisibleParent.right - rcVisibleParent.left;
	int visibleHeight = rcVisibleParent.bottom - rcVisibleParent.top;

	// Fallback: If the parent window is completely off-screen (dimensions became <= 0),
	// revert to the original parent coordinates to prevent broken calculations
	if (visibleWidth <= 0 || visibleHeight <= 0)
	{
		visibleWidth = rcParent.right - rcParent.left;
		visibleHeight = rcParent.bottom - rcParent.top;
		rcVisibleParent = rcParent;
	}

	*_X = rcVisibleParent.left + (visibleWidth - minWindowWidth) / 2;
	*_Y = rcVisibleParent.top + (visibleHeight - minWindowHeight) / 2;

	// If the child window is larger than the visible area
	// and still spills over the monitor edges, clamp it strictly inside the work area
	if (hasMonitorInfo)
	{
		if (*_X + minWindowWidth > mi.rcWork.right)  *_X = mi.rcWork.right - minWindowWidth;
		if (*_X < mi.rcWork.left)                    *_X = mi.rcWork.left;
		if (*_Y + minWindowHeight > mi.rcWork.bottom) *_Y = mi.rcWork.bottom - minWindowHeight;
		if (*_Y < mi.rcWork.top)                     *_Y = mi.rcWork.top;
	}
}



bool clsInfoWindow::CreateWnd(const HINSTANCE hInstance, const HWND hwndParent)
// return true if window is already exist else false
{
#ifdef ULISTER64
	const wchar_t *Title = L"uLister x64 File info";
#else
	const wchar_t *Title = L"uLister x86 File info";
#endif

	if (hwndFileInfo) return true;

	WNDCLASSW wc; ZeroMemory(&wc, sizeof(wc));

	wc.lpfnWndProc = FileInfoWindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = FILEINFOWINDOWCLASS;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	if (!hWindowIcon) hWindowIcon = ExtractIconA(hInstance, "shell32.dll", 221);
	wc.hIcon = hWindowIcon;

	RegisterClassW(&wc);

	int newX, newY;
	CenterChildPosition(hwndParent, &newX, &newY);

	hwndFileInfo = CreateWindowExW(
		0, FILEINFOWINDOWCLASS,
		Title,
		WS_OVERLAPPEDWINDOW ^ WS_MINIMIZEBOX,
		newX, newY, minWindowWidth, minWindowHeight,
		hwndParent, NULL,
		hInstance, (LPVOID)this); // WM_NCCREATE -> SetWindowLongPtr(this);

	//DWORD errorCode = GetLastError();
	//std::wstring msgW = L"CreateWindowExW=" + ToHexW(errorCode);
	//OutputDebugStringW(msgW.c_str());


	if (hwndFileInfo == NULL) return false;

	if (hWindowIcon)
	{
		SendMessageW(hwndFileInfo, WM_SETICON, ICON_SMALL, (LPARAM)hWindowIcon);
		SendMessageW(hwndFileInfo, WM_SETICON, ICON_BIG, (LPARAM)hWindowIcon);
	}


	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

	hEdit = CreateWindowExW(
		WS_EX_CLIENTEDGE,
		L"EDIT",
		pText,
		WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | WS_VSCROLL | WS_TABSTOP,
		0, 0, 0, 0,
		hwndFileInfo,
		(HMENU)ID_EDIT_TEXT,
		GetModuleHandle(NULL), NULL);

	//errorCode = GetLastError();
	//msgW = L"EDIT=" + ToHexW(hEdit);
	//OutputDebugStringW(msgW.c_str());
	//msgW = L"EDIT ERR=" + ToHexW(errorCode);
	//OutputDebugStringW(msgW.c_str());

	SendMessageW(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
	SetWindowSubclass(hEdit, EditSubclassProc, 0, 0);

	hButton = CreateWindowExW(
		0,
		L"BUTTON",
		L"Export formats",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
		0, 0, 0, 0,
		hwndFileInfo,
		(HMENU)ID_BUTTON_EXPORT, GetModuleHandle(NULL), NULL);
	SendMessageW(hButton, WM_SETFONT, (WPARAM)hFont, TRUE);
	SetWindowSubclass(hButton, ButtonSubclassProc, 0, 0);

	SetFocus(hEdit);

	hwndParentWindow = hwndParent;
	EnableWindow(hwndParentWindow, FALSE);

	return false;
}



void clsInfoWindow::WMDestroyHandler()
// WM_DESTROY:
{
	RemoveWindowSubclass(hEdit, EditSubclassProc, 0);
	RemoveWindowSubclass(hButton, ButtonSubclassProc, 0);

	EnableWindow(hwndParentWindow, TRUE);
	SetFocus(hwndParentWindow);

	hwndFileInfo = NULL;
}



void clsInfoWindow::WMSizeHandler(const int clientWidth, const int clientHeight)
// WM_SIZE:
{
	const int padding = 4;
	const int btnWidth = 110;
	const int btnHeight = 30;
	const int spaceBetween = 6;

	int btnX = clientWidth - btnWidth - padding;
	int btnY = clientHeight - btnHeight - padding;
	MoveWindow(hButton, btnX, btnY, btnWidth, btnHeight, TRUE);

	int editX = padding;
	int editY = padding;
	int editWidth = clientWidth - (padding * 2);
	int editHeight = clientHeight - (padding * 2) - btnHeight - spaceBetween;
	MoveWindow(hEdit, editX, editY, editWidth, editHeight, TRUE);
}



void clsInfoWindow::Show() { if (hwndFileInfo) { ShowWindow(hwndFileInfo, SW_SHOWNORMAL); UpdateWindow(hwndFileInfo); } }
void clsInfoWindow::Hide() { if (hwndFileInfo) ShowWindow(hwndFileInfo, SW_HIDE); }



LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	HWND hParent = GetParent(hWnd);
	clsInfoWindow* pInfoWindow = (clsInfoWindow*)GetWindowLongPtrW(hParent, GWLP_USERDATA);

	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) { SendMessageW(hParent, WM_CLOSE, 0, 0); return 0; }
		if (wParam == VK_TAB && pInfoWindow) { SetFocus(pInfoWindow->hButton); return 0; }
		break;
	case WM_CONTEXTMENU:
	{
		// custom pop-up menu
		HMENU hMenu = CreatePopupMenu();
		if (hMenu)
		{
			AppendMenuW(hMenu, MF_STRING, ID_CUSTOM_COPY, GetSystemString(SYSSTRID_COPY));
			AppendMenuW(hMenu, MF_STRING, ID_CUSTOM_SELECTALL, GetSystemString(SYSSTRID_SELALL));

			// disable "Copy" if no text is selected
			DWORD startSel = 0, endSel = 0;
			SendMessageW(hWnd, EM_GETSEL, (WPARAM)&startSel, (LPARAM)&endSel);
			if (startSel == endSel) EnableMenuItem(hMenu, ID_CUSTOM_COPY, MF_BYCOMMAND | MF_GRAYED);

			int xPos = LOWORD(lParam);
			int yPos = HIWORD(lParam);

			// keyboard Shift+F10 or Menu key
			if (xPos == -1 && yPos == -1)
			{
				RECT rect;
				GetWindowRect(hWnd, &rect);
				xPos = rect.left + 10;
				yPos = rect.top + 10;
			}

			// display menu and capture user selection
			TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, xPos, yPos, 0, hWnd, NULL);
			DestroyMenu(hMenu);
		}
		return 0;
	}
	case WM_COMMAND:
	{
		// handle custom menu actions: call native actions
		switch (LOWORD(wParam))
		{
		case ID_CUSTOM_COPY:
			SendMessageW(hWnd, WM_COPY, 0, 0);
			return 0;
		case ID_CUSTOM_SELECTALL:
			SendMessageW(hWnd, EM_SETSEL, 0, -1);
			return 0;
		}
		break;
	}
	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}



LRESULT CALLBACK ButtonSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	HWND hParent = GetParent(hWnd);
	clsInfoWindow* pInfoWindow = (clsInfoWindow*)GetWindowLongPtrW(hParent, GWLP_USERDATA);

	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_RETURN) { SendMessageW(hParent, WM_COMMAND, MAKEWPARAM(ID_BUTTON_EXPORT, BN_CLICKED), (LPARAM)hWnd); return 0; }
		if (wParam == VK_ESCAPE) { SendMessageW(hParent, WM_CLOSE, 0, 0); return 0; }
		if (wParam == VK_TAB && pInfoWindow) { SetFocus(pInfoWindow->hEdit); return 0; }
		break;
	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}



LRESULT CALLBACK FileInfoWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	clsInfoWindow* pInfoWindow = (clsInfoWindow*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

	switch (uMsg)
	{
	case WM_NCCREATE:
	{
		LPCREATESTRUCTW pCreateStruct = (LPCREATESTRUCTW)lParam;
		clsInfoWindow* pInfoWindowFromCreateWnd = (clsInfoWindow*)pCreateStruct->lpCreateParams;
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)pInfoWindowFromCreateWnd);
		return DefWindowProcW(hwnd, uMsg, wParam, lParam);
	}
	case WM_GETMINMAXINFO:
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		if (pInfoWindow)
		{
			lpMMI->ptMinTrackSize.x = pInfoWindow->minWindowWidth;
			lpMMI->ptMinTrackSize.y = pInfoWindow->minWindowHeight;
		}
		return 0;
	}

	case WM_SIZE:
		if (pInfoWindow) pInfoWindow->WMSizeHandler(LOWORD(lParam), HIWORD(lParam));
		return 0;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) { SendMessageW(hwnd, WM_CLOSE, 0, 0); return 0; }
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == ID_BUTTON_EXPORT)	{ ShowSaveFileDialog(hwnd); return 0; }
		return 0;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;

	case WM_DESTROY:
		if (pInfoWindow) pInfoWindow->WMDestroyHandler();
		return 0;
	}
	return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}



// ***********************************************************************************************************************************************************



HWND FindChildWindowByClass(HWND hParent, const wchar_t* szClassName)
{
	HWND hChild = FindWindowExW(hParent, NULL, szClassName, NULL);
	if (hChild) return hChild;

	hChild = FindWindowExW(hParent, NULL, NULL, NULL); // find first node
	while (hChild)
	{
		HWND hFound = FindChildWindowByClass(hChild, szClassName);
		if (hFound) return hFound;

		hChild = FindWindowExW(hParent, hChild, NULL, NULL); // find next node
	}
	return NULL;
}



#ifdef ULISTER64
const wchar_t szDefFileName[] = L"formats64";
#else
const wchar_t szDefFileName[] = L"formats32";
#endif

const wchar_t* Extensions[] = { L".txt", L".csv", L".csv", L".json", L"" }; // The last empty line is "All files (*.*)" and do nothing



UINT_PTR CALLBACK GetSaveFileNameHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	if (uiMsg == WM_NOTIFY)
	{
		LPOFNOTIFYW lpOfn = (LPOFNOTIFYW)lParam;

		if (lpOfn->hdr.code == CDN_TYPECHANGE || lpOfn->hdr.code == CDN_FOLDERCHANGE)
		{
			HWND hParent = GetParent(hdlg);
			wchar_t szFileName[MAX_PATH];
			szFileName[0] = L'\0';

			const int extIndex = (lpOfn->lpOFN->nFilterIndex) - 1;
			const size_t maxElements = sizeof(Extensions) / sizeof(Extensions[0]);

			if (extIndex >= 0 && (size_t)extIndex < maxElements - 1)
			{
				const wchar_t* szNewExt = Extensions[extIndex];

				SendMessageW(hParent, CDM_GETSPEC, MAX_PATH, (LPARAM)szFileName);

				if (!szFileName[0])
				{
					wcscpy_s(szFileName, MAX_PATH, szDefFileName);
					wcscat_s(szFileName, MAX_PATH, szNewExt);
				}
				else
				{
					wchar_t* pDot = wcsrchr(szFileName, L'.');
					if (pDot) wcscpy_s(pDot, MAX_PATH - (pDot - szFileName), szNewExt);
					else wcscat_s(szFileName, MAX_PATH, szNewExt);
				}

				HWND hEditCtrl = FindChildWindowByClass(hParent, L"Edit");
				if (hEditCtrl) SendMessageW(hEditCtrl, WM_SETTEXT, 0, (LPARAM)szFileName); // fix Windows bug
			}
		}
	}
	return 0;
} // GetSaveFileNameHookProc



/*

https://github.com/evgen0xb/uLister/issues/10
¬ообще-то formats.txt был изначально задуман так, чтобы его можно было буквально одной строчкой батника сконвертировать в CSV-файл:

ECHO.Format Number;DLL Name;Format Name> form_csv.csv
FOR /F "tokens=1,2,3,*" %%A IN (formats.txt) DO ECHO.%%A;%%B;%%D>> form_csv.csv

*/



void ShowSaveFileDialog(HWND hwndOwner)
{
	wchar_t szFileName[MAX_PATH];

	wcscpy_s(szFileName, MAX_PATH, szDefFileName);
	wcscat_s(szFileName, MAX_PATH, Extensions[0]);

	OPENFILENAMEW ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwndOwner;
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_ENABLEHOOK;
	ofn.lpfnHook = GetSaveFileNameHookProc;

	// Default settings are consistent with Extensions[0]
	ofn.lpstrFilter = L"Text files (*.txt)\0*.txt\0CSV files RFC 4180 (*.csv)\0*.csv\0CSV files Europe (*.csv)\0*.csv\0JSON files (*.json)\0*.json\0All files (*.*)\0*.*\0";
	ofn.lpstrDefExt = L"txt";
	ofn.nFilterIndex = 1;

	if (GetSaveFileNameW(&ofn))
	{
		clsInfoWindow *pInfoWindow = (clsInfoWindow*)GetWindowLongPtrW(hwndOwner, GWLP_USERDATA);
		if (!pInfoWindow) return;

		clsVTWindowInstance *mydata = (clsVTWindowInstance *)GetWindowLongPtrW(pInfoWindow->hwndParentWindow, GWLP_USERDATA); //hwndParentWindow === TListerWindow
		if (!mydata) return;

		DWORD filetype = ExportFormat::FTXT;
		if (ofn.nFilterIndex == 2) filetype = ExportFormat::FCSV4180;
		else if (ofn.nFilterIndex == 3) filetype = ExportFormat::FCSVEURO;
		else if (ofn.nFilterIndex == 4) filetype = ExportFormat::FJSON;

		mydata->pSharedPluginInstance->CreatFormatsTxt(ofn.lpstrFile, filetype);
	}
} // ShowSaveFileDialog