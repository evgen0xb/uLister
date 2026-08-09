/*
The plugin is provided as-is and without any warranty under the GPLv3 license.
*/

#include <windows.h>
#include <stdio.h>
#include <sccvw.h>

#include "ulister.h"
#include "sharedinstance.h"
#include "utils.h"



const char *AFIINIT = "FIInit";
const char *AFIDEINIT = "FIDeInit";
const char *AFIGETFIRSTID = "FIGetFirstId";
const char *AFIGETNEXTID = "FIGetNextId";
const char *AFIIDFILEEX = "FIIdFileEx";

const char *AFAMAPIDTOFILTERINFO = "FAMapIdToFilterInfo";


extern const char *WNDCLASSNAME_SCCVIEWER;



void clsSharedPluginInstance::ReloadPluginOptions()
{
	UlisterOptions.LoadUlisterOptions(UlisterInstance.inipath, UlisterInstance.WindowsBuildNumber);
	VTOptions.LoadVTOptions(UlisterInstance.inipath);
}



void clsSharedPluginInstance::InitPlugin(const HINSTANCE _hInst)
{
	UlisterInstance.InitUlister(_hInst);
	ReloadPluginOptions();
}



VTWORD clsSharedPluginInstance::GetVTFileType(const wchar_t* FileToLoad, __VTTYPENAMEBUF &pOutTypeName)
// return:
// Type Number (VTWORD)
// pOutTypeName: Type Name
{
	typedef VTDWORD(*FIInitFUNC)(VTVOID);
	typedef VTWORD(*FIIdFileExFUNC)(VTDWORD, const VTVOID *, VTDWORD, VTWORD *, VTLPTSTR, VTWORD);
	typedef VTDWORD(*FIDeInitFUNC)(VTVOID);

	FIInitFUNC FIInit;
	FIIdFileExFUNC FIIdFileEx;
	FIDeInitFUNC FIDeInit;

	if (pOutTypeName) pOutTypeName[0] = '\0';
	VTWORD  wVTFileType = FI_UNKNOWN;
	SCCERR  FIErrorCode = SCCERR_UNKNOWN;

	HINSTANCE hInstFileIdentDLL = UlisterInstance.FileIdentInstanceInc();

	if (hInstFileIdentDLL)
	{
		FIInit = (FIInitFUNC)GetProcAddress(hInstFileIdentDLL, AFIINIT);
		FIIdFileEx = (FIIdFileExFUNC)GetProcAddress(hInstFileIdentDLL, AFIIDFILEEX);
		FIDeInit = (FIDeInitFUNC)GetProcAddress(hInstFileIdentDLL, AFIDEINIT);

		if (FIInit && FIIdFileEx && FIDeInit)
		{
			FIInit();
			VTDWORD dwFlags = FIFLAG_NORMAL;
			FIErrorCode = FIIdFileEx(IOTYPE_UNICODEPATH, FileToLoad, dwFlags, &wVTFileType, pOutTypeName, sizeof(pOutTypeName));
			if (FIErrorCode != SCCERR_OK && pOutTypeName) pOutTypeName[0] = '\0';
			FIDeInit();
		}
	}

	UlisterInstance.FileIdentInstanceDec(UlisterOptions.keepinmemory);

	return 	(FIErrorCode == SCCERR_OK) ? wVTFileType : FI_UNKNOWN;
}



bool clsSharedPluginInstance::IsVTFileTypeAllowed(const VTWORD wType, bool isThumbnailMode)
{
	// TRUE = OK
	// FALSE = refuse

	wchar_t FTypeStr[INT64STRMAXBUF];
	_itow_s(wType, FTypeStr, INT64STRMAXBUF, 10);

	if (isThumbnailMode)
	{
		if (wcslen(UlisterOptions.inionlypreviewtypes) > 0)
			if (wcsstr(UlisterOptions.inionlypreviewtypes, FTypeStr)) return true; else return false;

		if (wcslen(UlisterOptions.ininopreviewtypes) > 0 && wcsstr(UlisterOptions.ininopreviewtypes, FTypeStr)) return false; else return true;
	}
	else
	{
		if (wcslen(UlisterOptions.inionlyloadtypes) > 0)
			if (wcsstr(UlisterOptions.inionlyloadtypes, FTypeStr)) return true; else return false;

		if (wcslen(UlisterOptions.ininoloadtypes) > 0 && wcsstr(UlisterOptions.ininoloadtypes, FTypeStr)) return false; else return true;
	}
}



bool clsSharedPluginInstance::LoadVTFile(const HWND hViewWnd, const wchar_t* FileToLoad)
{
	// TRUE - file load OK
	// FALSE - the file could not be loaded
	SCCVWVIEWFILE80  locViewFile;
	locViewFile.dwSize = sizeof(SCCVWVIEWFILE80);
	locViewFile.dwSpecType = IOTYPE_UNICODEPATH;
	locViewFile.pSpec = (VTVOID *)FileToLoad;
	locViewFile.dwViewAs = 0;
	locViewFile.bUseDisplayName = FALSE;
	locViewFile.bDeleteOnClose = FALSE;
	locViewFile.dwFlags = 0;
	locViewFile.dwReserved1 = 0;
	locViewFile.dwReserved2 = 0;
	return (SendMessage(hViewWnd, SCCVW_VIEWFILE, 0, (LPARAM)&locViewFile) == SCCVWERR_OK);
}



HBITMAP clsSharedPluginInstance::GetVTFilePreview(const wchar_t* FileToLoad, const int width, const int height)
{
	if (!UlisterInstance.ViewerLibraryInstanceInc()) return NULL;

	HWND hViewWnd = CreateWindow(WNDCLASSNAME_SCCVIEWER, NULL, WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, 0, UlisterInstance.hInstWLX, NULL);
	if (!IsWindow(hViewWnd))
	{
		UlisterInstance.ViewerLibraryInstanceDec(UlisterOptions.keepinmemory);
		return NULL;
	}

	// TC SDK:
	// Return a handle to your window if load succeeds, NULL otherwise. If NULL is returned, Lister will try the next plugin.

	if (!LoadVTFile(hViewWnd, FileToLoad))
	{
		DestroyWindow(hViewWnd);
		UlisterInstance.ViewerLibraryInstanceDec(UlisterOptions.keepinmemory);
		return NULL;
	}

	HDC OutputDC, FormatDC;
	HBITMAP bitmap, oldbitmap;
	SCCVWDRAWPAGE41  locDrawPage;
	BITMAPINFOHEADER  locBIH;

	FormatDC = GetDC(hViewWnd);
	OutputDC = CreateCompatibleDC(FormatDC);

	locBIH.biSize = sizeof(BITMAPINFOHEADER);
	locBIH.biWidth = width;
	locBIH.biHeight = height;
	locBIH.biPlanes = 1;
	locBIH.biBitCount = 24;
	locBIH.biCompression = BI_RGB;
	locBIH.biSizeImage = 0;
	locBIH.biXPelsPerMeter = 0;
	locBIH.biYPelsPerMeter = 0;
	locBIH.biClrUsed = 0;
	locBIH.biClrImportant = 0;

	bitmap = CreateDIBitmap(FormatDC, &locBIH, 0, NULL, NULL, DIB_RGB_COLORS);

	oldbitmap = (HBITMAP)SelectObject(OutputDC, bitmap);
	Rectangle(OutputDC, 0, 0, width, height);

	locDrawPage.dwSize = sizeof(SCCVWDRAWPAGE41);
	locDrawPage.dwPageToDraw = 0;
	locDrawPage.dwReserved = 0;
	locDrawPage.dwFlags = SCCVW_DPFLAG_DETERMINEOUTPUTTYPE;
	locDrawPage.lUnitsPerInch = 1440;
	locDrawPage.lFormatWidth = 12240;
	locDrawPage.lFormatHeight = 15840;
	locDrawPage.lTop = 0;
	locDrawPage.lLeft = 0;
	locDrawPage.lBottom = height;
	locDrawPage.lRight = width;
	locDrawPage.hOutputDC = OutputDC;
	locDrawPage.hFormatDC = FormatDC;

	SendMessage(hViewWnd, SCCVW_INITDRAWPAGE, 0, 0);
	SendMessage(hViewWnd, SCCVW_DRAWPAGE, 0, (LPARAM)(PSCCVWDRAWPAGE41)&locDrawPage);
	SelectObject(OutputDC, oldbitmap);
	DeleteDC(OutputDC);
	ReleaseDC(hViewWnd, FormatDC);

	SendMessage(hViewWnd, SCCVW_DEINITDRAWPAGE, 0, 0);
	SendMessage(hViewWnd, SCCVW_CLOSEFILE, 0, 0L);
	DestroyWindow(hViewWnd);

	UlisterInstance.ViewerLibraryInstanceDec(UlisterOptions.keepinmemory);

	return bitmap;
}



void clsSharedPluginInstance::CreatFormatsTxt(const wchar_t* path)
{
	typedef VTDWORD(*FIInitFUNC)(VTVOID);
	typedef VTDWORD(*FIDeInitFUNC)(VTVOID);
	typedef VTWORD(*FIGetFirstIdFUNC)(PFIGET pFiGet, VTWORD * pType, VTLPTSTR pTypeName, VTWORD wNameCount);
	typedef VTWORD(*FIGetNextIdFUNC)(PFIGET pFiGet, VTWORD * pType, VTLPTSTR pTypeName, VTWORD wNameCount);

	// ************************************************************************************* //
	// Undocumented:                                                                         //
	// To use FAMapIdToFilterInfo you don't need to explicitly call FAInit() and FADeInit()  //
	//                                                                                       //

#define SCCFILTERINFO8SIZE 272
#define SCCFILTERINFODLLNAMELEN 60
#define SCCFILTERINFOMAXDLLTYPES 102

	typedef union _SCCFILTERINFO
	{
		struct
		{
			__int16 reserved0;
			__int16 reserved1;
			__int16 DLLTypesCount;
			__int16 reserved2;
			__int16 DLLTypes[SCCFILTERINFOMAXDLLTYPES];
			char DLLName[SCCFILTERINFODLLNAMELEN];
		};
		char BUF[SCCFILTERINFO8SIZE];
	} SCCFILTERINFO, *PSCCFILTERINFO;

	typedef VTDWORD(*FAMapIdToFilterInfoFUNC)(VTWORD Type, PSCCFILTERINFO PSCCFAINFO);

	// return:                                                                               //
	// 4 == FAIL (Type not found); 0 == OK                                                   //
	// Reverse engineering sccfa.dll                                                         //
	// ************************************************************************************* //

	FIInitFUNC FIInit;
	FIDeInitFUNC FIDeInit;
	FIGetFirstIdFUNC FIGetFirstId;
	FIGetNextIdFUNC FIGetNextId;

	FAMapIdToFilterInfoFUNC FAMapIdToFilterInfo;

	HINSTANCE hInstFileIdentDLL = UlisterInstance.FileIdentInstanceInc();
	HINSTANCE hInstFilterAccessDLL = UlisterInstance.FilterAccessInstanceInc();

	SCCFILTERINFO filterinfo;

	if (hInstFileIdentDLL && hInstFilterAccessDLL)
	{
		FIInit = (FIInitFUNC)GetProcAddress(hInstFileIdentDLL, AFIINIT);
		FIDeInit = (FIDeInitFUNC)GetProcAddress(hInstFileIdentDLL, AFIDEINIT);
		FIGetFirstId = (FIGetFirstIdFUNC)GetProcAddress(hInstFileIdentDLL, AFIGETFIRSTID);
		FIGetNextId = (FIGetFirstIdFUNC)GetProcAddress(hInstFileIdentDLL, AFIGETNEXTID);

		FAMapIdToFilterInfo = (FAMapIdToFilterInfoFUNC)GetProcAddress(hInstFilterAccessDLL, AFAMAPIDTOFILTERINFO);

		if (FIInit && FIDeInit && FIGetFirstId && FIGetNextId && FAMapIdToFilterInfo)
		{
			HANDLE hFile = CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				FIGET figetTag;
				VTBOOL MoreIDs;
				__VTTYPENAMEBUF buf;
				DWORD bytesWritten;

				VTWORD TotalIDs = 0;
				VTWORD TotalSupported = 0;
				VTWORD TypeNumber;
				char TypeName[VTMAXTYPENAMEBUF];

				FIInit();
				MoreIDs = FIGetFirstId(&figetTag, &TypeNumber, TypeName, VTMAXTYPENAMEBUF);
				while (MoreIDs)
				{
					TotalIDs++;

					bool err = FAMapIdToFilterInfo(TypeNumber, &filterinfo);
					if (!err) TotalSupported++;

					//_snprintf_s(buf, ULISTMAXBUF, _TRUNCATE, "%u  -  %s\r\n", TypeNumber, TypeName);
					_snprintf_s(buf, ULISTMAXBUF, _TRUNCATE, "%u   %-12s  -  %s\r\n", TypeNumber, (err ? "*" : filterinfo.DLLName), TypeName);

					//OutputDebugStringA(buf);
					WriteFile(hFile, buf, (DWORD)strlen(buf), &bytesWritten, NULL);
					MoreIDs = FIGetNextId(&figetTag, &TypeNumber, TypeName, VTMAXTYPENAMEBUF);
				}
				FIDeInit();
				_snprintf_s(buf, ULISTMAXBUF, _TRUNCATE, "\r\nTotal known format IDs: %u\r\n", TotalIDs);
				//OutputDebugStringA(buf);
				WriteFile(hFile, buf, (DWORD)strlen(buf), &bytesWritten, NULL);

				_snprintf_s(buf, ULISTMAXBUF, _TRUNCATE, "Total supported format IDs: %u\r\n", TotalSupported);
				//OutputDebugStringA(buf);
				WriteFile(hFile, buf, (DWORD)strlen(buf), &bytesWritten, NULL);

				CloseHandle(hFile);
			}
		}
	}

	UlisterInstance.FileIdentInstanceDec(UlisterOptions.keepinmemory);
	UlisterInstance.FilterAccessInstanceDec(UlisterOptions.keepinmemory);
}

