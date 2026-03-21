/* Copyright (c) 2001, 2016, Oracle and/or its affiliates. 
All rights reserved.*/
/*   
|   Include File sccvwinfomsg.h
|   Constants for use with SCCVW_INFOMESSAGE message.
*/


#ifndef _SCCVWINFOMSG_H
#define _SCCVWINFOMSG_H


// Possible Values for SCCVW_INFOMESSAGE 
#define SCCVW_INFO_EQUATION              1
#define SCCVW_INFO_BADCOMPRESSION        2
#define SCCVW_INFO_BADCOLORSPACE         3
#define SCCVW_INFO_FORMS                 4
#define SCCVW_INFO_MISSINGMAP            5
#define SCCVW_INFO_VERTICALTEXT          6
#define SCCVW_INFO_TEXTEFFECTS           7
#define SCCVW_INFO_RTTOLFTTABLES         8
#define SCCVW_INFO_ALIASEDFONT           9
#define SCCVW_INFO_MISSINGFONT          10
#define SCCVW_INFO_SUBDOCFAILED         11
#define SCCVW_INFO_TYPETHREEFONT        12
#define SCCVW_INFO_BADSHADING           13
#define SCCVW_INFO_BADHTML              14
#define SCCVW_INFO_VECTOROBJECTLIMIT    15
#define SCCVW_INFO_INVALIDANNO          16
#define SCCVW_INFO_INLINEIMAGE          17
#define SCCVW_INFO_BADPAGESIZE          18 
#define SCCVW_INFO_PDFONETOMANY         19
#define SCCVW_INFO_LOWBADPAGESIZE       20 
#define SCCVW_INFO_ISBIDI               21 
#define SCCVW_INFO_UNSUPPORTEDFONT      22 
#define SCCVW_INFO_MAXPATH_LIMIT        23 
#define SCCVW_INFO_CURRPATH_SIZE        24 
#define SCCVW_INFO_MAXVECT_LIMIT        25 
#define SCCVW_INFO_CURRVECT_SIZE        26 
#define SCCVW_INFO_MAXOBJT_LIMIT        27 
#define SCCVW_INFO_CURROBJT_SIZE        28 
#define SCCVW_INFO_ISCIDTYPE0CFFFONT    29 
#define SCCVW_INFO_WIDTHUSEDINPDF       30 
#define SCCVW_INFO_CIDTYPE0FONT         31 
#define SCCVW_INFO_CIDTYPE2FONT         32 
#define SCCVW_INFO_PDFIMAGEEXIST        33
#define SCCVW_INFO_PDFFONTEXIST         34

#define SCCVW_INFOMESSAGE_TOP           34   // this should be the maximum of the above values

#endif
