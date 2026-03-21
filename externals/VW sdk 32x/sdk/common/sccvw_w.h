/* Copyright (c) 2001, 2016, Oracle and/or its affiliates. All rights reserved. */



  /*
  |  SCC Viewer Technology - Include
  |
  |  Include:       SCCVW_W.H (included in SCCVW.H)
  |  Environment:   Windows
  |  Function:      Windows specific definitions for
  |                 Outside In Viewer Technology Specification 4.0
  |
  */

#ifndef SCC_PACK_SET
#define SCC_PACK_SET 1
#define SCC_PACKED_BY_SCCVW_W_H 1
#pragma pack(push,8)
#endif /* SCC_PACK_SET */

#define SCCVW_START        WM_USER

#define SCCVIEWER_DLL(a)    "SC"#a"VW.DLL"
#define SCCVIEWER_CLASS(a)  "SCCVIEWER"#a

  /*
  |  Size defines
  */

#define SCCVW_DISPLAYNAMEMAX    MAX_PATH
#define SCCVW_FILENAMEMAX       MAX_PATH

#define SCCVW_FILEIDNAMEMAX     80
#define SCCVW_DEFAULTMENUMAX    5000


/*
 | The following structures/constants/etc. are defined in the Win32V-specific
 | header file
 |
 |  But the application interface won't have Win32V stuff exposed,
 |  so we still need these definitions
 */

  /*
  |
  |  SCCVWPRINTEX structures
  |
  */

typedef struct SCCVWPRINTEX40tag
  {
  VTDWORD    dwSize;      /* sizeof(SCCVWPRINTEX40) */
  VTDWORD    dwFlags;
  HWND       hParentWnd;
  HDC        hPrinterDC;
  VTTCHAR    szPrinter[128];
  VTTCHAR    szPort[128];
  VTTCHAR    szDriver[128];
  VTBOOL     bPrintSelectionOnly;
  VTBOOL     bDoSetupDialog;
  VTBOOL     bDoAbortDialog;
  VTBOOL     bPrintHeader;
  VTBOOL     bStartDocAlreadyDone;
  VTTCHAR    szJobName[40];
  VTDWORD    dwTopMargin;
  VTDWORD    dwBottomMargin;
  VTDWORD    dwLeftMargin;
  VTDWORD    dwRightMargin;
  VTTCHAR    szDefaultFont[32];
  VTWORD     wDefaultFontSize;    /* in twips */
  FARPROC    pAbortProc;
  VTBOOL     bCollate;
  VTDWORD    dwCopies;
  VTHANDLE   hDevMode;
  } SCCVWPRINTEX40, * LPSCCVWPRINTEX40;

#define SCCVW_USEPRINTERDC            0x00000001
#define SCCVW_USEPRINTERNAME          0x00000002
#define SCCVW_USEPRINTSELECTIONONLY   0x00000004
#define SCCVW_USEJOBNAME              0x00000008
#define SCCVW_USEMARGINS              0x00000010
#define SCCVW_USEPRINTHEADER          0x00000020
#define SCCVW_USEDEFAULTFONT          0x00000040
#define SCCVW_USEABORTPROC            0x00000080
#define SCCVW_DONTDISABLEPARENTS      0x00000100 /* no longer used */
#define SCCVW_USECOLLATE              0x00000200
#define SCCVW_USECOPIES               0x00000400
#define SCCVW_USEDEVMODE              0x00000800

  /*
  |
  |  SCCVWDRAWPAGE40 structure
  |
  */

typedef struct SCCVWDRAWPAGE40tag
  {
  VTDWORD    dwSize;
  VTDWORD    dwPageToDraw;
  VTDWORD    dwReserved;
  VTDWORD    dwFlags;
  VTLONG     lUnitsPerInch;
  VTLONG     lFormatWidth;
  VTLONG     lFormatHeight;
  VTLONG     lTop;
  VTLONG     lLeft;
  VTLONG     lBottom;
  VTLONG     lRight;
  VTLONG     lResultTop;
  VTLONG     lResultLeft;
  VTLONG     lResultBottom;
  VTLONG     lResultRight;
  HDC        hOutputDC;
  HDC        hFormatDC;
  } SCCVWDRAWPAGE40, * PSCCVWDRAWPAGE40;

  /*
  |
  |  SCCVWDRAWPAGE41 structure
  |
  */

typedef struct SCCVWDRAWPAGE41tag
  {
  VTDWORD    dwSize;
  VTDWORD    dwPageToDraw;
  VTDWORD    dwReserved;
  VTDWORD    dwFlags;
  VTLONG     lUnitsPerInch;
  VTLONG     lFormatWidth;
  VTLONG     lFormatHeight;
  VTLONG     lTop;
  VTLONG     lLeft;
  VTLONG     lBottom;
  VTLONG     lRight;
  VTLONG     lResultTop;
  VTLONG     lResultLeft;
  VTLONG     lResultBottom;
  VTLONG     lResultRight;
  HDC        hOutputDC;
  HDC        hFormatDC;
  HPALETTE hPalette;
  } SCCVWDRAWPAGE41, * PSCCVWDRAWPAGE41;


#define SCCVW_DPFLAG_RETURNPALETTE          0x0001
#define SCCVW_DPFLAG_NOTMETAFILE            0x0002
#define SCCVW_DPFLAG_IGNOREBACKGROUND       0x0004
#define SCCVW_DPFLAG_DETERMINEOUTPUTTYPE    0x0008
#define SCCVW_DPFLAG_DETERMINEIMAGEPIXEL    0x0016

  /*
  |
  |  SCCVWDRAWPAGEINFO structure
  |
  */

typedef struct SCCVWDRAWPAGEINFOtag
  {
  VTDWORD    dwSize;
  VTDWORD    dwPageToDraw;
  VTDWORD    dwReserved;
  VTDWORD    dwFlags;
  VTLONG     lUnitsPerInch;
  VTLONG     lFormatWidth;
  VTLONG     lFormatHeight;
  VTLONG	 lTableWidth;
  VTLONG     lDocumentHeight;
  HDC        hOutputDC;
  HDC        hFormatDC;
  } SCCVWDRAWPAGEINFO, * PSCCVWDRAWPAGEINFO;


/*
  |
  |  SCCVWPAGECOUNT structure
  |
  */

typedef struct SCCVWPAGECOUNTtag
  {
  VTDWORD    dwSize;
  VTDWORD    dwNumPages;  
  VTDWORD    dwFlags;
  VTLONG     lUnitsPerInch;
  VTLONG     lFormatWidth;
  VTLONG     lFormatHeight;
  VTLONG     lTop;
  VTLONG     lLeft;
  VTLONG     lBottom;
  VTLONG     lRight;  
  HDC        hFormatDC;
  } SCCVWPAGECOUNT, * PSCCVWPAGECOUNT;




#ifdef SCC_PACKED_BY_SCCVW_W_H
#pragma pack(pop)
#undef SCC_PACKED_BY_SCCVW_W_H
#undef SCC_PACK_SET
#endif /* SCC_PACKED_BY_SCCVW_W_H */
