/* Copyright (c) 2001, 2016, Oracle and/or its affiliates. All rights reserved. */ 

#ifdef WINDOWS
#ifndef SCC_PACK_SET
#define SCC_PACK_SET 1
#define SCC_PACKED_BY_SCCLINK_C 1
#pragma pack(push,8)
#endif /* SCC_PACK_SET */
#endif /* WINDOWS */

#include <windows.h>
#include <tchar.h>
#include <direct.h>
#include <ctype.h>
#include <strsafe.h>


#if defined(__cplusplus)
extern "C"
{
#endif


#ifndef UNUSED
#define UNUSED(x) ((x) = (x))
#endif


static HINSTANCE SCCLoadViewerDLL(LPTSTR lpViewerPath)
{
    HINSTANCE   locRet = 0;
	LPTSTR     locDirPtr;
    TCHAR     *locPath = NULL;
    TCHAR     *locDir = NULL;
    size_t      wPathLen; 

	/* 11 comes from adding "\SCCVW.DLL" plus the null terminator. */
	if (FAILED(StringCchLength((LPCTSTR)lpViewerPath, STRSAFE_MAX_CCH-11, &wPathLen)))
	{
		//Input path larger than can be handled.
		return locRet;
	}

    locDir = (TCHAR *)malloc((wPathLen + 1) * sizeof(TCHAR));
    if(locDir)
    {
        /*
        |   Make copy of load path
        */
		StringCbCopy((LPTSTR)locDir, wPathLen * sizeof(TCHAR), (LPCTSTR)lpViewerPath);

        /*
        |   Strip off trailing "\"
        */
        locDirPtr = locDir;
        while (*locDirPtr != 0x00)  locDirPtr++;
        locDirPtr--;
        if (*locDirPtr == '\\') *locDirPtr = 0x00;

        /* 11 comes from adding "\SCCVW.DLL" plus the null terminator
        |  to locDir
        */
        locPath = (TCHAR *)malloc((wPathLen + 11) * sizeof(TCHAR));
        if(locPath)
        {
            /*
            |   Load the library
            */
			StringCbCopy((LPTSTR)locPath, wPathLen * sizeof(TCHAR), (LPCTSTR)locDir);
			StringCbCat((LPTSTR)locPath, (wPathLen + 11) * sizeof(TCHAR), _T("\\"));
			StringCbCat((LPTSTR)locPath, (wPathLen + 11) * sizeof(TCHAR), _T("SCCVW.DLL"));
            locRet = LoadLibraryEx(locPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
            free(locPath);
        }
        free(locDir);
    }
    return(locRet);
}


static VTVOID SCCGetExePath(HINSTANCE hInst, LPTSTR lpPath,VTWORD wPathLen)
{
    GetModuleFileName(hInst, lpPath, wPathLen);

    /*
    |   Strip the file name
    */

    while (*lpPath != 0x00)
        lpPath++;
    while (*lpPath != '\\' && *lpPath != ':')
        lpPath--;
    lpPath++;
    *lpPath = 0x00;
}

#if defined(__cplusplus)
}
#endif

#ifdef SCC_PACKED_BY_SCCLINK_C
#pragma pack(pop)
#undef SCC_PACKED_BY_SCCLINK_C
#undef SCC_PACK_SET
#endif /* SCC_PACKED_BY_SCCLINK_C */
