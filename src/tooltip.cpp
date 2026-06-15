/*
The plugin is provided as-is and without any warranty under the GPLv3 license.
*/

#include <windows.h>
#include "ulister.h"

clsToolTip::clsToolTip(UINT_PTR _IDTimerEvent)
{
	nIDEvent = _IDTimerEvent;
	hMsgWnd = NULL;
	hParentWnd = NULL;
	Offset_X = 0;
	Offset_Y = 0;
	TargetWidth = 0;
	TargetHeight = 0;
}

void clsToolTip::InitPosition(HWND hWndParent, int _X, int _Y, int _Width, int _Height)
{
	hParentWnd = hWndParent;
	Offset_X = _X;
	Offset_Y = _Y;
	TargetWidth = _Width;
	TargetHeight = _Height;
};

bool clsToolTip::ShowTemporaryMessage(LPCWSTR InfoText, const BYTE Transparency, const UINT Timer_ms)
// BE CAREFUL! InfoText IS ONLY PTR, NOT A BUFFER! You can add malloc() with memcpy() in ShowTemporaryMessage and free() in ~destructor.
// return true - OK
{
	if (!hParentWnd) return false;
	if (hMsgWnd) DestroyWindow(hMsgWnd); // Remove the old window if it is still hanging

										 // SS_LEFTNOWORDWRAP
	hMsgWnd = CreateWindowExW(
		WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_LAYERED,
		L"STATIC", InfoText,
		WS_POPUP | SS_CENTER | WS_BORDER,
		Offset_X, Offset_Y,
		TargetWidth, TargetHeight,
		hParentWnd, // Parent
		(HMENU)NULL,
		GetModuleHandle(NULL), // Instance for global window class
		NULL); // lParam of WM_CREATE

	if (hMsgWnd)
	{
		Move(); // apply position limits
		SetLayeredWindowAttributes(hMsgWnd, 0, Transparency, LWA_ALPHA);
		Show();
		SetTimer(hParentWnd, nIDEvent, Timer_ms, NULL);
		return true;
	}
	else
	{
		DestroyTemporaryMessage();
		return false;
	}
}

void clsToolTip::DestroyTemporaryMessage()
{
	if (IsWindow(hParentWnd)) KillTimer(hParentWnd, nIDEvent);
	if (hMsgWnd) DestroyWindow(hMsgWnd);
	hMsgWnd = NULL; // don't touch the hParentWnd!
}

void clsToolTip::Move()
// move and resize with parent window client
{
	if (hMsgWnd && hParentWnd)
	{
		int toX, toY, limitedWidth, limitedHeight;
		PositionLimits(&toX, &toY, &limitedWidth, &limitedHeight);

		SetWindowPos(hMsgWnd, NULL,
			toX + Offset_X,
			toY + Offset_Y,
			limitedWidth, limitedHeight,
			SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void clsToolTip::PositionLimits(int *_X, int *_Y, int *_Width, int *_Height)
// to get absolute window coordinate without "Windows 10 Invisible Resizing Borders" and "Drop Shadows" you must call DwmGetWindowAttribute with DWMWA_EXTENDED_FRAME_BOUNDS instead GetWindowRect.
// however this function may not be implemented in older Windows versions.
// we will use old school GetClientRect and ClientToScreen instead
{
#if defined (__ULISTDEBUGMSG) && defined(__ULISTDEBUGBALLOON)
	OutputDebugStringW(L"*** PositionLimits ***");
#endif

	RECT rectParentClient; GetClientRect(hParentWnd, &rectParentClient);
	POINT pointParentClientTopLeft = { 0, 0 }; ClientToScreen(hParentWnd, &pointParentClientTopLeft);

	// absolute coordinate:
	// rectParentClient.top = pointParentClientTopLeft.y; <-never used
	// rectParentClient.left = pointParentClientTopLeft.x; <-never used
	// *_X = rectParentClient.left;
	// *_Y = rectParentClient.top;

	*_X = pointParentClientTopLeft.x;
	*_Y = pointParentClientTopLeft.y;
	rectParentClient.right = rectParentClient.right + pointParentClientTopLeft.x;
	rectParentClient.bottom = rectParentClient.bottom + pointParentClientTopLeft.y;

#if defined (__ULISTDEBUGMSG) && defined(__ULISTDEBUGBALLOON)
	std::wstring msgW = L"parent client absolute (" + ToStrW(pointParentClientTopLeft.x) + L", " + ToStrW(pointParentClientTopLeft.y) + L", " + ToStrW(rectParentClient.right) + L", " + ToStrW(rectParentClient.bottom) + L")";
	OutputDebugStringW(msgW.c_str());
#endif

	RECT rectMsgWindow; GetWindowRect(hMsgWnd, &rectMsgWindow);
	if (rectMsgWindow.left == Offset_X && rectMsgWindow.top == Offset_Y && !IsWindowVisible(hMsgWnd))
	{
		// it's impossible!
		// GetWindowRect fails due just created window is HIDDEN.

#if defined (__ULISTDEBUGMSG) && defined(__ULISTDEBUGBALLOON)
		OutputDebugStringW(L"GetWindowRect fails. Correcting coordinates.");
#endif

		rectMsgWindow.left += pointParentClientTopLeft.x;
		rectMsgWindow.top += pointParentClientTopLeft.y;
		rectMsgWindow.right += pointParentClientTopLeft.x;
		rectMsgWindow.bottom += pointParentClientTopLeft.y;
	}

#if defined (__ULISTDEBUGMSG) && defined(__ULISTDEBUGBALLOON)
	msgW = L"msg absolute (" + ToStrW(rectMsgWindow.left) + L", " + ToStrW(rectMsgWindow.top) + L", " + ToStrW(rectMsgWindow.right) + L", " + ToStrW(rectMsgWindow.bottom) + L")";
	OutputDebugStringW(msgW.c_str());
#endif

	union { int scrollbarHeight; int scrollbarWidth; };

	scrollbarHeight = GetSystemMetrics(SM_CYHSCROLL);
	*_Height = rectParentClient.bottom - rectMsgWindow.top - scrollbarHeight; // max available
	scrollbarWidth = GetSystemMetrics(SM_CXVSCROLL);
	*_Width = rectParentClient.right - rectMsgWindow.left - scrollbarWidth;

#if defined (__ULISTDEBUGMSG) && defined(__ULISTDEBUGBALLOON)
	msgW = L"max: newwidth=" + ToStrW(*_Width);
	OutputDebugStringW(msgW.c_str());
	msgW = L"max: newheight=" + ToStrW(*_Height);
	OutputDebugStringW(msgW.c_str());
#endif

	*_Height = (*_Height < 0) ? 0 : *_Height; // 0 if negative
	*_Width = (*_Width < 0) ? 0 : *_Width;

#if defined (__ULISTDEBUGMSG) && defined(__ULISTDEBUGBALLOON)
	msgW = L"positive: newwidth=" + ToStrW(*_Width);
	OutputDebugStringW(msgW.c_str());
	msgW = L"positive: newheight=" + ToStrW(*_Height);
	OutputDebugStringW(msgW.c_str());
#endif

	*_Height = (*_Height < TargetHeight) ? *_Height : TargetHeight; // min
	*_Width = (*_Width < TargetWidth) ? *_Width : TargetWidth;

#if defined (__ULISTDEBUGMSG) && defined(__ULISTDEBUGBALLOON)
	msgW = L"min: newwidth=" + ToStrW(*_Width);
	OutputDebugStringW(msgW.c_str());
	msgW = L"min: newheight=" + ToStrW(*_Height);
	OutputDebugStringW(msgW.c_str());
#endif
}

void clsToolTip::Show() { if (hMsgWnd) { ShowWindow(hMsgWnd, SW_SHOWNOACTIVATE); UpdateWindow(hMsgWnd); } }
void clsToolTip::Hide() { if (hMsgWnd) ShowWindow(hMsgWnd, SW_HIDE); }

