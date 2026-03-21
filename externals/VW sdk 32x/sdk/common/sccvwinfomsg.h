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
#define SCCVW_INFO_BADPAGESIZE          18   // RNT for 27301827, 30/Jan/2018
#define SCCVW_INFO_PDFONETOMANY         19
#define SCCVW_INFO_LOWBADPAGESIZE       20   // RNT, bug 24675020, 25-APR-2018
#define SCCVW_INFO_ISBIDI               21   // aanudeep for 27746695, 31/Aug/2018
#define SCCVW_INFO_UNSUPPORTEDFONT      22   // aanudeep for 28973155, 12/Dec/2018
#define SCCVW_INFO_MAXPATH_LIMIT        23   // RNT, bug 28044429, 30/APR/2019
#define SCCVW_INFO_CURRPATH_SIZE        24   // RNT, bug 28044429, 30/APR/2019
#define SCCVW_INFO_MAXVECT_LIMIT        25   // RNT, bug 28044429, 30/APR/2019
#define SCCVW_INFO_CURRVECT_SIZE        26   // RNT, bug 28044429, 30/APR/2019
#define SCCVW_INFO_MAXOBJT_LIMIT        27   // RNT, bug 28044429, 30/APR/2019
#define SCCVW_INFO_CURROBJT_SIZE        28   // RNT, bug 28044429, 30/APR/2019
#define SCCVW_INFO_ISCIDTYPE0CFFFONT    29   // aanudeep, bugfix:32293790, 02/FEB/2021
#define SCCVW_INFO_WIDTHUSEDINPDF       30   // RNT, bug 31754216, 30/JAN/2021
#define SCCVW_INFO_CIDTYPE0FONT         31   // RNT, bug 33125678, 14/Aug/2021
#define SCCVW_INFO_CIDTYPE2FONT         32   // RNT, bug 33125678, 14/Aug/2021

#define SCCVW_INFOMESSAGE_TOP           32   // this should be the maximum of the above values

#endif
