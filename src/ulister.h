/*
The plugin is provided as-is and without any warranty under the GPLv3 license.
*/

// ********** MUST BE INCLUDED FIRST! **********

#ifndef ULISTERPLUGIN
#define ULISTERPLUGIN

#include <windows.h>
#include <sccvw.h>

#include "infowindow.h"
#include "tooltip.h"



//#define __ULISTDEBUGMSG
//#define __ULISTDEBUGKEEPINMEMORY
//#define __ULISTDEBUGBALLOON



#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined (_M_AMD64) || defined(_AMD64_)
#define ULISTER64
#endif



class clsLoadedFileInfo
{
public:
	wchar_t* pPath;
	VTWORD wType;
	char* pTypeName;

	clsLoadedFileInfo();
	~clsLoadedFileInfo();

	void Init(LPCWSTR _pPath, const VTWORD _wType, LPCSTR _pTypeName);
};



struct ALLMYDATA
{
	HWND TListerWindow;
	WNDPROC OriginalTListerWindowProc;

	HWND waWindow; // WTF-naming
	
	WNDPROC OriginalSccviewerWindowProc;
	HWND SccviewerWindow;

	WNDPROC OriginalSccdisplayWindowProc;
	HWND SccdisplayWindow;

	clsToolTip ToolTip;

	clsLoadedFileInfo LoadedFileInfo;
	clsInfoWindow InfoWindow;

	ALLMYDATA();
};



class clsUlisterInstance
{
public:
	
	HINSTANCE	hInstWLX;
	unsigned long long WindowsBuildNumber;

	void Init(const HINSTANCE _hInst);
	~clsUlisterInstance();

	HINSTANCE ViewerLibraryInstanceInc();
	void ViewerLibraryInstanceDec(bool _keepinmemory);

	HINSTANCE FileIdentInstanceInc();
	void FileIdentInstanceDec(bool _keepinmemory);

private:
	
	HINSTANCE	hViewerLibrary;
	int			NumInstancesViewLib;

	HINSTANCE	hFileIdentLibrary;
	int			NumInstancesFileIdentLib;

};



namespace WindowsNTLevel {
	enum Type {
		WinNT5=0,
		WinNT6
	};
} // VS2005 fix

#endif
