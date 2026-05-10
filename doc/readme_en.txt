1. About the Program
The uLister internal viewer plugin supports over 500 different file formats.

2. Supported Formats
Many popular word processor, spreadsheet, presentation, raster, and vector image formats.
A complete list can be found in OutsideIn_8.5.7_Supported_File_Formats.pdf

3. License
The plugin is provided as-is and without any warranty.
Although the plugin is freeware, it uses third-party Oracle libraries. Please read the license before using it.
https://www.oracle.com/downloads/licenses/standard-license.html

Outside In Viewer (C) 1991, 2026 Oracle(R)
(C) 2011 Egor Vlaznev
(C) 2016 Michael Loster (aka milo1012)
(C) 2024 Marek Jasinski (aka Marek)
(C) 2024 Vogelbacher Andreas (aka avogelba)
(C) 2026 evgen_b (aka evgen_b)

4. Libraries Used
Oracle(R) Outside In Technology: Viewer Technology.

5. Configuration
The settings are stored in the ulister.ini file, which is searched in the following order:
1) plugin directory
2) directory containing %COMMANDER_INI%
3) %APPDATA%
The file will not be loaded if the path exceeds 260 characters (Windows limit).

The name of the settings section is [ulister].

The following parameters are available in version 4.0.0.4 of the plugin:

keepinmemory        (default 1) - don't unload Outside In Viewer libraries from memory after first use, which reduces
                    document loading time but increases memory consumption.
noloadtypes         list of formats excluded from loading
onlyloadtypes       list of formats allowed to loading
nopreviewtypes      list of format thumbnails excluded from displaying
onlypreviewtypes    list of format thumbnails allowed to displaying

Four-digit format codes with descriptions are located in the formats.txt file.
Any non-numeric character, such as a comma or space, can be used as a separator.
If an allow string is specified, the corresponding deny string is ignored.

optionsdir          path to the directory in which the ".oit" subdirectory is created,
                    containing the internal settings of the Oracle Outside In Technology: Viewer Technology library;
                    environment %variables% are supported;
                    Windows limitation - path length is 260 characters maximum;
                    For optionsdir to work correctly, you must use a patched version of the sccut.dll file.

Some patched versions of this file are located in the "OIT_DATA_PATH support" directory (for example, the latest versions supporting Windows XP).

If the optionsdir parameter is not specified, the library creates ".oit" in %APPDATA%; if this value is undefined, it creates it in %LOCALAPPDATA%;
if this value is also undefined, it creates it in the library directory.

Version 4.0.0.7 of the plugin introduces additional parameters related to Outside In Technology library copying to the clipboard.
These parameters should be located in the [clipboard] section and are set to "SKIP" by default, meaning they use the settings stored
in Outside In Technology (".oit" directory) rather than the INI file.

The main group is what can be accessed by right-clicking Options->Clipboard in an open document.
These parameters determine the format in which blocks can be placed on the clipboard (SDK A.10.10 SCCID_TOCLIPBOARD).

[clipboard]
ascii=skip|on|off
rtf=skip|on|off
unicode=skip|on|off (by default, this option is disabled, which in the original led to the appearance of "kryakozyabr" when copying national characters)
bitmap=skip|on|off
windib=skip|on|off
metafile=skip|on|off
palette=skip|on|off

The following setting enables or disables drag-and-drop copying of a selected block (SDK A.10.6 SCCID_OLEFLAGS).
dragdrop=skip|on|off

It is recommended to set all clipboard section parameters to "on" (especially for unicode).

This settings specifies the format for copying cells from spreadsheets (SDK A.4.8 SCCID_SSCLIPBOARD)
or from database (SDK A.4.1 SCCID_DBCLIPBOARD)
spreadsheet=skip|rtf|tabs|optimizedtabs
database=skip|rtf|tabs|optimizedtabs
    rtf - copy as a table using RTF format
    tabs - as plain text, separated between cells by tabs
    optimizedtabs - as in the previous case, but empty cells are skipped

This setting in the [viewer] section indicates how the word processor/HTML/email display engine displays documents since 4.0.1.0
(A.7.2 SCCID_WPDISPLAYMODE / SCCID_HTMLDISPLAYMODE / SCCID_EMAILDISPLAYMODE)

wpdisplaymode=skip|draft|normal|preview|weblayout
htmldisplaymode=skip|draft|normal|preview|weblayout
emaildisplaymode=skip|draft|normal|preview|weblayout
    draft - Display using only a single font and size (SCCID_DEFAULTDISPLAYFONT), do not display embedded graphics, do not display graphic or table borders, wrap the text to the size of the view window.
    normal - Display all supported formatting, wrap the text to the size of the view window.
    preview - Display all supported formatting, wrap the text as it will be printed.
    weblayout - Display all supported formatting, wrap the text as it would appear in a browser.

This settings in the [viewer] section controls the size of word processor pages when using preview or weblayout mode
(A.7.3 SCCID_WPFITMODE / SCCID_HTMLFITMODE / SCCID_EMAILFITMODE)
These settings are applied correctly only from the second launch of ulister after
changes in the .ini file due to an bug in the Outside In Viewer library (workaround).

webprevwpfitmode=skip|original|width|window
webprevhtmlfitmode=skip|original|width|window
webprevemailfitmode=skip|original|width|window
    original - sizes the preview page to the actual size
    width - sizes the preview page to the width of the window
    window - sizes the preview page to the window; sizes the weblayout to the width of the window

Options are set the first time you launch the plugin. Restart Total Commander for the changes to take effect.

6. Installation
The Oracle Outside In Technology: Viewer Technology library can be downloaded from
https://www.oracle.com/middleware/technologies/outside-in-technology-downloads.html

Install the uLister plugin.

Download oit-8.5.8.204-vw-win-x64.zip for the 64-bit version of Total Commander and oit-8.5.7.243-vw-win-x32.zip for the 32-bit version. Unzip the files.
(If you need support for Windows XP x64 SP2 or Windows XP x86 SP3, use the older versions oit-8.5.5.12-vw-win-x86-64.zip and oit-8.5.5.12-vw-win-x86-32.zip.)

If you unpacked the 64-bit library, copy the 64-bit files from the unpacked "redist" directory to the plugin's redist64 directory.
If you unpacked the 32-bit library, copy the 32-bit files from the unpacked "redist" directory to the plugin's redist32 directory.

If you plan to use the plugin's optionsdir parameter, replace the sccut.dll file with the corresponding patched version from the "OIT_DATA_PATH support" directory.

To run Oracle Outside In Technology: Viewer Technology, you need current versions of the Visual C++ Redistributables libraries.
AIO can be installed, for example, from here: https://github.com/abbodi1406/vcredist

Starting with version 4.0.0.6, uLister selects libraries even more flexibly:
Now, only for operating systems below Windows 7, a search for Outside In libraries has been added, first in the XPdist32 or XPdist64 directories (higher priority),
and if nothing found, then in redist32 or redist64 as usual (lower priority). For Windows 7 and higher, the search is performed as before, only in redist32 or redist64.
This is done so that uLister can flexibly select the appropriate Outside In libraries depending on the OS it is running on
(legacy libraries but with XP/Vista support, and the newest libraries for other operating systems starting with Windows 7 and higher).
Therefore, if automatic XP/Vista support using older library versions is not needed, then the XPdist32 and XPdist64 directories containing
older library versions are also unnecessary, and everything will work as before on any OS without them.

7. Keyboard Shortcuts
Enter a search string - Ctrl+F/F7
Find Next/Previous - F3/Shift+F3
(Search is not available in some formats; case-insensitive search option is supported)
For some formats, mainly tabular and vector ones, you can zoom in/out - Ctrl "+" / Ctrl "-"
For some formats, you can select a block and copy it to the clipboard, or drag-and-drop copy.

8. Compilation.
Use the vs2015.sln file to edit the project's source code in modern versions of Visual Studio.
Use the vs2005.sln file to compile the project into compact code with Windows XP 32/64 support using
Visual Studio 2005 (for x64 compilation, you will need the Pro version).
Then use MAKECAB.CMD script to create an automatic installation file plugin wlx-ulister.cab for Total Commander.

9. History
Ancient version history

1.0.0 (20.01.2011)
Initial release 

1.0.1 (21.01.2011)
All dll now load dynamically, older Total Commander version and another programs that use wlx plugin now supported. 
Added attachments support for Total Commander 7.50 and higher 

1.0.2 (21.01.2011)
Attachments support now really work for Total Commander 7.50 and higher. 
Automatic saving option on exit. 

1.0.3 (21.01.2011)
Fixed serious bug introduced in 1.0.2 version 
New option in oilister.ini allow keep Outside In Viewer library in memory, it reduce document load time but increase memory consumption. 

1.0.4 (22.01.2011)
Fixed vertical scroll. 
Added horizontal scroll, if your mouse support it. 
Added zoom hotkey: Ctrl +/- or mouse scroll 
Fixed search, Shift F3 now works. 

1.1.0 (29.01.2011)
Fixed critical bug: vector image loading now working stable. 
Fixed html navigation. 
Added ListGetPreviewBitmap,ListGetPreviewBitmapW 

1.2.0 (03.02.2011)
Added content plugin function for search files contain the specified text. 

1.2.1 (03.02.2011)
Ñontent plugin speed dramatically increased. 

1.3.0 (09.02.2011)
Added conversion feature, you can convert files to txt,pdf,html,mhtml. 

1.4.0 (17.02.2011)
Fixed many bugs in conversion routine(Unicode + Save file dialog). 
Fixed dynamic library loading function (Now unicode). 
Added conversion to jpeg 2000,jpeg,tiff,png,gif,bmp. 
Added position saving feature. 
Ini parser now accept only UTF-8 file and can work with unicode filenames.

2.0.0 (02.03.2011)
Added toolbar 
Added context menu items 
Fixed display unicode file name in lister window title 
Fixed ini parser

2.5.0 (17.03.2011)
Added options dialog
Fixed many bugs 

2.6.0 (18.03.2011)
Added wcx compatiblity for Document Converter, now it can convert many file at once 
Fixed bugs 

3.0.0 (29.03.2011)
Added tabs 
Added bookmarks browser 
Added converters options 
Added filetypes filter in options dialog 
Fixed many bugs 

3.0.1 (31.03.2011)
Tabs removed 

3.0.2 (8.04.2011)
Quick view mode fix 
Window resize fix

4.0.0 (September 20, 2011)
Removed all untrusted code, keep only the basic file viewing functions.
Added x64 support.

Recovering modern history after version 4.0.0.0 (by source code comparison)

2016-06-01 4.0.0.1 (Fork by Michael Loster (aka milo1012), author of plugins _RegXtract.wcx_ and _PCREsearch.wdx_.)
Changed external search for Oracle libraries:
Previously: First, search for files in the plugin directory, then in the redist subdirectory
Now: No longer search for files in the plugin directory; searching for 32-bit libraries only in the redist32 subdirectory and 64-bit libraries only in the redist64 subdirectory.

2024-06-18 4.0.0.2 (Fork by Marek Jasinski (aka Marek))
https://freecommander.com/forum/viewtopic.php?p=42993 
Added Delphi 12 fix to support FreeCommander 909 and higher.
Changes to the external Oracle library search again,
search steps:
1. Initially, as in the previous version, there were 32-bit libraries in the redist32 subdirectory and 64-bit libraries in the redist64 subdirectory
(then compatibility with versions up to and including 4.0.0.0 was added, i.e.)
2. If they aren't there, then search in the plugin directory
3. If they aren't there either, then search in the "redist" subdirectory
(This method was suitable for the transitional version at the time, but is no longer relevant)

2024-10-09 4.0.0.3 (Fork by avogelba)
https://github.com/avogelba/uLister
(!) This version inherits from 4.0.0.1 and doesn't contain any fixes from version 4.0.0.2.
(!) It was created to build in Visual Studio 2022, and PlatformToolset v143 does not contain code for Windows XP.
To compile the project, you must add the Oracle and Total Commander SDKs.
(!) This project is configured incorrectly for generating 32-bit code.
Added original files from Egor Vlaznev with an (outdated) plugin description (html), etc.
As it turns out, unused macros still remained in the project properties, making the code very difficult to understand.
Added:
    - In the new version of VS2022, using C legacy functions leads to compilation errors, so bypass directives have been added
    - Minor fix to the search functionality to align with the modern Oracle API
Therefore, this code contains the maximum number of regressions (with an exclamation mark).



2026-03-14 4.0.0.4 (Fork by evgen_b)
It so happens that the files from version 4.0.0.3 https://github.com/avogelba/uLister were used as a basis

Plugin initialization has been completely rewritten
    - Buffer expansion error when parsing ulister.ini have been fixed
    - Clear steps for searching for the ulister.ini file: first in the plugin directory; if it's not there, then in the Wincmd.ini directory;
        if it's not there either, then in %APPDATA% (the original algorithm did not match the description, and the search priority was reverse direction)
    - The path to ulister.ini will also be ignored if it contains more than 260 characters (Windows limitation)
    - Detailed, human-readable error messages to make it clear what is being loaded and from where
    - Fixed a regression in 4.0.0.3 - it was impossible to build a correct 32-bit file
    - Added vs2005 template for generating compact 32-bit code
    - Added modified the sccut.dll version from oit-8.5.8.204-vw-win-x64.zip to dynamically obtain the OIT_DATA_PATH parameter at runtime
    - Removed dead source file

2026-03-15
    - Added modified the sccut.dll version from oit-8.5.7.243-vw-win-x32.zip (latest 32-bit) to dynamically obtain the OIT_DATA_PATH parameter at runtime

2026-03-16
    - Added modified the sccut.dll version from oit-8.5.5.12-vw-win-x86-32.zip for WinXP SP3 x86
    and version from oit-8.5.5.12-vw-win-x86-64.zip for SP2 x64 to dynamically obtain the OIT_DATA_PATH parameter at runtime

2026-03-18
    - Now compiles in both x86 and x64 targets for VS2005 to reach most compact code;
    - Preprocessor macros have been brought into uniformity; unused macros removed from project properties
    - Regression fix in 4.0.0.3: Returned compatibility with Delphi 12 to support FreeCommander 909 and higher (from the 4.0.0.2 branch)

2026-03-19
    - Added a template for VS2015.
Use the vs2015.sln file to edit project source code in modern versions of Visual Studio.
Use the vs2005.sln file to compile the project to reach most compact code with Windows XP 32/64 support using Visual Studio 2005.
    - A readme_ru.txt file has been created describing the current version of the plugin.

2026-03-20
    - Added the CLEAN.CMD script to clean the project of Visual Studio activity.
    - Fixed a typo in the sccut.dll patch for oit-8.5.5.12-vw-win-x86-32.zip - incorrect handling of OIT_DATA_PATH longer than 260 characters
    - For very old versions of Windows XP, likely SP0, the following sccut.dll patches have been added:
        oit-8.3.5-vw-win-x86-32.zip
        oit-8.4.1-vw-win-x86-32.zip
        oit-8.4.1-vw-win-x86-64.zip
    - Added the pluginst.inf and ulister.ini config examples

2026-03-21
    - Added the MAKECAB.CMD script to create an automatic installation file plugin wlx-ulister.cab for Total Commander.
    - A readme_en.txt file has been created 

2026-03-22
    https://github.com/evgen0xb/uLister

2026-03-31 4.0.0.5 (Fork by evgen_b)
    - Implemented support for UTF16 search using Oracle's built-in tools https://www.ghisler.ch/board/viewtopic.php?p=459162#p459162
      (but it seems that the string is converted back to ANSI inside Outside In Viewer)
    - Fixed a potential overflow when calculating the search string length (if the input string is longer than 79 characters - an Oracle limitation)
    - If the search fails, the actual search string is displayed (always truncated to 79 characters), not what was entered in the search field.
    - Bug fix: keepinmemory (unloading "SCCVW.DLL") had no effect, because The number of newly opened documents in uLister wasn't counted.
Finally, the search procedure has been rewritten:
    - After opening a document in uLister, you can immediately use the SEARCHFORWARD/SEARCHBACK keys (F3/Shift+F3) if a search string was previously entered in TC (Ctrl+F/F7).
    - Previously, the search string was shared across all open uLister windows, which was very confusing for the user. Now, a search string is
      correctly supported for each open uLister window (in the original, there was no synchronization between what was actually searched for,
      what was entered into the searchbox, and what was shown in the "Not found" window).
      (Giesler implemented the SEARCHFORWARD/SEARCHBACK "case-sensitive/case-insensitive" etc search settings for each individual window - this was confirmed experimentally.)

2026-04-02
    - Compilation hacks in VS2005

2026-04-04 4.0.0.6 (Fork by evgen_b)
    - Refactoring and dead code removal
    - Slightly more detailed error messages (if library files exist but cannot be loaded, for example, when the library and plugin bit depths don't match,
      VCREDIST isn't installed, or some files are missing)
    - Added a link to Visual C++ Redistributables to the installation description
    - Changed the search order for Oracle(R) Outside In Technology: Viewer Technology libraries
      (try search in XPdist32 or XPdist64 directories first for Outside In libraries for XP/Vista, then in redist32 or redist64 as usual; no changes for other newer OSes)

2026-04-11
    - For ListSearchTextW, the Outside In Technology library now explicitly uses the internal UNICODE search engine.
    - For ListSearchText, the internal ASCII search engine is explicitly used (as was previously the case).
      (The internal UNICODE search engine allows you to use of specific UNICODE characters, but this only works correctly if
      the Outside In Technology library "knows" them and can interpret them correctly. Unfortunately, it does not support many new characters.)

2026-04-12 4.0.0.7 (Fork by evgen_b)
    - added clipboard settings to the [clipboard] section of the ulister.ini file (such as Unicode support, drag-and-drop copying of
      a selected block, selection format for spreadsheets, etc.)
    - refactoring

2026-04-25
    - refactoring and memory optimization

2026-05-09
    - clipboard setting for the database has been added to ulister.ini
    - added [viewer] section and wpdisplaymode, htmldisplaymode and emaildisplaymode settings

2026-05-10 4.0.1.0
    - refactoring and memory optimization
    - added webprevwpfitmode, webprevhtmlfitmode, and webprevemailfitmode settings
