/*
The plugin is provided as-is and without any warranty under the GPLv3 license.
*/

#ifndef ULISTERWINDOW
#define ULISTERWINDOW

// VS 2005 FIX
#define WM_MOUSEHWHEEL                  0x020E
#define WM_MOUSEWHEEL                   0x020A
#define GET_WHEEL_DELTA_WPARAM(wParam)  ((short)HIWORD(wParam))

#include <windows.h>



HWND CreateListerWindow(HWND ParentWin); // TODO + UlisterInstance.hInstWLX



#endif
