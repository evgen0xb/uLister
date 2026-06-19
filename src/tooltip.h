/*
The plugin is provided as-is and without any warranty under the GPLv3 license.
*/

#ifndef ULISTERTOOLTIPWNDCLS
#define ULISTERTOOLTIPWNDCLS

#include <windows.h>



#define TOOLTIPTIMER 3000
#define TOOLTIP_TIMER_MSG 42 // and ID_TIMER_MSG=1 is used in the SCCVIEWER class window btw
#define TOOLTIP_XOFFS 10
#define TOOLTIP_YOFFS 10
#define TOOLTIP_WIDTH 200
#define TOOLTIP_HEIGHT 30
#define TOOLTIP_TRANSPARENCY 244



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
