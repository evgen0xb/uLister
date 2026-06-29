# Supported File Format Matrix for Oracle Outside In 8.5.8 Release

## Overview

This document provides a comprehensive matrix of file formats supported by Oracle Outside-In Technology (OIT) version 8.5.8.  
Outside In is a powerful suite of software development kits (SDKs) used for file viewing, conversion, and text extraction in a wide range of enterprise applications.  

The Supported File Format Matrix helps developers, integrators, and quality assurance teams:  

- Understand which file types are natively supported
- Identify the version coverage of each format
- Determine any known limitations or special considerations
- Plan test coverage and compatibility verification for their integration

## How to Use This Matrix

The matrix is organized into categories for easy reference:  

- **Archive Formats** (e.g., ZIP, RAR, 7z)
- **Database Formats** (e.g., Access, dBase, MySQL)
- **Raster Image Formats** (e.g., TIFF, JPEG, PNG)
- **Email Formats** (e.g., MSG, EML)
- **Multimedia Formats** (e.g., MP3, MP4)
- **Text & Markup Formats** (e.g., HTML, XML, RTF)
- **Vector Image Formats** (e.g., AutoCAD, Visio)
- **Word Processing Formats** (e.g., Word, WordPerfect)
- **Spreadsheet Formats** (e.g., Excel, Quattro Pro)
- **Presentation Formats** (e.g., PowerPoint)
- **Other Formats** (miscellaneous types)
- **E-Book Formats**

Each section contains a table with the following columns:  

| Format                      | Version                               | Notes                                                                       |
| --------------------------- | ------------------------------------- | --------------------------------------------------------------------------- |
| The name of the file format | The version(s) supported by OIT 8.5.8 | Important details such as limitations, partial support, or special handling |

### Example Entry

| Format | Version | Notes                        |
| ------ | ------- | ---------------------------- |
| RAR    | 1.5–6.x | Split archives not supported |

This means that RAR files from version 1.5 to 6.x are supported, but split archives (.part1.rar, etc.) are not.

### Important Considerations

- **File ID Only** entries indicate that OIT can identify the file type but does not render or extract its content.
- Some formats are supported in **text extraction only** mode.
- Certain features (e.g., advanced multimedia rendering, embedded objects) may not be fully supported.
- Always refer to the [Oracle Outside In Documentation](https://docs.oracle.com/en/middleware/standalone/outsidein/8.5/index.html) for the most up-to-date details and platform-specific notes.

## Version Information

This matrix applies specifically to:  

- **Product**: Oracle Outside-In Technology
- **Release**: 8.5.8


## Archive Formats

| Format                                | Version                 | Notes                        |
| ------------------------------------- | ----------------------- | ---------------------------- |
| 7z                                    |                         | Split archives not supported |
| 7z Self Extracting exe                |                         | Split archives not supported |
| ACE Archive Compressed                |                         |                              |
| ALZ                                   |                         | File ID only                 |
| Amiga LZX Compressed File             |                         | File ID only                 |
| APK (.ZIP/JAR File)                   |                         | File ID only                 |
| ARJ Archive Compressed                |                         | File ID only                 |
| Bzip2 compressed data                 | .tbz/.bz2               | File ID only                 |
| CAB (Microsoft Cabinet)               | 95–97                   |                              |
| DAR Disk Archiver File                |                         | File ID only                 |
| DEB Debian Linux                      |                         | File ID only                 |
| DMG Mac OS X Disk Image               |                         | File ID only                 |
| DZ Dzip Compressed Archive            |                         | File ID only                 |
| ISO 9660 Disc Image                   |                         | File ID only                 |
| KGB archive                           |                         | File ID only                 |
| LBR file                              |                         | File ID only                 |
| LZ                                    |                         | File ID only                 |
| LZ4 compressed data                   |                         | File ID only                 |
| LZH Compress                          |                         |                              |
| LZMA compressed archive               |                         | File ID only                 |
| LZO file                              |                         | File ID only                 |
| LZA Self Extracting Compress          |                         |                              |
| MPQ file                              |                         | File ID only                 |
| PEA Zip file format                   |                         | File ID only                 |
| PKG (XAR Archive Compressed)          |                         | File ID only                 |
| Power ISO Direct-Access-Archive (DAA) |                         | File ID only                 |
| RAR                                   | 1.5, 2.0, 2.9, 5.x, 6.x |                              |
| RPM Package                           |                         | File ID only                 |
| SIT/SITX (StuffIt)                    |                         | File ID only                 |
| TAR                                   |                         |                              |
| TGZ (UNIX GZip)                       |                         | File ID only                 |
| TGS (UNIX GZip)                       |                         | File ID only                 |
| TZ (UNIX Compress)                    |                         | File ID only                 |
| Uuencode                              |                         |                              |
| VPK (.ZIP/JAR File)                   |                         | File ID only                 |
| WHL zip Archive                       |                         | File ID only                 |
| WUX (Compressed Wii U Disk Image WUD) |                         | File ID only                 |
| XAPK (.ZIP/JAR File)                  |                         | File ID only                 |
| XAR Archive Compressed                |                         | File ID only                 |
| XPI (.ZIP/JAR File)                   |                         | File ID only                 |
| XZ file                               |                         | File ID only                 |
| ZIP PKZip, WinZip, ZIP64              |                         |                              |
| ZIPX (.ZIP/JAR File)                  |                         | File ID only                 |
| Z file (UNIX GZip)                    |                         | File ID only                 |
| ZIM Wiki archive                      |                         | File ID only                 |
| Zlip compressed data                  |                         | File ID only                 |
| ZST file                              |                         | File ID only                 |
| SFG (.ZIP/JAR File)                   |                         | File ID only                 |
| SIFZ (UNIX GZip)                      |                         | File ID only                 |
| OAR (UNIX GZip)                       |                         | File ID only                 |
| MPKG (XAR Archive Compressed)         |                         | File ID only                 |
| FZPZ (.ZIP/JAR File)                  |                         | File ID only                 |
| PET (UNIX GZip)                       |                         | File ID only                 |
| PUP (.ZIP/JAR File)                   |                         | File ID only                 |

## Database Formats

| Format                                   | Version                        | Notes        |
| ---------------------------------------- | ------------------------------ | ------------ |
| DataEase                                 | 4.x                            |              |
| DBase                                    | III, IV, V, X, X1              |              |
| First Choice DB                          | Through 3.0                    |              |
| Framework DB                             | 3                              |              |
| Microsoft Access (text only)             | 1.0, 2.0, 95 … 2019, 2007–2016 |              |
| Microsoft Access Report Snapshot         | 2000 – 2003                    | File ID only |
| Microsoft Works DB for DOS               | 1, 2                           |              |
| Microsoft Works DB for Macintosh         | 2                              |              |
| Microsoft Works DB for Windows           | 3.0, 4.0                       |              |
| Microsoft Universal Data Link file       |                                | File ID only |
| Windows Works DB                         |                                | File ID only |
| Lotus Data Interchange Format            |                                | File ID only |
| Master Database File / Log Database File | MDF / LDF                      | File ID only |
| MySQL MyISAM Index file                  | MYI                            | File ID only |
| Lotus Notes Database or Template         | NSF / NTF                      |              |
| SAV data file                            |                                | File ID only |
| TRC data file                            |                                | File ID only |
| Paradox for DOS                          | 2.0 – 4.0                      |              |
| Paradox for Windows                      | 1                              |              |
| SQLite data file                         | DB3                            | File ID only |
| CAN Database DBC File                    |                                | File ID only |
| Q&A Database                             | Through 2.0                    |              |
| R:Base 5000                              |                                |              |
| R:Base System V                          |                                |              |
| 4D Database                              | 4DB & 4DD                      | File ID only |
| SmartWare II DB                          | 1.02                           |              |
| Reflex                                   | 2                              |              |
| Microsoft Access Template File           | 2007, 2010, 2013               |              |

## Raster Formats

| Format                             | Version                                 | Notes        |
| ---------------------------------- | --------------------------------------- | ------------ |
| Adobe Photoshop                    | 4                                       |              |
| Adobe Photoshop PSD                |                                         | File ID only |
| Adobe Photoshop CS                 | 1–6, CC 2014–2018                       |              |
| CALS Raster (GP4)                  | Type I, Type II                         |              |
| Computer Graphics Metafile         | ANSI, CALS, NIST                        |              |
| Encapsulated PostScript (EPS)      |                                         |              |
| GEM Image (Bitmap)                 |                                         |              |
| Graphics Interchange Format (GIF)  |                                         |              |
| IBM Graphics Data Format (GDF)     |                                         |              |
| IBM Picture Interchange Format     |                                         |              |
| JBIG2 Graphics (embeddings in PDF) |                                         |              |
| JFIF (JPEG not in TIFF format)     |                                         |              |
| JPEG                               |                                         |              |
| JPEG 2000 JP2                      |                                         |              |
| Kodak Flash Pix                    |                                         |              |
| Kodak Photo CD                     | 1                                       |              |
| Lotus PIC                          |                                         |              |
| Lotus Snapshot                     | BMP Only                                |              |
| Macintosh PICT                     | BMP Only, PICT2                         |              |
| MacPaint                           |                                         |              |
| Paint Shop Pro (Win32 only)        | 5.0, 6.0                                |              |
| PC Paintbrush (PCX)                |                                         |              |
| PC Paintbrush DCX                  | Multi-page PCX                          |              |
| Portable Bitmap                    | PBM                                     |              |
| Portable Graymap                   | PGM                                     |              |
| Portable Network Graphics          | PNG                                     |              |
| Portable Pixmap                    | PPM                                     |              |
| Portable Arbitrary Map             | PAM                                     |              |
| Progressive JPEG                   |                                         |              |
| StarOffice Draw                    | 6.x–9.0                                 |              |
| Sun Raster                         |                                         |              |
| TIFF                               | Header only, Group 5 & 6, CCITT G3 & G4 |              |
| TruVision TGA (Targa)              | 2                                       |              |
| WebP                               |                                         | 1.4.0        |
| WBMP                               | Wireless graphics format                |              |
| Perfect Graphics                   |                                         |              |
| X-Windows Bitmap                   | x10 compatible                          |              |
| X-Windows Dump                     | x10 compatible                          |              |
| X-Windows Pixmap                   | x10 compatible                          |              |
| WordPerfect Graphics               | 2.0 – 10.0                              |              |
| JT Image                           | 8.0, 9.0, 10.0                          | File ID only |
| Microsoft Windows Bitmap           |                                         |              |
| Microsoft Windows Cursor           |                                         |              |
| Microsoft Windows Icon             |                                         |              |
| OS/2 Bitmap                        |                                         |              |
| OS/2 Warp Bitmap                   |                                         |              |

## Email Formats

| Format                                  | Version                | Notes                                                                         |
| --------------------------------------- | ---------------------- | ----------------------------------------------------------------------------- |
| Microsoft Outlook (MSG)                 | 97 – 2019              |                                                                               |
| Microsoft Outlook Express (EML)         | 97 – 2019              |                                                                               |
| Microsoft Outlook Forms Template (OFT)  |                        |                                                                               |
| Microsoft Outlook OLM                   | 2011 for Mac           | File ID only                                                                  |
| Microsoft Outlook OST                   | 97 – 2019              |                                                                               |
| Microsoft Outlook PST                   | 97 – 2019              |                                                                               |
| Microsoft Outlook PST (Mac)             | 2001                   |                                                                               |
| MSG with Digital Signature              | SMIME                  |                                                                               |
| EML with Digital Signature              | SMIME                  |                                                                               |
| Apple Mail Message                      |                        | File ID only                                                                  |
| IBM Lotus Notes Domino XML Language DXL | 8.5                    |                                                                               |
| IBM Lotus Notes NSF                     | 7.x, 8.x               | File ID only                                                                  |
| IBM Lotus Notes NSF                     | 8.x                    | Win32, Win64, Linux x86-32, Solaris8 (Notes Client or Domino Server required) |
| MBox Mailbox                            | RFC 822                | File ID only                                                                  |
| Encoded mail messages                   | MHT                    |                                                                               |
| Encoded mail messages                   | Multi Part Alternative |                                                                               |
| Encoded mail messages                   | Multi Part Digest      |                                                                               |
| Encoded mail messages                   | Multi Part Mixed       |                                                                               |
| Encoded mail messages                   | Multi Part News Group  |                                                                               |
| Encoded mail messages                   | Multi Part Signed      |                                                                               |
| Encoded mail messages                   | TNEF                   |                                                                               |

## Multimedia Formats

| Format                       | Version        | Notes                                          |
| ---------------------------- | -------------- | ---------------------------------------------- |
| AVI                          |                | Metadata only                                  |
| DICOM                        |                | File ID only                                   |
| Flash (text extraction only) | 6.x, 7.x, Lite |                                                |
| Flash                        | 9, 10          | File ID only                                   |
| Real Media                   |                | File ID only                                   |
| MP3                          |                | ID3 metadata only                              |
| MPEG-1 Audio Layer 3         | ID3 v1         | Metadata only                                  |
| MPEG-1 Audio Layer 3         | ID3 v2         | Metadata only                                  |
| MPEG-1 Video                 | V 2            | File ID only                                   |
| MPEG-1 Video                 | V 3            | File ID only                                   |
| MPEG-2 Audio                 |                | File ID only                                   |
| MPEG-4                       |                | Metadata only                                  |
| MPEG-7                       |                | Metadata only                                  |
| QuickTime                    |                | Metadata only                                  |
| Windows Media ASF            |                | Metadata only                                  |
| Windows Media DVR-MS         |                | Metadata only                                  |
| Windows Media Audio WMA      |                | Metadata only                                  |
| Windows Media Playlist       |                | File ID only                                   |
| Windows Media Video WMV      |                | Metadata only                                  |
| WAV                          |                | Metadata only                                  |
| Apple HEIF                   |                | Supported on LinuxX64, WindowsX64, Linux ARM64 |
| WebM                         |                | File ID only                                   |
| OGPUS open container format  |                | File ID only                                   |

## Text & Markup Formats

| Format                                 | Version                              | Notes        |
| -------------------------------------- | ------------------------------------ | ------------ |
| ANSI Text                              | 7 & 8-bit                            |              |
| ASCII Text                             | 7 & 8-bit                            |              |
| Ami Pro for OS2                        |                                      |              |
| Ami Pro for Windows                    | 2.0, 3.0                             |              |
| Apple iWork Pages                      | 2014, 2020                           | File ID only |
| DEC DX                                 | Through 4.0                          |              |
| DEC DX Plus                            | 4.0, 4.1                             |              |
| Enable Word Processor                  | 3.0 – 4.5                            |              |
| First Choice WP                        | 1.0, 3.0                             |              |
| Framework WP                           | 3                                    |              |
| Hangul                                 | 97 – 2010                            |              |
| IBM DCA/FFT                            |                                      |              |
| IBM DCA/RFT                            |                                      |              |
| IBM DisplayWrite                       | 2.0 – 5.0                            |              |
| IBM Writing Assistant                  | 1.01                                 |              |
| Ichitaro                               | 5.0, 6.0, 8.0–13.0, 2004, 2010, 2013 |              |
| JustWrite                              | Through 3.0                          |              |
| Kingsoft WPS Writer                    | 2010                                 |              |
| Legacy                                 | 1.1                                  |              |
| LibreOffice Writer                     | 4.x                                  |              |
| Lotus Manuscript                       | Through 2.0                          |              |
| Lotus WordPro                          | 9.7, 96 – Millennium 9.8             | Text only    |
| MacWrite II                            | 1.1                                  |              |
| Mass                                   | 11 through 8.0                       |              |
| Microsoft Publisher                    | 2003 – 2016                          | File ID only |
| Microsoft Word for DOS                 | 4.0 – 6.0                            |              |
| Microsoft Word for Macintosh           | 4.0 – 6.0, 98 – 2011                 |              |
| Microsoft Word for Windows             | 1.0 – 2016, 2019, MS365              |              |
| Microsoft Word for Windows (text only) | 2003 XML                             |              |
| Rich Text Format (RTF)                 |                                      |              |
| Unicode Text                           | 3.0, 4.0                             |              |
| UTF-8                                  |                                      |              |
| Wireless Markup Language               |                                      |              |
| XHTML                                  |                                      | File ID only |
| XML                                    |                                      | Text only    |
| Extensible Markup Language             |                                      |              |
| Internet HTML                          |                                      |              |

## Vector Image Formats

| Format                               | Version                  | Notes                          |
| ------------------------------------ | ------------------------ | ------------------------------ |
| Adobe FrameMaker (MIF only)          | 3.0 – 6.0                |                                |
| Adobe Illustrator Postscript         | Level 2                  |                                |
| Adobe Illustrator                    | 4.0 – 7.0                |                                |
| Adobe Illustrator (PDF Preview only) | 9.0, CS1 – 6             |                                |
| Adobe Illustrator XMP                | CS1 – 6                  |                                |
| Adobe InDesign XMP                   | CS1 – 6                  |                                |
| Adobe InDesign Interchange XMP only  |                          |                                |
| Adobe PDF                            | 1.0 – 1.7 (Acrobat 1–10) |                                |
| Adobe PDF Package                    | 1.7 (Acrobat 8–10)       |                                |
| Adobe PDF Portfolio                  | 1.7 (Acrobat 8–10)       |                                |
| Ami Draw SDW                         |                          |                                |
| AutoCAD Drawing                      | 2.5, 2.6                 |                                |
| AutoCAD Drawing                      | 9.0 – 14.0               |                                |
| AutoCAD Drawing                      | 2000i – 2015, 2016–2021  |                                |
| AutoShade Rendering                  | 2                        | File ID only                   |
| Corel Draw                           | 2.0 – 9.0, X3 – X7       |                                |
| Corel Draw Clipart                   | 5.0, 7.0                 |                                |
| Enhanced Metafile (EMF)              |                          |                                |
| Escher Graphics                      |                          |                                |
| FrameMaker Graphics (FMV)            | 3.0 – 5.0                |                                |
| GEM File (Vector)                    |                          |                                |
| Harvard Graphics Chart DOS           | 2.0 – 3.0                |                                |
| Harvard Graphics for Windows         |                          |                                |
| HP Graphics Language                 | 2                        |                                |
| IGES Drawing                         | 5.1 – 5.3                |                                |
| Micrografx Designer                  | Through 3.1, 6           |                                |
| Micrografx Draw                      | Through 4.0              |                                |
| Microsoft XPS                        |                          | Text only                      |
| Novell PerfectWorks Draw             | 2                        |                                |
| OpenOffice Draw                      | 1.1 – 3.0                |                                |
| Oracle Open Office Draw              | 3.x                      |                                |
| SVG                                  |                          | Processed as XML, not rendered |
| Visio (Page Preview mode WMF/EMF)    | 4                        |                                |
| Visio                                | 5.0 – 2010               |                                |
| Visio (text only)                    | 2013                     |                                |
| Visio XML VSX                        | 2007                     | File ID only                   |
| Windows Metafile                     |                          |                                |

## Word Processing Formats

| Format                              | Version                | Notes        |
| ----------------------------------- | ---------------------- | ------------ |
| Microsoft Word for Windows          | 98-J, MS365            |              |
| Microsoft WordPad                   |                        |              |
| Microsoft Works WP for DOS          | 2                      |              |
| Microsoft Works WP for Macintosh    | 2                      |              |
| Microsoft Works WP for Windows      | 3.0, 4.0               |              |
| Microsoft Write for Windows         | 1.0 – 3.0              |              |
| MultiMate                           | Through 4.0            |              |
| MultiMate Advantage                 | 2                      |              |
| Navy DIF                            |                        |              |
| Nota Bene                           | 3                      |              |
| Novell PerfectWorks Word Processor  | 2                      |              |
| OfficeWriter                        | 4.0 – 6.0              |              |
| OpenOffice Writer                   | 1.1 – 3.0              |              |
| Oracle Open Office Writer           | 3.x                    |              |
| PC File Doc                         | 5                      |              |
| LibreOffice Writer                  | 3.x (ODF 1.1, 1.2)     | File ID only |
| Microsoft Word Recovery file        |                        | File ID only |
| Wordstar for DOS                    | 3.0 – 7.0              |              |
| Wordstar for Windows                | 1                      |              |
| Wordstar 2000 for DOS               | 1.0 – 3.0              |              |
| Wordstar 2000 for DOS               | 2.0, 3.0               |              |
| XyWrite                             | Through III+           |              |
| PFS: Write                          | A, B                   |              |
| Professional Write for DOS          | 1.0, 2.0               |              |
| Professional Write Plus for Windows | 1                      |              |
| Q&A Write                           | 2.0, 3.0               |              |
| Samna Word IV                       | 1.0 – 3.0              |              |
| Samna Word IV+                      |                        |              |
| Samsung JungUm Global               |                        | File ID only |
| Signature                           | 1                      |              |
| SmartWare II WP                     | 1.02                   |              |
| Sprint                              | 1                      |              |
| StarOffice Writer                   | 5.2 – 9.0              |              |
| Strict Open XML – Document          | 2013, 2016, 2019       | File ID only |
| Total Word                          | 1.2                    |              |
| Wang IWP                            | Through 2.6            |              |
| WordMarc Composer                   |                        |              |
| WordMarc Composer+                  |                        |              |
| WordMarc Word Processor             | X3 – X7                |              |
| WordPerfect for DOS                 | 4.2                    |              |
| WordPerfect for Macintosh           | 1.02 – 3.1, 6.1 – 12.0 |              |
| WordPerfect for Windows             | 5.1 – X7               |              |

## Spreadsheet Formats

| Format                                         | Version           | Notes        |
| ---------------------------------------------- | ----------------- | ------------ |
| Lotus 1-2-3 for OS/2                           | 2                 |              |
| Microsoft Excel Charts                         | 2.x – 2007, MS365 |              |
| Microsoft Excel for Macintosh                  | 98 – 2011         |              |
| Microsoft Excel for Windows                    | 3.0 – 2019, MS365 |              |
| Microsoft Excel for Windows (text only)        | 2003 XML, MS365   |              |
| Microsoft Excel for Windows (.xlsb)            | 2007 – 2019       | Binary       |
| Microsoft Works SS for DOS                     | 2                 |              |
| Microsoft Works SS for Macintosh               | 2                 |              |
| Microsoft Works SS for Windows                 | 3.0, 4.0          |              |
| Multiplan                                      | 4                 |              |
| Novell PerfectWorks Spreadsheet                | 2                 |              |
| OpenOffice Calc                                | 1.1 – 3.0         |              |
| Oracle Open Office Calc                        | 3.x               |              |
| PFS: Plan                                      | 1                 |              |
| QuattroPro for DOS                             | Through 5.0       |              |
| QuattroPro for Windows                         | Through X7        |              |
| SmartWare Spreadsheet                          |                   |              |
| SmartWare II SS                                | 1.02              |              |
| Symphony                                       | Through 2.0       |              |
| VP-Planner                                     | 1                 |              |
| FileMakerPro (FMP12) Spreadsheet               |                   | File ID only |
| GNUMERIC Spreadsheet                           |                   | File ID only |
| Comma-separated values (CSV) UTF-8 Spreadsheet |                   | File ID only |
| Everything File List (EFU) Spreadsheet         |                   | File ID only |
| Ability Spreadsheet Template (AST)             |                   | File ID only |
| Transport-Neutral Encapsulation Format (TNEF)  |                   | File ID only |
| TimeMap timeline (TMVT/TMV) Spreadsheet        |                   | File ID only |
| Ability Spreadsheet (AST/AWS)                  |                   | File ID only |
| File Maker (FP7) Spreadsheet                   |                   | File ID only |
| TimeMap timeline (TMVT) template               |                   | File ID only |
| PMDX (PlanMaker) Spreadsheet                   |                   | File ID only |
| IMP (Lotus Improv) Spreadsheet                 |                   | File ID only |
| CELL Workbook file                             | 2010              | File ID only |
| XML Forms Data Format (XFDF)                   |                   |              |
| StarOffice Calc                                | 5.2 – 9.0         |              |
| Strict Open XML – Spreadsheet                  | 2013, 2016, 2019  | File ID only |
| SuperCalc                                      | 5                 |              |

## Presentation Formats

| Format                                          | Version                 | Notes        |
| ----------------------------------------------- | ----------------------- | ------------ |
| Apple iWork Keynote                             | 2014, 2020              | File ID only |
| Harvard Graphics Presentation (DOS)             | 3                       |              |
| IBM Lotus Symphony Presentations                | 1.x                     |              |
| Kingsoft WPS Presentation                       | 2010                    |              |
| LibreOffice Impress                             | 3.x, 4.x, 5.x, 6.x      |              |
| Lotus Freelance                                 | 1.0 – Millennium 9.8    |              |
| Lotus Freelance for OS/3                        | 2                       |              |
| Lotus Freelance for Windows                     | 95, 97, SmartSuite 9.8  |              |
| Microsoft PowerPoint for Macintosh              | 4.0 – 2016, 2019        |              |
| Microsoft PowerPoint for Windows                | 3.0 – 2016, 2019, MS365 |              |
| Microsoft PowerPoint for Windows Slideshow      | 2007 – 2019, MS365      |              |
| Microsoft PowerPoint Macro Enabled Slideshow    | 2016/2019, MS365        |              |
| Microsoft PowerPoint Macro Enabled Presentation | 2016/2019, MS365        |              |
| Microsoft PowerPoint Macro Enabled Template     | 2010/2011, MS365        |              |
| Microsoft PowerPoint Slideshow                  | 2016/2019, MS365        |              |
| Microsoft PowerPoint Template                   | 2013, MS365             |              |
| Novell Presentations                            | 3.0, 7.0                |              |
| OpenOffice Impress                              | 1.1, 3.0, 4.x           |              |
| Oracle Open Office Impress                      | 3.x                     |              |
| StarOffice Impress                              | 5.2 – 9.0               |              |
| Strict Open XML – Presentation                  | 2013, 2019              | File ID only |
| WordPerfect Presentations                       | 5.1 – X7                | File ID only |
| Advanced Function Presentation (AFP)            |                         | File ID only |

## Other Formats

| Format                                         | Version      | Notes        |
| ---------------------------------------------- | ------------ | ------------ |
| MOBI                                           |              | File ID only |
| TimeMap timeline (TMVT) template file          |              | File ID only |
| IMP (Lotus Improv) Spreadsheet                 |              | File ID only |
| AOL Messenger                                  | 7.3          | File ID only |
| Wolfram notebook (nb) file format              |              | File ID only |
| CELL Workbook file                             |              | File ID only |
| CELL 2010 Workbook file                        |              | File ID only |
| File Maker (FP7) Spreadsheet                   |              | File ID only |
| GNUMERIC Spreadsheet                           |              | File ID only |
| Comma-separated values (CSV) UTF-8 Spreadsheet |              | File ID only |
| Everything File List (EFU) Spreadsheet         |              | File ID only |
| Free Lossless Image Format                     |              | File ID only |
| SketchUp File Format                           |              | File ID only |
| Multiple-image Network Graphics Format         |              | File ID only |
| Gerber Image File (PCB) Format                 |              | File ID only |
| DJVU Compressed Image File                     |              | File ID only |
| JPEG 2000 Compound File (JPM)                  |              | File ID only |
| Apple Icon Image                               |              | File ID only |
| Windows Cursor                                 |              | File ID only |
| PUB                                            |              | File ID only |
| PUZ                                            |              | File ID only |
| MPX                                            |              | File ID only |
| XER                                            |              | File ID only |
| Printer Command Language                       | PCL 5e, PCL6 | File ID only |
| Microsoft InfoPath                             | 2007         | File ID only |
| Microsoft Live Messenger                       | 10           | File ID only |
| Microsoft Office Theme files                   | 2007–2019    | File ID only |
| Microsoft OneNote (text only)                  | 2007–2019    | File ID only |
| Microsoft Project (table view only)            | 98–2010      | File ID only |
| Microsoft Windows Compiled Help                | .chm         | File ID only |
| Microsoft Windows DLL                          | .dll         | File ID only |
| Microsoft Windows Executable                   | .exe, .com   | File ID only |
| Microsoft Windows Explorer Command             | .scf         | File ID only |
| Microsoft Windows Help                         | .hlp         | File ID only |
| Microsoft Windows Shortcut                     | .lnk         | File ID only |
| Trillian Text Log File                         | 4.2          | File ID only |
| Trillian XML Log File                          | 4.2          | File ID only |
| TrueType Font                                  | .ttf, .ttc   | File ID only |
| vCalendar                                      | 2.1          |              |
| vCard                                          | 2.1          |              |
| Yahoo! Messenger                               | 6.x – 8      |              |
| P6XML                                          |              | File ID only |

## E-Book Formats

| Format | Version | Notes        |
| ------ | ------- | ------------ |
| EPUB   |         | File ID only |
