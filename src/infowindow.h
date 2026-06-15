

#ifndef FILEINFOWNDCLS
#define FILEINFOWNDCLS

#include <windows.h>

class clsInfoWindow
{
public:
	HWND hEdit;
	HWND hButton;
	wchar_t* pText;
	int minWindowWidth; // 400
	int minWindowHeight; // 250

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
