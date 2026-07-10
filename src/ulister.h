/*
The plugin is provided as-is and without any warranty under the GPLv3 license.
*/

// ********** MUST BE INCLUDED FIRST! **********

#ifndef ULISTERPLUGIN
#define ULISTERPLUGIN

#include <windows.h>
#include <sccvw.h>




#define __ULISTDEBUGMSG
//#define __ULISTDEBUGKEEPINMEMORY
//#define __ULISTDEBUGTOOLTIPS
#define __ULISTDEBUGBUFFCFG



#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined (_M_AMD64) || defined(_AMD64_)
#define ULISTER64
#endif



class clsLoadedFileInfo
{
public:
	wchar_t* pPath;
	VTWORD wType;
	char* pTypeName;
	bool isQuickviewMode;

	clsLoadedFileInfo();
	~clsLoadedFileInfo();

	void Init(LPCWSTR _pPath, const VTWORD _wType, LPCSTR _pTypeName, const bool quickview);
};

#endif
