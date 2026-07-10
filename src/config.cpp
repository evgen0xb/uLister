/*
The plugin is provided as-is and without any warranty under the GPLv3 license.
*/

#include <windows.h>

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
}



///////////////////////////////////////////////////////////////////////////////////////////////////



VTDWORD clsVTOptionsViewer::ReadIniViewOptDisplay(const wchar_t *optionname, wchar_t *inipath)
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



VTDWORD clsVTOptionsViewer::ReadIniViewOptWebPrevFitMode(const wchar_t *optionname, wchar_t *inipath)
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



VTDWORD clsVTOptionsViewer::ReadIniViewOptGraphicFitMode(const wchar_t *optionname, wchar_t *inipath)
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



__int8 clsVTOptionsViewer::ReadIniViewOptSpreadsheetDisplayMode(const wchar_t *optionname, wchar_t *inipath)
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



VTDWORD clsVTOptionsViewer::ReadIniViewOptBufferSize(const wchar_t *optionname, wchar_t *inipath)
{
	wchar_t buf[INT64STRMAXBUF];

	GetPrivateProfileStringW(MEMORYSECTION, optionname, ASKIP, buf, INT64STRMAXBUF, inipath);
	return (_wcsicmp(buf, ASKIP) == 0) ? Opt::SKIP : (VTDWORD)wcstol(buf, NULL, 0); // boundary check in clsVTWindowInstance::SendVTOptions; BASE=0 (AUTO)
}



///////////////////////////////////////////////////////////////////////////////////////////////////



VTDWORD clsVTOptionsViewer::ReadIniViewOptMemoryMode(const wchar_t *optionname, wchar_t *inipath)
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



void clsVTOptionsViewer::LoadViewerOptions(wchar_t *inipath)
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

	ReadBufferSize.Option = ReadIniViewOptBufferSize(L"readbuffersizekb", inipath);
	MMapBufferSize.Option = ReadIniViewOptBufferSize(L"mmapbuffersizekb", inipath);
	TempBufferSize.Option = ReadIniViewOptBufferSize(L"tempbuffersizekb", inipath);

	MemoryMode.Option = ReadIniViewOptMemoryMode(L"memorymode", inipath);
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



__int8 clsVTOptionsClipboard::ReadIniClipbOpt(const wchar_t *optionname, wchar_t *inipath)
{
	wchar_t buf[INT64STRMAXBUF];
	__int8 result;

	GetPrivateProfileStringW(CLIPBOARDSECTION, optionname, ASKIP, buf, INT64STRMAXBUF, inipath);
	if (_wcsicmp(buf, AON) == 0) result = Opt::ON;
	else if (_wcsicmp(buf, AOFF) == 0) result = Opt::OFF;
	else result = Opt::SKIP;

	return result;
}



VTDWORD clsVTOptionsClipboard::ReadIniClipbSubFormat(const wchar_t *optionname, wchar_t *inipath)
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



void clsVTOptionsClipboard::LoadClipboardOptions(wchar_t *inipath)
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



///////////////////////////////////////////////////////////////////////////////////////////////////



void clsVTOptions::LoadVTOptions(wchar_t *inipath)
{
	VTClipboard.LoadClipboardOptions(inipath);
	VTViewer.LoadViewerOptions(inipath);
}

