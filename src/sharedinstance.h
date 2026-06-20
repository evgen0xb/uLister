/*
The plugin is provided as-is and without any warranty under the GPLv3 license.
*/

#ifndef SHAREDINSTANCE
#define SHAREDINSTANCE

#include <windows.h>
#include <sccvw.h>

#include "ulisterinstance.h"
#include "config.h"



class clsSharedPluginInstance
{
public:
	// Common for all Viewer Technology windows:
	clsUlisterInstance UlisterInstance;
	clsUlisterOptions UlisterOptions;
	clsVTOptions VTOptions;

	void InitPlugin(const HINSTANCE _hInst);
	void ReloadPluginOptions();

	VTWORD GetVTFileType(const wchar_t* FileToLoad, __VTTYPENAMEBUF &pOutTypeName);
	bool IsVTFileTypeAllowed(const VTWORD wType, bool isThumbnailMode);
	//bool IsVTFileTypePreviewAllowed(const VTWORD wType);
	HBITMAP GetVTFilePreview(const wchar_t* FileToLoad, const int width, const int height);
	static bool LoadVTFile(const HWND hViewWnd, const wchar_t* FileToLoad);

	void CreatFormatsTxt(const wchar_t* path);


};



#endif
