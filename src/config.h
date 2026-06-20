/*
The plugin is provided as-is and without any warranty under the GPLv3 license.
*/

#ifndef ULISTERCONFIG
#define ULISTERCONFIG

#include <windows.h>
#include <sccvw.h>



#define ULISTMAXBUF 1024
#define VTMAXSEARCHBUF 80 // don't change!
#define VTMAXTYPENAMEBUF 128
#define INT64STRMAXBUF 24



#define WINDOWS7BETABUILDNUMBER 7000
#define WINDOWS8BUILDNUMBER 9200



typedef char __VTTYPENAMEBUF[VTMAXTYPENAMEBUF];



namespace Opt { // Outside In Technology option representation
	enum Type {
		SKIP = -1, // don't change
		OFF = 0, // set FALSE
		ON = 1 // set TRUE
	};
} // VS2005 fix



namespace UlisterSSDisplayMode {
	enum Type {
		SKIP = -1,
		DRAFT = 0,
		NORMAL,
		NORMALHIDDEN,
	};
} // VS2005 fix



class clsUlisterOptions
{
public:
	wchar_t	ininoloadtypes[ULISTMAXBUF];
	wchar_t	inionlyloadtypes[ULISTMAXBUF];
	wchar_t	ininopreviewtypes[ULISTMAXBUF];
	wchar_t	inionlypreviewtypes[ULISTMAXBUF];
	bool	keepinmemory;
	bool	mwhscrollinvert;

	UINT ToolTipTimer;
	WORD ToolTipTransparency;

	void LoadUlisterOptions(wchar_t *inipath, unsigned long long WindowsBuildNumber);

};



class clsVTDWORDOption // simpest VTDWORD-wrapper for extra value 'SKIP'
{
public:
	VTDWORD Option;
	clsVTDWORDOption();
	VTDWORD FilterSkip(VTDWORD val) const;
};



class clsVTOptionsClipboard
{
public:

	clsVTDWORDOption SSCLIPBOARDSUBFORMAT;
	clsVTDWORDOption DBCLIPBOARDSUBFORMAT;
	__int8 FORMAT_TEXT;
	__int8 FORMAT_RTF;
	__int8 FORMAT_UNICODE;
	__int8 FORMAT_WINBITMAP;
	__int8 FORMAT_WINDIB;
	__int8 FORMAT_WINMETAFILE;
	__int8 FORMAT_WINPALETTE;
	__int8 OLE_ENABLEDRAGDROP;

	clsVTOptionsClipboard();
	VTDWORD Get_SCCVW_CLIPFORMAT(VTDWORD ClipFormat) const;
	VTDWORD Get_SCCVW_OLE(VTDWORD OLEFlags) const;

	void LoadClipboardOptions(wchar_t *inipath);

private:
	__int8 ReadIniClipbOpt(const wchar_t *optionname, wchar_t *inipath);
	VTDWORD ReadIniClipbSubFormat(const wchar_t *optionname, wchar_t *inipath);

}; // clsVTOptionsClipboard



class clsSSViewModeOption
{
public:
	__int8 Option;
	clsSSViewModeOption();
	VTBOOL FilterSkipHiddenCells(VTBOOL val) const;
	VTBOOL FilterSkipDraft(VTBOOL val) const;
};



class clsVTOptionsViewer
{
public:

	clsVTDWORDOption WPDISPLAYMODE;
	clsVTDWORDOption HTMLDISPLAYMODE;
	clsVTDWORDOption EMAILDISPLAYMODE;

	clsVTDWORDOption WEBPREVWPFITMODE;
	clsVTDWORDOption WEBPREVHTMLFITMODE;
	clsVTDWORDOption WEBPREVEMAILFITMODE;

	clsVTDWORDOption VECTORFITMODE;
	clsVTDWORDOption BITMAPFITMODE;

	clsSSViewModeOption SPREADSHEETDISPLAYMODE;

	void LoadViewerOptions(wchar_t *inipath);

private:
	VTDWORD ReadIniViewOptDisplay(const wchar_t *optionname, wchar_t *inipath);
	VTDWORD ReadIniViewOptWebPrevFitMode(const wchar_t *optionname, wchar_t *inipath);
	VTDWORD ReadIniViewOptGraphicFitMode(const wchar_t *optionname, wchar_t *inipath);
	__int8 ReadIniViewOptSpreadsheetDisplayMode(const wchar_t *optionname, wchar_t *inipath);

}; // clsVTOptionsViewer



class clsVTOptions
{
public:
	clsVTOptionsClipboard	VTClipboard;
	clsVTOptionsViewer		VTViewer;

	void LoadVTOptions(wchar_t *inipath);
};

#endif
