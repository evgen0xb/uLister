1. About the Program
The uLister internal viewer plugin supports over 500 different file formats.

2. Supported Formats
Many popular word processor, spreadsheet, presentation, raster, and vector image formats.
A complete list can be found in OutsideIn_8.5.7_Supported_File_Formats.pdf

3. License
The plugin is provided as-is and without any warranty under the GPLv3 license.
Although the plugin is freeware, it uses third-party Oracle libraries. Please read the license before using it.
https://www.oracle.com/downloads/licenses/standard-license.html

Outside In Viewer (C) 1991, 2026 Oracle(R)
(C) 2011 Egor Vlaznev (aka arax)
(C) 2016 Michael Loster (aka milo1012)
(C) 2024 Marek Jasinski (aka Marek)
(C) 2024 Vogelbacher Andreas (aka avogelba)
(C) 2026 evgen_b (aka evgen_b)

Main README.md by (C) 2026 yozhic

Additional permission under GNU GPL version 3 section 7 (https://github.com/microsoft/WindowsAppSDK/discussions/3511#discussioncomment-10121323):
If you modify this Program, or any covered work, by linking or combining it with the Microsoft Visual C++ Redistributable, Windows SDK (or a modified version of these libraries), containing
parts covered by the terms of the Microsoft Software License, the licensors of this Program grant you additional permission to convey the resulting work.

The plugin's source code is available at https://github.com/evgen0xb/uLister

4. Libraries Used
Oracle(R) Outside In Technology: Viewer Technology.
This library, in turn, uses Microsoft Visual C++ Redistributable files.

5. Configuration
The settings are stored in the ulister.ini file, which is searched in the following order:
1) plugin directory
2) directory containing %COMMANDER_INI%
3) %APPDATA%
The file will not be loaded if the path exceeds 260 characters (Windows limit).

Since version 4.2.0.0, if the ulister.ini file is not found, the plugin will attempt to create a file with default settings in its directory.

The name of the settings section is [ulister].

The following parameters are available in version 4.0.0.4 of the plugin:

keepinmemory        (default 1) - don't unload Outside In Viewer libraries from memory after first use, which reduces
                    document loading time but increases memory consumption.
noloadtypes         list of formats excluded from loading
onlyloadtypes       list of formats allowed to loading
nopreviewtypes      list of format thumbnails excluded from displaying
onlypreviewtypes    list of format thumbnails allowed to displaying

Four-digit format codes with descriptions are located in the formats.txt file (see also the keyboard shortcut Ctrl+I).
Any non-numeric character, such as a comma or space, can be used as a separator.
If an allow string is specified, the corresponding deny string is ignored.

optionsdir          path to the directory in which the ".oit" subdirectory is created,
                    containing the internal settings of the Oracle Outside In Technology: Viewer Technology library;
                    environment %variables% are supported;
                    Windows limitation - path length is 260 characters maximum;
                    For optionsdir to work correctly, you must use a patched version of the sccut.dll file.
                    Some patched versions of this file are located in the "OIT_DATA_PATH support"
                    directory (for example, the latest versions supporting Windows XP).

If the optionsdir parameter is not specified, the library creates ".oit" in %APPDATA%; if this value is undefined, it creates it in %LOCALAPPDATA%;
if this value is also undefined, it creates it in the library directory.

mwhscrollinvert     (default on) - invert mouse wheel horizontal scroll,
                    off - do not invert; this option is available starting with version 4.0.3.0.

Version 4.2.0.0 now displays tooltips when switching modes:

tooltipsdelayms     (default 3000) - tooltip display time in milliseconds
tooltipstransparency (default 244, from 0 to 255 inclusive) - tooltip transparency (only works in Windows 8 and above, without overloading older systems);
                    use a value of -1 to force transparency mode off.

Since version 4.0.0.7 of the plugin, ulister.ini can be used to write some useful internal settings of the Outside In Technology
library, stored in a proprietary binary representation in the ".oit" directory.

Almost all such options by default have a special reserved value "skip", i.e. skip this setting from the ini-file
and use the internal library setting stored in Outside In Technology (directory ".oit").
However, if the option in the ini is explicitly set to a different value, then the library (not the plugin) overwrites this value in .oit storage.

The first part of these parameters is related to the copying to the clipboard engine of Outside In Technology library, and the options should be located in the clipboard section.
Some settings can be changed from the context menu. Some users may want these settings to be remembered; for this purpose, they
should be set as "skip" in the INI file. Others may want these settings to be restored to their original state
by restarting Total Commander, so that they cannot be overwritten from the menu; in this case, they should be explicitly
set in the INI file. However, most other options are not available in the context menu and can only be changed from the INI file.

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
This option can also be controlled from the context menu "Options->Drag'n'Drop" (plugin version 4.4.0.0).

It is recommended to set all clipboard section parameters to "on" (especially for unicode).

This settings specifies the format for copying cells from spreadsheets (SDK A.4.8 SCCID_SSCLIPBOARD)
or from database (SDK A.4.1 SCCID_DBCLIPBOARD)
spreadsheet=skip|rtf|tabs|optimizedtabs
database=skip|rtf|tabs|optimizedtabs
    rtf         - copy as a table using RTF format
    tabs        - as plain text, separated between cells by tabs
    optimizedtabs - as in the previous case, but empty cells are skipped

This setting in the [viewer] section indicates how the word processor/HTML/email display engine displays documents since 4.0.1.0
(A.7.2 SCCID_WPDISPLAYMODE / SCCID_HTMLDISPLAYMODE / SCCID_EMAILDISPLAYMODE)

wpdisplaymode=skip|draft|normal|preview|weblayout
htmldisplaymode=skip|draft|normal|preview|weblayout
emaildisplaymode=skip|draft|normal|preview|weblayout
    draft       - Display using only a single font and size (SCCID_DEFAULTDISPLAYFONT), do not display embedded graphics, do not display graphic or table borders, wrap the text to the size of the view window.
    normal      - Display all supported formatting, wrap the text to the size of the view window.
    preview     - Display all supported formatting, wrap the text as it will be printed.
    weblayout   - Display all supported formatting, wrap the text as it would appear in a browser.

This settings in the [viewer] section controls the size of word processor pages when using preview or weblayout mode
(A.7.3 SCCID_WPFITMODE / SCCID_HTMLFITMODE / SCCID_EMAILFITMODE)
These settings are applied correctly only from the second launch of ulister after
changes in the .ini file due to an bug in the Outside In Viewer library (workaround).

webprevwpfitmode=skip|original|width|window
webprevhtmlfitmode=skip|original|width|window
webprevemailfitmode=skip|original|width|window
    original    - sizes the preview page to the actual size
    width       - sizes the preview page to the width of the window
    window      - sizes the preview page to the window; sizes the weblayout to the width of the window

Version 4.0.2.0 of the plugin now includes settings for controlling the display of bitmap and vector graphics.
(A.5.11 SCCID_VECFITMODE, A.5.4 SCCID_BMPFITMODE)
Some image containers, such as Macintosh Pict, can contain both bitmap and vector graphics.
For some reason, the Outside In Technology library classifies such files as vector.
vectorfitmode=skip|best|original|window|height|width|stretch
bitmapfitmode=skip|best|original|window|height|width|stretch***|imagesize
    best        - If the view window is smaller than the original image, this option will fit it to the window.
                If the view window is larger than the original image, the image will be displayed at its original size.
    original    - The image is displayed one pixel on the screen for every unit in the images coordinate system for vector image or
                pixel for pixel on the screen for bitmap; size of the window has no effect.
    window      - The image will be stretched to fill as much of the window as possible while maintaining its proper aspect ratio.
    height      - The image will be stretched so its full height fits in the height of the window.
                Depending on the image, its full width may or may not fit inside the window.
    width       - The image will be stretched so its full width fits in the width of the window.
                Depending on the image, its full height may or may not fit inside the window.
    stretch     - The image will be stretched to fill the window. The images aspect ratio is not maintained.
                Although the stretch parameter is defined in the SDK for bitmap graphics too, it has no effect for them.
    imagesize   - Scale to image size.

Version 4.1.0.0 of the plugin now includes a option for controlling the display of spreadsheets.
(A.4.9 SCCID_SSDRAFTMODE, A.4.19 SCCID_SSSHOWHIDDENCELLS)
spreadsheetdisplaymode=skip|draft|normal|normalhidden
    draft       - draft document display with a single font of the same size, without graphics, etc
    normal      - display without restrictions
    normalhidden- additionally display hidden rows and columns

In version 4.4.0.0 of the plugin, the following memory management settings are available in the memory section:

[memory]
readbuffersizekb=skip|Size_KB
mmapbuffersizekb=skip|Size_KB
tempbuffersizekb=skip|Size_KB
memorymode=skip|4m|16m|64m|256m|1024m

(A.12.1 SCCID_IO_BUFFERSIZE)

The value is specified in kilobytes, you can specify hexadecimal using the "0x" prefix
(or octal with "0" if you're on a PDP-11, so it's best to never write numbers starting with a zero here, like "012"!).

ReadBufferSizeKB used to define the number of Kbytes that will read from disk into memory at any given time.
Once the buffer has data, further file reads will proceed within the buffer until the end of the buffer is reached,
at which point the buffer will again be filled from the disk. This can lead to performance improvements in many file formats,
regardless of the size of the document.

The default value is 2 KB,
the minimum is to read 1 KB at a time, and the maximum is 4194303 (4 GB minus 1 KB, or 0x003fffff)

MMapBufferSizeKB used to define a maximum size that a document can be and use a memory-mapped I/O model.
In this situation, the entire file is read from disk into memory and all further I/O is performed on the data in memory.
This can lead to significantly improved performance, but note that either the entire file can be read into memory, or it cannot.
If both of these buffers are set, then if the file is smaller than the MMapBufferSizeKB, the entire file will be read into memory;
if not, it will be read in blocks defined by the ReadBufferSizeKB.

The default is 8192 KB (memory-mapped files up to 8 MB),
the minimum value is 0 KB (do not use memory-mapped I/O), and the maximum is 4194303 (4 GB minus 1 KB, or 0x003fffff).

TempBufferSizeKB is maximum size that a temporary file can occupy in memory before being written to disk as a physical file.
Storing temporary files in memory can boost performance on archives and files that have embedded objects or attachments.

The default is 2048 KB (temporary files in memory are limited to 2 MB).
The minimum value is 0 KB (all temporary files are immediately written to disk). The maximum is 4194303 (4 GB minus 1 KB, or 0x003fffff).
(Note from evgen_b - this likely refers to the total size for all required temporary files, not just one.)

(A.12.3 SCCOPT_DOCUMENTMEMORYMODE)

The MemoryMode option determines the maximum amount of memory that the chunker may use to store the document's data,
from 4 MB to 1 GB. The more memory the chunker has available to it, the less often it needs to re-read data from the document.
The default is 256 MB.

TODO: In the future, it would be useful to heuristically select a more optimal value for these parameters, AUTO, dynamically,
depending on the amount of free RAM and the file size, before opening it (is this possible?).

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
In version 4.4.0.0, the search can also be launched from the context menu using the right mouse button.
Find Next/Previous - F3/Shift+F3
(Search is not available in some formats; case-insensitive search option is supported)
You can zoom in or out using Ctrl "+" / Ctrl "-" or Ctrl + Mouse Wheel (in older versions of the OutsideIn library, as for WinXP, it is not available for some viewing modes).
Keyboard shortcut Ctrl+NUM*/Ctrl+8 or Ctrl + middle mouse button - reset zoom to 100%.
Use Shift + mouse wheel (or special horizontal mouse wheel) to scroll horizontally.

For non-graphical documents, zooming is only achieved by changing the font size, which in certain situations will result in
some formatting issues in the document, but it's still better than nothing at all.
Outside In Technology never implemented full scaling for non-graphical documents, and it appears that the mouse message interface
for scaling and horizontal scrolling was deliberately blocked in the library due to rendering artifacts.

For some formats, you can select a block and copy it to the clipboard, or drag-and-drop copy.
For Visio, database and spreadsheet formats, you can switch between sheets using Ctrl+PgDn/Ctrl+PgUp.
For PowerPoint/Lotus Freelance presentations, etc., slide switching is performed using the PgDn/PgUp keys, cursor keys or the mouse wheel.
Thus, keys are mainly processed internally by the library and are strictly dependent on the document type and viewing mode.

Outside In Viewer can work with simple containers such as ZIP files, MS ZIP-compressed MS CAB files, Outlook Personal Storage Table (*.pst), and so on.
Double-click a link or file contained within it will unzip the file to a temporary directory and open it in a new viewing window.

The keyboard shortcut Ctrl+Shift+R reload the configuration parameters from the ini file (some of them may take effect immediately).
When reloading parameters, the plugin does not look for the new location of the ulister.ini file.

Ctrl+M/Ctrl+Shift+M and Ctrl+Shift + Mouse Wheel:
    for Word Processor / HTML / EMAIL switching between view modes (draft->normal->preview->weblayout);
    90 degree rotation for bitmaps (gif, jpeg, png, ...);
    change sort order for archives (none->name->size->date);
    switching between spreadsheet viewing modes (draft->normal->normal with hidden rows and columns displayed).

Ctrl+I: 
    shows a "File Information" window with some technical information about the viewing mode; 
    in this window it is possible to export the actual version of formats.txt corresponding to the used Viewer Technology library.
    This action can also be launched from the context menu - "File info" (plugin version 4.3.2.0).

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
    - refactoring and code optimization
    - added webprevwpfitmode, webprevhtmlfitmode, and webprevemailfitmode settings

2026-05-12
    - added vectorfitmode and bitmapfitmode settings
    - refactoring of the SCCVW_VIEWTHISFILE event handling procedure (should now work on FreeCommander/Double Commander as well)

2026-05-20 4.0.2.0
    - Complete redesign for windows messages handling
    - Separate zoom settings for graphics and documents
    - Added: Ctrl+NUM*/Ctrl+8 - Reset zoom to 100%
    - Oracle bug fix: Ctrl + Mouse Wheel (zoom) now works on all documents (but not in all viewing modes; in the original, it only works on graphics)
    - Oracle bug fix: Shift + Mouse Wheel - Added horizontal scrolling
    - Documentation update

2026-05-23
    - added: Ctrl + middle mouse button - reset zoom to 100%
    - added: Ctrl+Shift+R reload the configuration options from the ini file
    - refactoring
    - added option mwhscrollinvert (invert mouse wheel horizontal scroll)

2026-05-24
    - oracle bug workaround: zoom not working if preview or weblayout mode of Word Processor / HTML / EMAIL (A.10.5 SCCID_FONTSCALINGFACTOR Note)
    - code optimization
    - view optimization

2026-05-28 4.0.3.0
    - added SDK PDF "OIVWR 854.pdf", links from the readme lead to its sections
    - clarifying the license

2026-05-30
    - refactoring keepinmemory option
    - refactoring keepinmemory for SCCFI.DLL

2026-05-31
    - in accordance with the TC SDK standard, ListLoadW and ListLoadNext now actually return an error if the document format is not supported
    - added minimal mode for debug messages
    - original bugfix: the plugin don't destroy its view window after loading an unsupported file type
    - original bugfix: memory leak
    - bugfix related to unclear TC SDK

2026-06-01
    - code for testing the keepinmemory option (OK)

2026-06-07 4.1.0.0
    - added: Ctrl+M/Ctrl+Shift+M and Ctrl+Shift + Mouse Wheel - switching between Word Processor / HTML / EMAIL viewing modes (draft->normal->preview->weblayout)
    - added: Ctrl+M/Ctrl+Shift+M and Ctrl+Shift + Mouse Wheel - 90 degree rotation for bitmaps (gif, jpeg, png, ...)
    - added: Ctrl+M/Ctrl+Shift+M and Ctrl+Shift + Mouse Wheel - change sort order for archives (none->name->size->date)
    - added: Ctrl+M/Ctrl+Shift+M and Ctrl+Shift + Mouse Wheel - switching between spreadsheet viewing modes (draft->normal->normal with hidden rows and columns displayed)
    - added spreadsheetdisplaymode option

2026-06-12
    - added display of viewing mode when changing
    - added options tooltipsdelayms and tooltipstransparency
    - if the ulister.ini file is not found, try creating a default one in the plugin directory

2026-06-13
    - ulister can now export the actual version of formats.txt corresponding to the used Viewer Technology library

2026-06-15
    - added a "File Information" window with some technical information about the file viewing mode, accessible by pressing Ctrl+I
    - added the ability to export a file with format types (formats.txt) from the "File Information" window

2026-06-17 4.2.0.0
    - tooltip transparency only works in Windows 8 and above (without overloading older systems)
    - formats.txt has been updated (OutsideIn version 8.5.8)
    - added FILID.pdf documentation
    - updating plugin documentation

2026-06-19
    - refactoring

2026-06-20
    - total refactoring

2026-06-21
    - refactoring; memory and code optimization

2026-06-22
    - fix: center the "file info" window relative to its parent

2026-06-23
    - fix: Unix LF -> Windows CR LF

2026-06-24
    - the "Whole words only" checkbox in the search window (Ctrl+F/F7) now means that the search starts from the beginning of the document (or from the end if searching in the opposite direction).
    - custom context menu in the "File Info" window
    - in the TC search window (Ctrl+F/F7), replace the text of the 'Whole words only' button with 'FROM BEGINING' (heuristics)
    - the 'File info' item has been added to the context menu

2026-06-29
    - File ID documentation update - PDF version 8.5.4
    - updating plugin documentation (RU)
    - added formats*.md

2026-07-01 4.3.2.0
    - slightly more detailed error messages
    - refactoring

2026-07-07
    - added options readbuffersizekb, mmapbuffersizekb and tempbuffersizekb

2026-07-10
    - added option memorymode
    - refactoring
    - "Options->Drag'n'Drop" has been added to the context menu
    - the "Find" item has been added to the context menu

2026-07-11
    - full-screen mode support has been implemented for all document types (via a context menu item)
    - refactoring of the Full Screen mode
