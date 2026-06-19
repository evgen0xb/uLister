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



#include "ulister.h"
#include "infowindow.h"
#include "utils.h"



#define ID_BUTTON_EXPORT 101
#define ID_EDIT_TEXT     102

const wchar_t FILEINFOWINDOWCLASS[] = L"FileInfoWindowClass";



void ShowSaveFileDialog(HWND hwndOwner);
LRESULT CALLBACK FileInfoWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK ButtonSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);



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



clsInfoWindow::~clsInfoWindow()
{
	if (pText) free(pText);
	if (hWindowIcon) DestroyIcon(hWindowIcon);
}



void clsInfoWindow::Init(LPCWSTR textToDisplay, const int _minWindowWidth, const int _minWindowHeight)
{
	pText = _wcsdup(textToDisplay);
	minWindowWidth = _minWindowWidth;
	minWindowHeight = _minWindowHeight;
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

	WNDCLASSW wc; ZeroMemory(&wc, sizeof(wc)); //WNDCLASSW wc = {};

	wc.lpfnWndProc = FileInfoWindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = FILEINFOWINDOWCLASS;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	if (!hWindowIcon) hWindowIcon = ExtractIconA(hInstance, "shell32.dll", 221);
	wc.hIcon = hWindowIcon;

	RegisterClassW(&wc);

	hwndFileInfo = CreateWindowExW(
		0, FILEINFOWINDOWCLASS,
		Title,
		WS_OVERLAPPEDWINDOW ^ WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, minWindowWidth, minWindowHeight,
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



void ShowSaveFileDialog(HWND hwndOwner)
{
#ifdef ULISTER64
	wchar_t szFileName[MAX_PATH] = L"formats64.txt";
#else
	wchar_t szFileName[MAX_PATH] = L"formats32.txt";
#endif

	OPENFILENAMEW ofn;

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwndOwner;
	ofn.lpstrFilter = L"Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = L"txt";

	if (GetSaveFileNameW(&ofn)) CreatFormatsTxt(ofn.lpstrFile);
}
