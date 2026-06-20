/*
The plugin is provided as-is and without any warranty under the GPLv3 license.
*/

#ifndef ULISTERINSTANCE
#define ULISTERINSTANCE

#include <windows.h>
#include <sccvw.h>



#define SMARTINIPATH



class clsUlisterInstance
{
public:

	HINSTANCE	hInstWLX;
	unsigned long long WindowsBuildNumber;
	wchar_t inipath[MAX_PATH];

	void InitUlister(const HINSTANCE _hInst);
	~clsUlisterInstance();

	HINSTANCE ViewerLibraryInstanceInc();
	void ViewerLibraryInstanceDec(bool _keepinmemory);

	HINSTANCE FileIdentInstanceInc();
	void FileIdentInstanceDec(bool _keepinmemory);

#ifdef SMARTINIPATH
	static void CreateDefaultUlisterIni(wchar_t *_inipath);
#endif


private:

	HINSTANCE	hViewerLibrary;
	int			NumInstancesViewLib;

	HINSTANCE	hFileIdentLibrary;
	int			NumInstancesFileIdentLib;

	void GetIniPathWLX(wchar_t *_inipath);
	static void GetIniPathCOMMANDER(wchar_t *_inipath);
	static void GetIniPathAPPDATA(wchar_t *_inipath);
	bool GetIniPath();

	void GetRegCurrentBuildNumber();

	bool GetLibPathVT(wchar_t *libpath, const wchar_t *libname, const int ntlev);
	HINSTANCE LoadLibVT(const wchar_t *libname);
	void ErrMsgIssue(const int issuetype, const wchar_t *path, const DWORD dwError);


};


#endif

