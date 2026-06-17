/*
The plugin is provided as-is and without any warranty under the GPLv3 license.
*/

#ifndef ULISTERTOOLTIPWNDCLS
#define ULISTERTOOLTIPWNDCLS

#include <windows.h>

class clsToolTip
{
public:

	void InitPosition(HWND hWnd, int _X, int _Y, int _Width, int _Height);
	bool ShowTemporaryMessage(LPCWSTR InfoText, const WORD Transparency, const UINT Timer_ms);
	void DestroyTemporaryMessage();
	void Move();

	void Show();
	void Hide();

	UINT_PTR nIDEvent;
	clsToolTip(UINT_PTR _IDEvent);
	//~clsToolTip();

private:

	HWND	hParentWnd;
	HWND	hMsgWnd;
	int		Offset_X;
	int		Offset_Y;
	int		TargetWidth;
	int		TargetHeight;

	void PositionLimits(int *_X, int *_Y, int *_Width, int *_Height);

};

#endif
