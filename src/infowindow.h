/*
The plugin is provided as-is and without any warranty under the GPLv3 license.
*/

#ifndef ULISTERFILEINFOWNDCLS
#define ULISTERFILEINFOWNDCLS

#include <windows.h>

class clsInfoWindow
{
public:
	HWND hEdit;
	HWND hButton;
	wchar_t* pText;
	int minWindowWidth;
	int minWindowHeight;

	clsInfoWindow();
	~clsInfoWindow();

	void Init(LPCWSTR textToDisplay, const int _minWindowWidth, const int _minWindowHeight);
	bool CreateWnd(const HINSTANCE hInstance, const HWND hwndParent);

	void Show();
	void Hide();

	void WMDestroyHandler();
	void WMSizeHandler(const int clientWidth, const int clientHeight);

	HICON hWindowIcon;
	HWND hwndFileInfo;
	HWND hwndParentWindow;

};

#endif
