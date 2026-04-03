/* 
	uLister is a powerful document viewer which can open more than 500 file formats.
	Copyright (C) 2011 Egor Vlaznev 
	
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <windows.h>
#include <stdio.h>
#include <map>
#include "ulister.h"
#include "total.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
HINSTANCE  hInst;
HANDLE     hViewerLibrary; // extern
int        numInstances;
extern int keepinmemory;
extern wchar_t ininoloadtypes[ULISTMAXBUF];
extern wchar_t inionlyloadtypes[ULISTMAXBUF];
extern wchar_t ininopreviewtypes[ULISTMAXBUF];
extern wchar_t inionlypreviewtypes[ULISTMAXBUF];
const char *ANOTFOUND = "Not found:";
const wchar_t *WNOTFOUND = L"Not found:";
const int MAXSEARCH = VTMAXSEARCHBUF - 1;

// нужно помнить строку поиска для каждого окна uLister, чтобы правильно выводить сообщение, что именно НЕ найдено,
// т.к. в каждом окне строка поиска FORWARD/BACK независимая.
std::map<HWND, char*> SearchStringPerWindowA;		// SearchStringW + SearchStringA + SearchParameter - into ALLMYDATA (TODO)
std::map<HWND, wchar_t*> SearchStringPerWindowW;



///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain(HINSTANCE hinst, unsigned long reason, void* lpReserved) {
	switch (reason) {
	case DLL_PROCESS_ATTACH:
		hInst = hinst;
		hViewerLibrary = NULL;
		numInstances = 0;
		iniparse();
		break;

	case DLL_PROCESS_DETACH:
		if (hViewerLibrary) FreeLibrary((HINSTANCE)hViewerLibrary);
		hViewerLibrary = NULL;

		std::map<HWND, wchar_t*>::iterator itW; // VS2005 fix
		for (itW = SearchStringPerWindowW.begin(); itW != SearchStringPerWindowW.end(); ++itW)
			free(itW->second);

		std::map<HWND, char*>::iterator itA; // VS2005 fix
		for (itA = SearchStringPerWindowA.begin(); itA != SearchStringPerWindowA.end(); ++itA)
			free(itA->second);

		break;
	}
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) HWND __stdcall ListLoadW(HWND ParentWin, wchar_t* FileToLoad, int ShowFlags) {
	HWND        hViewWnd;
	if (!CheckFile(FileToLoad, inionlyloadtypes, ininoloadtypes)) return NULL;
	hViewWnd = CreateLister(ParentWin);
	if (!IsWindow(hViewWnd)) return NULL;
	ALLMYDATA *mydata;
	mydata = (ALLMYDATA *)GetWindowLongPtr(hViewWnd, GWLP_USERDATA);
	if (mydata) {
		LoadFile(mydata->oiWindow, FileToLoad);
		numInstances++; // fix
	}
	return hViewWnd;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) HWND __stdcall ListLoad(HWND ParentWin, char* FileToLoad, int ShowFlags) {
	wchar_t path[MAX_PATH] = L"";
	MultiByteToWideChar(CP_ACP, 0, FileToLoad, -1, path, MAX_PATH);
	return ListLoadW(ParentWin, path, ShowFlags);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) int __stdcall ListLoadNextW(HWND ParentWin, HWND ListWin, wchar_t* FileToLoad, int ShowFlags) {
	if (!CheckFile(FileToLoad, inionlyloadtypes, ininoloadtypes)) return LISTPLUGIN_ERROR;
	ALLMYDATA *mydata;
	mydata = (ALLMYDATA *)GetWindowLongPtr(ListWin, GWLP_USERDATA);
	if (mydata) {
		LoadFile(mydata->oiWindow, FileToLoad);
		numInstances++; // fix
	}
	return LISTPLUGIN_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) int __stdcall ListLoadNext(HWND ParentWin, HWND ListWin, char* FileToLoad, int ShowFlags) {
	wchar_t path[MAX_PATH] = L"";
	MultiByteToWideChar(CP_ACP, 0, FileToLoad, -1, path, MAX_PATH);
	ListLoadNextW(ParentWin, ListWin, path, ShowFlags);
	return LISTPLUGIN_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport)void __stdcall ListCloseWindow(HWND ListWin) {
	if (IsWindow(ListWin)) {
		ALLMYDATA *mydata;
		mydata = (ALLMYDATA *)GetWindowLongPtr(ListWin, GWLP_USERDATA);
		if (mydata) {
			SendMessage(mydata->oiWindow, SCCVW_SAVEOPTIONS, 0, 0L);
			SendMessage(mydata->oiWindow, SCCVW_CLOSEFILE, 0, 0L);
			DestroyWindow(mydata->oiWindow);
			DestroyWindow(mydata->waWindow);
			numInstances--;
			if ((hViewerLibrary != NULL) && (keepinmemory == 0) && (numInstances == 0)) {
				FreeLibrary((HINSTANCE)hViewerLibrary);
				hViewerLibrary = NULL;
			}
			delete mydata;

			if (SearchStringPerWindowW.count(ListWin) != 0)
			{
				free(SearchStringPerWindowW[ListWin]);
				SearchStringPerWindowW.erase(ListWin);
			}
			if (SearchStringPerWindowA.count(ListWin) != 0)
			{
				free(SearchStringPerWindowA[ListWin]);
				SearchStringPerWindowA.erase(ListWin);
			}


		}
	}
}



///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport)int __stdcall ListSearchText(HWND ListWin, char* SearchString, int SearchParameter) { // ANSI
// reserved for Windows 98 SE future support maybe
#pragma warning( push )
#pragma warning( disable : 4996 )

	bool WindowWithoutSearchStringYet;
	char *WindSearchStr;
	ALLMYDATA *mydata;

	mydata = (ALLMYDATA *)GetWindowLongPtr(ListWin, GWLP_USERDATA);
	if (mydata) {

		WindowWithoutSearchStringYet = (SearchStringPerWindowA.count(ListWin) == 0);

		if ((SearchParameter & lcs_findfirst) || WindowWithoutSearchStringYet) {
			if (WindowWithoutSearchStringYet) // new search string
			{
				// VS2005 fix WindSearchStr = (char *)malloc(sizeof(SCCVWSEARCHINFO40::siText));
				WindSearchStr = (char *)malloc(member_size(SCCVWSEARCHINFO40, siText));
				strncpy(WindSearchStr, SearchString, MAXSEARCH);
				WindSearchStr[MAXSEARCH] = 0; // fix potencial overflow
				SearchStringPerWindowA[ListWin] = WindSearchStr;
			}
			else // update search string
			{
				WindSearchStr = SearchStringPerWindowA[ListWin];
				strncpy(WindSearchStr, SearchString, MAXSEARCH);
				WindSearchStr[MAXSEARCH] = 0; // fix potencial overflow
			}
		}
		else // get search string
		{
			WindSearchStr = SearchStringPerWindowA[ListWin];
		}

		if ((SearchParameter & lcs_findfirst) || WindowWithoutSearchStringYet) {

			SCCVWSEARCHINFO40 locSearchInfo40;

			locSearchInfo40.dwSize = sizeof(SCCVWSEARCHINFO40);
			// original used strncpy, supress error about unsafe strcopy and using strncpy_s instead 
			strncpy(locSearchInfo40.siText, WindSearchStr, MAXSEARCH);
			locSearchInfo40.siText[MAXSEARCH] = 0;
			locSearchInfo40.siTextLen = (VTWORD) strlen(locSearchInfo40.siText);

			locSearchInfo40.siType = (SearchParameter & lcs_matchcase) ? SCCVW_SEARCHCASE : SCCVW_SEARCHNOCASE;
			locSearchInfo40.siFrom = SCCVW_SEARCHCURRENT;
			locSearchInfo40.siDirection = (SearchParameter & lcs_backwards) ? SCCVW_SEARCHBACK : SCCVW_SEARCHFORWARD;

			//Updated for find to work
			//if(SendMessage(mydata->oiWindow,SCCVW_SEARCH,0,(LPARAM)(PSCCVWSEARCHINFO40)&locSearchInfo)!=0)
			if (SendMessage(mydata->oiWindow, SCCVW_SEARCH, 0, (LPARAM)(PSCCVWSEARCHINFO80)&locSearchInfo40) != 0) // hack!
				MessageBox(mydata->oiWindow, WindSearchStr, ANOTFOUND, MB_OK);
		}
		else
			if (SearchParameter & lcs_backwards) {
				if (SendMessage(mydata->oiWindow, SCCVW_SEARCHNEXT, SCCVW_SEARCHBACK, 0) != 0)
					MessageBox(mydata->oiWindow, WindSearchStr, ANOTFOUND, MB_OK);
			}
			else
				if (SendMessage(mydata->oiWindow, SCCVW_SEARCHNEXT, SCCVW_SEARCHFORWARD, 0) != 0)
					MessageBox(mydata->oiWindow, WindSearchStr, ANOTFOUND, MB_OK);
	}
	return LISTPLUGIN_OK;
#pragma warning( pop ) 
}



///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport)int __stdcall ListSearchTextW(HWND ListWin, WCHAR* SearchStringW, int SearchParameter) { // UTF16
#pragma warning( push )
#pragma warning( disable : 4996 )

	bool WindowWithoutSearchStringYet;
	wchar_t *WindSearchStr;
	ALLMYDATA *mydata;
	
	mydata = (ALLMYDATA *) GetWindowLongPtrW(ListWin, GWLP_USERDATA);
	if (mydata) {
		
		WindowWithoutSearchStringYet = (SearchStringPerWindowW.count(ListWin) == 0);

		if ((SearchParameter & lcs_findfirst) || WindowWithoutSearchStringYet) {
			if (WindowWithoutSearchStringYet) // new search string
			{
				// VS2005 fix WindSearchStr = (wchar_t *) malloc(sizeof(SCCVWSEARCHINFO80::siText));
				WindSearchStr = (wchar_t *) malloc(member_size(SCCVWSEARCHINFO80, siText));
				wcsncpy(WindSearchStr, SearchStringW, MAXSEARCH);
				WindSearchStr[MAXSEARCH] = 0; // fix potencial overflow
				SearchStringPerWindowW[ListWin] = WindSearchStr;
			}
			else // update search string
			{
				WindSearchStr = SearchStringPerWindowW[ListWin];
				wcsncpy(WindSearchStr, SearchStringW, MAXSEARCH);
				WindSearchStr[MAXSEARCH] = 0; // fix potencial overflow
			}
		}
		else // get search string
		{
			WindSearchStr = SearchStringPerWindowW[ListWin];
		}

		if ((SearchParameter & lcs_findfirst) || WindowWithoutSearchStringYet) {

			SCCVWSEARCHINFO80 locSearchInfo80;

			locSearchInfo80.dwSize = sizeof(SCCVWSEARCHINFO80);
			wcsncpy((wchar_t*)locSearchInfo80.siText, WindSearchStr, MAXSEARCH);
			locSearchInfo80.siText[MAXSEARCH] = 0;
			locSearchInfo80.siTextLen = (VTWORD) wcslen((wchar_t*)locSearchInfo80.siText);

			locSearchInfo80.siType = (SearchParameter & lcs_matchcase) ? SCCVW_SEARCHCASE : SCCVW_SEARCHNOCASE;
			locSearchInfo80.siFrom = SCCVW_SEARCHCURRENT;
			locSearchInfo80.siDirection = (SearchParameter & lcs_backwards) ? SCCVW_SEARCHBACK : SCCVW_SEARCHFORWARD;

			if (SendMessageW(mydata->oiWindow, SCCVW_SEARCH, 0, (LPARAM)(PSCCVWSEARCHINFO80)&locSearchInfo80) != 0)
				MessageBoxW(mydata->oiWindow, WindSearchStr, WNOTFOUND, MB_OK);
		}
		else
			if (SearchParameter & lcs_backwards) {
				if (SendMessageW(mydata->oiWindow, SCCVW_SEARCHNEXT, SCCVW_SEARCHBACK, 0) != 0)
					MessageBoxW(mydata->oiWindow, WindSearchStr, WNOTFOUND, MB_OK);
			}
			else
				if (SendMessageW(mydata->oiWindow, SCCVW_SEARCHNEXT, SCCVW_SEARCHFORWARD, 0) != 0)
					MessageBoxW(mydata->oiWindow, WindSearchStr, WNOTFOUND, MB_OK);
	}
	return LISTPLUGIN_OK;
#pragma warning( pop ) 
}



///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport)int __stdcall ListPrint(HWND ListWin, char* FileToPrint, char* DefPrinter, int PrintFlags, RECT* Margins) {
	ALLMYDATA *mydata;
	mydata = (ALLMYDATA *)GetWindowLongPtr(ListWin, GWLP_USERDATA);
	if (mydata)
		SendMessage(mydata->oiWindow, SCCVW_PRINT, 0, 0);
	return LISTPLUGIN_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport)int __stdcall ListSendCommand(HWND ListWin, int Command, int Parameter) {
	ALLMYDATA *mydata;
	mydata = (ALLMYDATA *)GetWindowLongPtr(ListWin, GWLP_USERDATA);
	if (mydata)
		switch (Command) {
		case lc_copy:
			SendMessage(mydata->oiWindow, SCCVW_COPYTOCLIP, 0, 0);
			return LISTPLUGIN_OK;
		case lc_selectall:
			SendMessage(mydata->oiWindow, SCCVW_SELECTALL, 0, 0);
			return LISTPLUGIN_OK;
		}
	return LISTPLUGIN_ERROR;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport)HBITMAP __stdcall ListGetPreviewBitmapW(wchar_t* FileToLoad, int width, int height, char* contentbuf, int contentbuflen) {
	if (!CheckFile(FileToLoad, inionlypreviewtypes, ininopreviewtypes))return NULL;
	HBITMAP bitmap = getpreview(FileToLoad, width, height);
	return bitmap;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport)HBITMAP __stdcall ListGetPreviewBitmap(char* FileToLoad, int width, int height, char* contentbuf, int contentbuflen) {
	wchar_t path[MAX_PATH] = L"";
	MultiByteToWideChar(CP_ACP, 0, FileToLoad, -1, path, MAX_PATH);
	return ListGetPreviewBitmapW(path, width, height, contentbuf, contentbuflen);
}
