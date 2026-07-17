/*
The plugin is provided as-is and without any warranty under the GPLv3 license.
*/

#include <windows.h>
#include <sccvw.h>

#include "ulister.h"
#include "config.h"
#include "utils.h"



const wchar_t *CLIPBOARDSECTION = L"clipboard";
const wchar_t *VIEWERSECTION = L"viewer";
const wchar_t *MEMORYSECTION = L"memory";
const wchar_t *ASKIP = L"SKIP";
const wchar_t *AON = L"ON";
const wchar_t *AOFF = L"OFF";



///////////////////////////////////////////////////////////////////////////////////////////////////



void clsUlisterOptions::LoadUlisterOptions(wchar_t *inipath, const unsigned long long WindowsBuildNumber)
{
	const wchar_t *ULISTERSECTION = L"uLister";
	const wchar_t *OIT_DATA_PATH = L"OIT_DATA_PATH";

	wchar_t inioptdir[MAX_PATH];
	wchar_t oitdatapath[MAX_PATH];

	wchar_t buf[INT64STRMAXBUF];

	GetPrivateProfileStringW(ULISTERSECTION, L"keepinmemory", L"1", buf, INT64STRMAXBUF, inipath);
	if (_wcsicmp(buf, L"1") == 0) keepinmemory = true; else keepinmemory = false;

	GetPrivateProfileStringW(ULISTERSECTION, L"mwhscrollinvert", AON, buf, INT64STRMAXBUF, inipath);
	if (_wcsicmp(buf, AON) == 0) mwhscrollinvert = true; else mwhscrollinvert = false;

	GetPrivateProfileStringW(ULISTERSECTION, L"optionsdir", L"", inioptdir, MAX_PATH, inipath);

	GetPrivateProfileStringW(ULISTERSECTION, L"noloadtypes", L"", ininoloadtypes, ULISTMAXBUF, inipath);
	GetPrivateProfileStringW(ULISTERSECTION, L"onlyloadtypes", L"", inionlyloadtypes, ULISTMAXBUF, inipath);
	GetPrivateProfileStringW(ULISTERSECTION, L"nopreviewtypes", L"", ininopreviewtypes, ULISTMAXBUF, inipath);
	GetPrivateProfileStringW(ULISTERSECTION, L"onlypreviewtypes", L"", inionlypreviewtypes, ULISTMAXBUF, inipath);

	if (wcslen(inioptdir) > 0) {
		ExpandEnvironmentStringsW(inioptdir, oitdatapath, MAX_PATH);
		SetEnvironmentVariableW(OIT_DATA_PATH, oitdatapath);
	}

	GetPrivateProfileStringW(ULISTERSECTION, L"tooltipsdelayms", L"3000", buf, INT64STRMAXBUF, inipath);
	ToolTipTimer = (UINT)wcstol(buf, NULL, 10);

	GetPrivateProfileStringW(ULISTERSECTION, L"tooltipstransparency", L"244", buf, INT64STRMAXBUF, inipath);
	ToolTipTransparency = (WORD)wcstol(buf, NULL, 10);
	if (ToolTipTransparency > 255 || WindowsBuildNumber < WINDOWS8BUILDNUMBER) ToolTipTransparency = -1;

	GetPrivateProfileStringW(ULISTERSECTION, L"smartscrollbar", AON, buf, INT64STRMAXBUF, inipath);
	if (_wcsicmp(buf, AON) == 0) smartscrollbar = true; else smartscrollbar = false;
}



///////////////////////////////////////////////////////////////////////////////////////////////////



VTDWORD clsVTOptionsViewer::ReadIniViewOptDisplay(const wchar_t *optionname, const wchar_t *inipath)
{
	// A.7.2 SCCID_WPDISPLAYMODE / SCCID_HTMLDISPLAYMODE / SCCID_EMAILDISPLAYMODE
	wchar_t buf[INT64STRMAXBUF];
	VTDWORD result;

	GetPrivateProfileStringW(VIEWERSECTION, optionname, ASKIP, buf, INT64STRMAXBUF, inipath);
	if (_wcsicmp(buf, L"draft") == 0) result = SCCVW_WPMODE_DRAFT;
	else if (_wcsicmp(buf, L"normal") == 0) result = SCCVW_WPMODE_NORMAL;
	else if (_wcsicmp(buf, L"preview") == 0) result = SCCVW_WPMODE_PREVIEW;
	else if (_wcsicmp(buf, L"weblayout") == 0) result = SCCVW_WPMODE_WEBLAYOUT;
	else result = Opt::SKIP;

	return result;
}



///////////////////////////////////////////////////////////////////////////////////////////////////



VTDWORD clsVTOptionsViewer::ReadIniViewOptWebPrevFitMode(const wchar_t *optionname, const wchar_t *inipath)
{
	// A.7.3 SCCID_WPFITMODE / SCCID_HTMLFITMODE / SCCID_EMAILFITMODE
	wchar_t buf[INT64STRMAXBUF];
	VTDWORD result;

	GetPrivateProfileStringW(VIEWERSECTION, optionname, ASKIP, buf, INT64STRMAXBUF, inipath);
	if (_wcsicmp(buf, L"original") == 0) result = SCCVW_FITMODE_ORIGINAL;
	else if (_wcsicmp(buf, L"width") == 0) result = SCCVW_FITMODE_WINDOWWIDTH;
	else if (_wcsicmp(buf, L"window") == 0) result = SCCVW_FITMODE_WINDOW;
	else result = Opt::SKIP;

	return result;
}



///////////////////////////////////////////////////////////////////////////////////////////////////



VTDWORD clsVTOptionsViewer::ReadIniViewOptGraphicFitMode(const wchar_t *optionname, const wchar_t *inipath)
{
	// A.5.11 SCCID_VECFITMODE, A.5.4 SCCID_BMPFITMODE
	wchar_t buf[INT64STRMAXBUF];
	VTDWORD result;

	GetPrivateProfileStringW(VIEWERSECTION, optionname, ASKIP, buf, INT64STRMAXBUF, inipath);
	if (_wcsicmp(buf, L"best") == 0) result = SCCVW_FITMODE_BEST;
	else if (_wcsicmp(buf, L"original") == 0) result = SCCVW_FITMODE_ORIGINAL;
	else if (_wcsicmp(buf, L"window") == 0) result = SCCVW_FITMODE_WINDOW;
	else if (_wcsicmp(buf, L"height") == 0) result = SCCVW_FITMODE_WINDOWHEIGHT;
	else if (_wcsicmp(buf, L"width") == 0) result = SCCVW_FITMODE_WINDOWWIDTH;
	else if (_wcsicmp(buf, L"stretch") == 0) result = SCCVW_FITMODE_STRETCHWINDOW;
	else if (_wcsicmp(buf, L"imagesize") == 0) result = SCCVW_FITMODE_IMAGESIZE;
	else result = Opt::SKIP;

	return result;
}



///////////////////////////////////////////////////////////////////////////////////////////////////



__int8 clsVTOptionsViewer::ReadIniViewOptSpreadsheetDisplayMode(const wchar_t *optionname, const wchar_t *inipath)
{
	wchar_t buf[INT64STRMAXBUF];
	__int8 result;

	GetPrivateProfileStringW(VIEWERSECTION, optionname, ASKIP, buf, INT64STRMAXBUF, inipath);
	if (_wcsicmp(buf, L"draft") == 0) result = UlisterSSDisplayMode::DRAFT;
	else if (_wcsicmp(buf, L"normal") == 0) result = UlisterSSDisplayMode::NORMAL;
	else if (_wcsicmp(buf, L"normalhidden") == 0) result = UlisterSSDisplayMode::NORMALHIDDEN;
	else result = Opt::SKIP;

	return result;
}



///////////////////////////////////////////////////////////////////////////////////////////////////



void clsVTOptionsViewer::LoadViewerOptions(const wchar_t *inipath)
{
	WPDISPLAYMODE.Option = ReadIniViewOptDisplay(L"wpdisplaymode", inipath);
	HTMLDISPLAYMODE.Option = ReadIniViewOptDisplay(L"htmldisplaymode", inipath);
	EMAILDISPLAYMODE.Option = ReadIniViewOptDisplay(L"emaildisplaymode", inipath);

	WEBPREVWPFITMODE.Option = ReadIniViewOptWebPrevFitMode(L"webprevwpfitmode", inipath);
	WEBPREVHTMLFITMODE.Option = ReadIniViewOptWebPrevFitMode(L"webprevhtmlfitmode", inipath);
	WEBPREVEMAILFITMODE.Option = ReadIniViewOptWebPrevFitMode(L"webprevemailfitmode", inipath);

	VECTORFITMODE.Option = ReadIniViewOptGraphicFitMode(L"vectorfitmode", inipath);
	BITMAPFITMODE.Option = ReadIniViewOptGraphicFitMode(L"bitmapfitmode", inipath);

	SPREADSHEETDISPLAYMODE.Option = ReadIniViewOptSpreadsheetDisplayMode(L"spreadsheetdisplaymode", inipath);
}



///////////////////////////////////////////////////////////////////////////////////////////////////



void clsVTOptionsViewer::SendVTViewOptions(const HWND SccviewerWindow)
{
	SCCVWOPTIONSPEC40 locOptionSpec;
	locOptionSpec.dwSize = sizeof(SCCVWOPTIONSPEC40);
	locOptionSpec.dwFlags = SCCVWOPTION_CURRENT;

	union
	{
		VTDWORD WPdisplaymode;
		VTDWORD HTMLdisplaymode;
		VTDWORD EMAILdisplaymode;

		VTDWORD WebPrevWPfitmode;
		VTDWORD WebPrevHTMLfitmode;
		VTDWORD WebPrevEMAILfitmode;

		VTDWORD Vectorfitmode;
		VTDWORD Bitmapfitmode;

		VTDWORD ArcSortOrder;

		VTBOOL SpreadsheetDraftMode;
		VTBOOL SpreadsheetHiddenCells;

		VTDWORD ScrollFlags;
	};

	// word processor display engine:
	locOptionSpec.dwId = SCCID_WPDISPLAYMODE;
	locOptionSpec.pData = &WPdisplaymode;
	SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	WPdisplaymode = WPDISPLAYMODE.FilterSkip(WPdisplaymode);
	SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// HTML display engine:
	locOptionSpec.dwId = SCCID_HTMLDISPLAYMODE;
	//locOptionSpec.pData = &HTMLdisplaymode;
	SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	HTMLdisplaymode = HTMLDISPLAYMODE.FilterSkip(HTMLdisplaymode);
	SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// email display engine:
	locOptionSpec.dwId = SCCID_EMAILDISPLAYMODE;
	//locOptionSpec.pData = &EMAILdisplaymode;
	SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	EMAILdisplaymode = EMAILDISPLAYMODE.FilterSkip(EMAILdisplaymode);
	SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	/*
	It's not working propertly due Outside In Viewer library internal bug.
	The last call SendMessage::SCCVW_SETOPTION always sets the settings for all document types, ignoring the SCCID_WPFITMODE, SCCID_HTMLFITMODE, and SCCID_EMAILFITMODE types.
	However, after calling SendMessage::SCCVW_SETOPTION, these settings are stored inside the library (in the .oit directory).
	Only settings later read by the library from the internal .oit storage work correctly.
	A workaround would be to have SendMessage::SCCVW_SETOPTION settings sent to the library only if they have been changed in the ini file.
	Thus, only the first call after the changes, due to the effect of SendMessage::SCCVW_SETOPTION, the view will be incorrect.

	// size of word processor pages when using weblayout/preview mode:
	locOptionSpec.dwId = SCCID_WPFITMODE;
	//locOptionSpec.pData = &WebPrevWPfitmode;
	SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	WebPrevWPfitmode = WEBPREVWPFITMODE.FilterSkip(WebPrevWPfitmode);
	SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// size of HTML pages when using weblayout/preview mode:
	locOptionSpec.dwId = SCCID_HTMLFITMODE;
	//locOptionSpec.pData = &WebPrevHTMLfitmode;
	SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	WebPrevHTMLfitmode = WEBPREVHTMLFITMODE.FilterSkip(WebPrevHTMLfitmode);
	SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// size of EMAIL pages when using weblayout/preview mode:
	locOptionSpec.dwId = SCCID_EMAILFITMODE;
	//locOptionSpec.pData = &WebPrevEMAILfitmode;
	SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	WebPrevEMAILfitmode = WEBPREVEMAILFITMODE.FilterSkip(WebPrevEMAILfitmode);
	SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
	*/

	/******************** WORKAROUND ********************/

	// size of word processor pages when using weblayout/preview mode:
	locOptionSpec.dwId = SCCID_WPFITMODE;
	//locOptionSpec.pData = &WebPrevWPfitmode;
	SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	if (WEBPREVWPFITMODE.Option != Opt::SKIP && WEBPREVWPFITMODE.Option != WebPrevWPfitmode)
	{
		WebPrevWPfitmode = WEBPREVWPFITMODE.Option;
		SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
	}

	// size of HTML pages when using weblayout/preview mode:
	locOptionSpec.dwId = SCCID_HTMLFITMODE;
	//locOptionSpec.pData = &WebPrevHTMLfitmode;
	SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	if (WEBPREVHTMLFITMODE.Option != Opt::SKIP && WEBPREVHTMLFITMODE.Option != WebPrevHTMLfitmode)
	{
		WebPrevHTMLfitmode = WEBPREVHTMLFITMODE.Option;
		SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
	}

	// size of EMAIL pages when using weblayout/preview mode:
	locOptionSpec.dwId = SCCID_EMAILFITMODE;
	//locOptionSpec.pData = &WebPrevEMAILfitmode;
	SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	if (WEBPREVEMAILFITMODE.Option != Opt::SKIP && WEBPREVEMAILFITMODE.Option != WebPrevEMAILfitmode)
	{
		WebPrevEMAILfitmode = WEBPREVEMAILFITMODE.Option;
		SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
	}

	/****************************************************/

	// vector display engine:
	locOptionSpec.dwId = SCCID_VECFITMODE;
	//locOptionSpec.pData = &Vectorfitmode;
	SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	Vectorfitmode = VECTORFITMODE.FilterSkip(Vectorfitmode);
	SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// bitmap display engine:
	locOptionSpec.dwId = SCCID_BMPFITMODE;
	//locOptionSpec.pData = &Bitmapfitmode;
	SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	Bitmapfitmode = BITMAPFITMODE.FilterSkip(Bitmapfitmode);
	SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// spreadsheet display engine-1:
	locOptionSpec.dwId = SCCID_SSDRAFTMODE;
	//locOptionSpec.pData = &SpreadsheetDraftMode;
	SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	SpreadsheetDraftMode = SPREADSHEETDISPLAYMODE.FilterSkipDraft(SpreadsheetDraftMode);
	SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// spreadsheet display engine-2:
	locOptionSpec.dwId = SCCID_SSSHOWHIDDENCELLS;
	//locOptionSpec.pData = &SpreadsheetHiddenCells;
	SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	SpreadsheetHiddenCells = SPREADSHEETDISPLAYMODE.FilterSkipHiddenCells(SpreadsheetHiddenCells);
	SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	/****************************************************/

	// reset to default non-INI options:

	// archive display engine:
	locOptionSpec.dwId = SCCID_ARCSORTORDER;
	//locOptionSpec.pData = &ArcSortOrder;
	ArcSortOrder = SCCVW_SORT_NAME;
	SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
}



///////////////////////////////////////////////////////////////////////////////////////////////////



clsSSViewModeOption::clsSSViewModeOption() { Option = UlisterSSDisplayMode::SKIP; }

VTBOOL clsSSViewModeOption::FilterSkipHiddenCells(VTBOOL val) const
{
	if (Option == UlisterSSDisplayMode::SKIP) return val;
	return (Option == UlisterSSDisplayMode::NORMALHIDDEN) ? TRUE : FALSE;
}

VTBOOL clsSSViewModeOption::FilterSkipDraft(VTBOOL val) const
{
	if (Option == UlisterSSDisplayMode::SKIP) return val;
	return (Option == UlisterSSDisplayMode::DRAFT) ? TRUE : FALSE;
}



///////////////////////////////////////////////////////////////////////////////////////////////////



clsVTDWORDOption::clsVTDWORDOption() { Option = Opt::SKIP; }
VTDWORD clsVTDWORDOption::FilterSkip(VTDWORD val) const { if (Option == Opt::SKIP) return val; else return Option; }



///////////////////////////////////////////////////////////////////////////////////////////////////



clsVTOptionsClipboard::clsVTOptionsClipboard()
{
	FORMAT_TEXT = Opt::SKIP;
	FORMAT_RTF = Opt::SKIP;
	FORMAT_UNICODE = Opt::SKIP;
	FORMAT_WINBITMAP = Opt::SKIP;
	FORMAT_WINDIB = Opt::SKIP;
	FORMAT_WINMETAFILE = Opt::SKIP;
	FORMAT_WINPALETTE = Opt::SKIP;
	OLE_ENABLEDRAGDROP = Opt::SKIP;
}



VTDWORD clsVTOptionsClipboard::Get_SCCVW_OLE(VTDWORD OLEFlags) const
{
	if (OLE_ENABLEDRAGDROP == Opt::SKIP) return OLEFlags;
	return (OLE_ENABLEDRAGDROP) ? SCCVW_OLE_ENABLEDRAGDROP : 0;
}



VTDWORD clsVTOptionsClipboard::Get_SCCVW_CLIPFORMAT(VTDWORD ClipFormat) const
{
	/*
	//ClipFormat = 0;
	wchar_t buf[ULISTMAXBUF];
	swprintf_s(buf, ULISTMAXBUF,
	L"SRC ClipFormat: 0x%08X (%u)\n"
	L"FORMAT_TEXT=%i\n"
	L"FORMAT_RTF=%i\n"
	L"FORMAT_UNICODE=%i\n"
	L"FORMAT_WINBITMAP=%i\n"
	L"FORMAT_WINDIB=%i\n"
	L"FORMAT_WINMETAFILE=%i\n"
	L"FORMAT_WINPALETTE=%i\n",
	ClipFormat, ClipFormat,
	FORMAT_TEXT, FORMAT_RTF, FORMAT_UNICODE, FORMAT_WINBITMAP, FORMAT_WINDIB, FORMAT_WINMETAFILE, FORMAT_WINPALETTE);
	MessageBoxW(NULL, buf, L"Get_SCCVW_CLIPFORMAT", MB_OK);
	*/

	// skip/set/reset bits
	if (FORMAT_TEXT != Opt::SKIP) ClipFormat = FORMAT_TEXT ? (ClipFormat | SCCVW_CLIPFORMAT_TEXT) : (ClipFormat & ~SCCVW_CLIPFORMAT_TEXT);
	if (FORMAT_RTF != Opt::SKIP) ClipFormat = FORMAT_RTF ? (ClipFormat | SCCVW_CLIPFORMAT_RTF) : (ClipFormat & ~SCCVW_CLIPFORMAT_RTF);
	if (FORMAT_UNICODE != Opt::SKIP) ClipFormat = FORMAT_UNICODE ? (ClipFormat | SCCVW_CLIPFORMAT_UNICODE) : (ClipFormat & ~SCCVW_CLIPFORMAT_UNICODE);
	if (FORMAT_WINBITMAP != Opt::SKIP) ClipFormat = FORMAT_WINBITMAP ? (ClipFormat | SCCVW_CLIPFORMAT_WINBITMAP) : (ClipFormat & ~SCCVW_CLIPFORMAT_WINBITMAP);
	if (FORMAT_WINDIB != Opt::SKIP) ClipFormat = FORMAT_WINDIB ? (ClipFormat | SCCVW_CLIPFORMAT_WINDIB) : (ClipFormat & ~SCCVW_CLIPFORMAT_WINDIB);
	if (FORMAT_WINMETAFILE != Opt::SKIP) ClipFormat = FORMAT_WINMETAFILE ? (ClipFormat | SCCVW_CLIPFORMAT_WINMETAFILE) : (ClipFormat & ~SCCVW_CLIPFORMAT_WINMETAFILE);
	if (FORMAT_WINPALETTE != Opt::SKIP) ClipFormat = FORMAT_WINPALETTE ? (ClipFormat | SCCVW_CLIPFORMAT_WINPALETTE) : (ClipFormat & ~SCCVW_CLIPFORMAT_WINPALETTE);

	/*
	swprintf_s(buf, ULISTMAXBUF,
	L"DST ClipFormat: 0x%08X (%u)\n",
	ClipFormat, ClipFormat);
	MessageBoxW(NULL, buf, L"Get_SCCVW_CLIPFORMAT", MB_OK);
	*/

	return ClipFormat;
}



__int8 clsVTOptionsClipboard::ReadIniClipbOpt(const wchar_t *optionname, const wchar_t *inipath)
{
	wchar_t buf[INT64STRMAXBUF];
	__int8 result;

	GetPrivateProfileStringW(CLIPBOARDSECTION, optionname, ASKIP, buf, INT64STRMAXBUF, inipath);
	if (_wcsicmp(buf, AON) == 0) result = Opt::ON;
	else if (_wcsicmp(buf, AOFF) == 0) result = Opt::OFF;
	else result = Opt::SKIP;

	return result;
}



VTDWORD clsVTOptionsClipboard::ReadIniClipbSubFormat(const wchar_t *optionname, const wchar_t *inipath)
{
	// Spreadsheet Or Database Copy-Paste
	wchar_t buf[INT64STRMAXBUF];
	VTDWORD result;

	GetPrivateProfileStringW(CLIPBOARDSECTION, optionname, ASKIP, buf, INT64STRMAXBUF, inipath);
	if (_wcsicmp(buf, L"rtf") == 0) result = SCCVW_CLIPSUBFORMAT_TABLE;
	else if (_wcsicmp(buf, L"tabs") == 0) result = SCCVW_CLIPSUBFORMAT_TABS;
	else if (_wcsicmp(buf, L"optimizedtabs") == 0) result = SCCVW_CLIPSUBFORMAT_OPTIMIZEDTABS;
	else result = Opt::SKIP;

	return result;
}



void clsVTOptionsClipboard::LoadClipboardOptions(const wchar_t *inipath)
{
	FORMAT_TEXT = ReadIniClipbOpt(L"ascii", inipath);
	FORMAT_RTF = ReadIniClipbOpt(L"rtf", inipath);
	FORMAT_UNICODE = ReadIniClipbOpt(L"unicode", inipath);
	FORMAT_WINBITMAP = ReadIniClipbOpt(L"bitmap", inipath);
	FORMAT_WINDIB = ReadIniClipbOpt(L"windib", inipath);
	FORMAT_WINMETAFILE = ReadIniClipbOpt(L"metafile", inipath);
	FORMAT_WINPALETTE = ReadIniClipbOpt(L"palette", inipath);

	OLE_ENABLEDRAGDROP = ReadIniClipbOpt(L"dragdrop", inipath);

	SSCLIPBOARDSUBFORMAT.Option = ReadIniClipbSubFormat(L"spreadsheet", inipath);
	DBCLIPBOARDSUBFORMAT.Option = ReadIniClipbSubFormat(L"database", inipath);
}



void clsVTOptionsClipboard::SendVTClipOptions(const HWND SccviewerWindow)
{
	SCCVWOPTIONSPEC40 locOptionSpec;
	locOptionSpec.dwSize = sizeof(SCCVWOPTIONSPEC40);
	locOptionSpec.dwFlags = SCCVWOPTION_CURRENT;

	union
	{
		VTDWORD ClipFormat;
		VTDWORD OLEFlags;

		VTDWORD SpreadsheetClipboard;
		VTDWORD DatabaseClipboard;
	};

	// unicode clipboard:
	locOptionSpec.dwId = SCCID_TOCLIPBOARD;
	locOptionSpec.pData = &ClipFormat;
	SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	ClipFormat = Get_SCCVW_CLIPFORMAT(ClipFormat);
	SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// drag-and-drop copying:
	locOptionSpec.dwId = SCCID_OLEFLAGS;
	//locOptionSpec.pData = &OLEFlags;
	SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	OLEFlags = Get_SCCVW_OLE(OLEFlags);
	SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// spreadsheet copying:
	locOptionSpec.dwId = SCCID_SSCLIPBOARD;
	//locOptionSpec.pData = &SpreadsheetClipboard;
	SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	SpreadsheetClipboard = SSCLIPBOARDSUBFORMAT.FilterSkip(SpreadsheetClipboard);
	SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);

	// database copying:
	locOptionSpec.dwId = SCCID_DBCLIPBOARD;
	//locOptionSpec.pData = &DatabaseClipboard;
	SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	DatabaseClipboard = DBCLIPBOARDSUBFORMAT.FilterSkip(DatabaseClipboard);
	SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
}



///////////////////////////////////////////////////////////////////////////////////////////////////



VTDWORD clsVTOptionsMemoryManager::ReadIniViewOptBufferSize(const wchar_t *optionname, const wchar_t *inipath)
{
	wchar_t buf[INT64STRMAXBUF];

	GetPrivateProfileStringW(MEMORYSECTION, optionname, ASKIP, buf, INT64STRMAXBUF, inipath);
	return (_wcsicmp(buf, ASKIP) == 0) ? Opt::SKIP : (VTDWORD)wcstol(buf, NULL, 0); // boundary check in clsVTOptionsMemoryManager::SendVTMemOptions; BASE=0 (AUTO)
}



///////////////////////////////////////////////////////////////////////////////////////////////////



VTDWORD clsVTOptionsMemoryManager::ReadIniViewOptMemoryMode(const wchar_t *optionname, const wchar_t *inipath)
{
	wchar_t buf[INT64STRMAXBUF];
	VTDWORD result;

	GetPrivateProfileStringW(MEMORYSECTION, optionname, ASKIP, buf, INT64STRMAXBUF, inipath);
	if (_wcsicmp(buf, L"4m") == 0) result = SCCDOCUMENTMEMORYMODE_SMALLEST;
	else if (_wcsicmp(buf, L"16m") == 0) result = SCCDOCUMENTMEMORYMODE_SMALL;
	else if (_wcsicmp(buf, L"64m") == 0) result = SCCDOCUMENTMEMORYMODE_MEDIUM;
	else if (_wcsicmp(buf, L"256m") == 0) result = SCCDOCUMENTMEMORYMODE_LARGE;
	else if (_wcsicmp(buf, L"1024m") == 0) result = SCCDOCUMENTMEMORYMODE_LARGEST;
	else result = Opt::SKIP;

	return result;
}



///////////////////////////////////////////////////////////////////////////////////////////////////



void clsVTOptionsMemoryManager::LoadMemoryOptions(const wchar_t *inipath)
{
	ReadBufferSize.Option = ReadIniViewOptBufferSize(L"readbuffersizekb", inipath);
	MMapBufferSize.Option = ReadIniViewOptBufferSize(L"mmapbuffersizekb", inipath);
	TempBufferSize.Option = ReadIniViewOptBufferSize(L"tempbuffersizekb", inipath);

	MemoryMode.Option = ReadIniViewOptMemoryMode(L"memorymode", inipath);
}



///////////////////////////////////////////////////////////////////////////////////////////////////



void clsVTOptionsMemoryManager::SendVTMemOptions(const HWND SccviewerWindow)
{
	SCCVWOPTIONSPEC40 locOptionSpec;
	locOptionSpec.dwSize = sizeof(SCCVWOPTIONSPEC40);
	locOptionSpec.dwFlags = SCCVWOPTION_CURRENT;

	union
	{
		SCCBUFFEROPTIONS iobufsize;

		VTDWORD VTMemoryMode;
	};

	// memory buffers:
	// We will only send a new value if it has actually changed in the ulister.ini file.
	locOptionSpec.dwId = SCCOPT_IO_BUFFERSIZE;
	locOptionSpec.pData = &iobufsize;
	iobufsize.dwFlags = 0;
	SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)&locOptionSpec); // return: iobufsize.dwFlags:=7 (undocumented)!!!

#if defined (__ULISTDEBUGMSG) && defined(__ULISTDEBUGMEMCFG)
	std::wstring msgW = L"(ReadBufferSize=" + ToStrW(iobufsize.dwReadBufferSize) + L", MMapBufferSize=" + ToStrW(iobufsize.dwMMapBufferSize) + L", TempBufferSize=" + ToStrW(iobufsize.dwTempBufferSize) + L", Flags=" + ToStrW(iobufsize.dwFlags) + L")";
	OutputDebugStringW(msgW.c_str());
	msgW = L"(INI_ReadBuffer=" + ToStrW(ReadBufferSize.Option) + L", INI_MMapBuffer=" + ToStrW(MMapBufferSize.Option) + L", INI_TempBuffer=" + ToStrW(TempBufferSize.Option) + L")";
	OutputDebugStringW(msgW.c_str());
#endif

	iobufsize.dwFlags = 0; // !!!

	if (ReadBufferSize.Option != Opt::SKIP)
		if (ReadBufferSize.Option != iobufsize.dwReadBufferSize) // if possible, do not change the value
		{
			//OutputDebugStringW(L"-1-");
			iobufsize.dwFlags = iobufsize.dwFlags | SCCBUFOPT_SET_READBUFSIZE;
			iobufsize.dwReadBufferSize = ReadBufferSize.Option;

			if (iobufsize.dwReadBufferSize < SCCBUFOPT_MIN_READBUFSIZE) iobufsize.dwReadBufferSize = SCCBUFOPT_MIN_READBUFSIZE;
			if (iobufsize.dwReadBufferSize > SCCBUFOPT_MAX_READBUFSIZE) iobufsize.dwReadBufferSize = SCCBUFOPT_MAX_READBUFSIZE;
		}

	if (MMapBufferSize.Option != Opt::SKIP)
		if (MMapBufferSize.Option != iobufsize.dwMMapBufferSize) // if possible, do not change the value
		{
			//OutputDebugStringW(L"-2-");
			iobufsize.dwFlags = iobufsize.dwFlags | SCCBUFOPT_SET_MMAPBUFSIZE;
			iobufsize.dwMMapBufferSize = MMapBufferSize.Option;

			//if (iobufsize.dwMMapBufferSize < SCCBUFOPT_MIN_MMAPBUFSIZE) iobufsize.dwMMapBufferSize = SCCBUFOPT_MIN_MMAPBUFSIZE;
			if (iobufsize.dwMMapBufferSize > SCCBUFOPT_MAX_MMAPBUFSIZE) iobufsize.dwMMapBufferSize = SCCBUFOPT_MAX_MMAPBUFSIZE;
		}

	if (TempBufferSize.Option != Opt::SKIP)
		if (TempBufferSize.Option != iobufsize.dwTempBufferSize) // if possible, do not change the value
		{
			//OutputDebugStringW(L"-3-");
			iobufsize.dwFlags = iobufsize.dwFlags | SCCBUFOPT_SET_TEMPBUFSIZE;
			iobufsize.dwTempBufferSize = TempBufferSize.Option;

			//if (iobufsize.dwTempBufferSize < SCCBUFOPT_MIN_TEMPBUFSIZE) iobufsize.dwTempBufferSize = SCCBUFOPT_MIN_TEMPBUFSIZE;
			if (iobufsize.dwTempBufferSize > SCCBUFOPT_MAX_TEMPBUFSIZE) iobufsize.dwTempBufferSize = SCCBUFOPT_MAX_TEMPBUFSIZE;
		}

#if defined (__ULISTDEBUGMSG) && defined(__ULISTDEBUGMEMCFG)
	msgW = L"(dwReadBuffer=" + ToStrW(iobufsize.dwReadBufferSize) + L", dwMMapBuffer=" + ToStrW(iobufsize.dwMMapBufferSize) + L", dwTempBuffer=" + ToStrW(iobufsize.dwTempBufferSize) + L", flags=" + ToStrW(iobufsize.dwFlags) + L")";
	OutputDebugStringW(msgW.c_str());
#endif

	if (iobufsize.dwFlags) SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec); // if possible, do not change the value

#if defined (__ULISTDEBUGMSG) && defined(__ULISTDEBUGMEMCFG)
	iobufsize.dwFlags = 0;
	SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)&locOptionSpec);
	msgW = L"(NEW_ReadBufferSize=" + ToStrW(iobufsize.dwReadBufferSize) + L", NEW_MMapBufferSize=" + ToStrW(iobufsize.dwMMapBufferSize) + L", NEW_TempBufferSize=" + ToStrW(iobufsize.dwTempBufferSize) + L")";
	OutputDebugStringW(msgW.c_str());
#endif

	/****************************************************/

	// chunker memory:
	locOptionSpec.dwId = SCCOPT_DOCUMENTMEMORYMODE;
	//locOptionSpec.pData = &VTMemoryMode;
	SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
#if defined (__ULISTDEBUGMSG) && defined(__ULISTDEBUGMEMCFG)
	msgW = L"MemoryMode=" + ToStrW(VTMemoryMode);
	OutputDebugStringW(msgW.c_str());
#endif
	if (MemoryMode.Option != Opt::SKIP)
		if (MemoryMode.Option != VTMemoryMode) // if possible, do not change the value
		{
			OutputDebugStringW(L"* Change MemoryMode *");
			VTMemoryMode = MemoryMode.Option;
			SendMessage(SccviewerWindow, SCCVW_SETOPTION, 0, (LPARAM)&locOptionSpec);
		}

#if defined (__ULISTDEBUGMSG) && defined(__ULISTDEBUGMEMCFG)
	SendMessage(SccviewerWindow, SCCVW_GETOPTION, 0, (LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
	msgW = L"NEW_MemoryMode=" + ToStrW(VTMemoryMode);
	OutputDebugStringW(msgW.c_str());
#endif
}



///////////////////////////////////////////////////////////////////////////////////////////////////



void clsVTOptions::LoadVTOptions(const wchar_t *inipath)
{
	VTClipboard.LoadClipboardOptions(inipath);
	VTViewer.LoadViewerOptions(inipath);
	VTMemoryManager.LoadMemoryOptions(inipath);
}



void clsVTOptions::SendVTOptions(const HWND SccviewerWindow)
{
	VTClipboard.SendVTClipOptions(SccviewerWindow);
	VTViewer.SendVTViewOptions(SccviewerWindow);
	VTMemoryManager.SendVTMemOptions(SccviewerWindow);
}
