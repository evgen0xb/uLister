REM Creative Commons Attribution 4.0 International Public License
REM 2026 (L) evgen_b

SET "mypath=%~p0"
SET "mydisk=%~d0"
SET "mypath=%mypath:~0,-1%"
CD /d "%mydisk%%mypath%"

SET "OUTDIR=wlx-ulister"

RD /s /q "%OUTDIR%"
MD "%OUTDIR%"
MD "%OUTDIR%\redist32"
MD "%OUTDIR%\redist64"
ECHO.Put 32-bit Oracle OiT DLLs here.> "%OUTDIR%\redist32\!.txt"
ECHO.Put 64-bit Oracle OiT DLLs here.> "%OUTDIR%\redist64\!.txt"

xcopy.exe /I /E /C /H /R /Y  ".\doc" "%OUTDIR%\doc"

COPY ".\plugin config files\*.*" "%OUTDIR%"

COPY .\release\wlx\ulister.wlx "%OUTDIR%"
COPY .\x64\release\wlx\ulister.wlx64 "%OUTDIR%"

:: ====================================================================================================

SET listfile="%CD%\%OUTDIR%.lst"
SET "root=%OUTDIR%"
DEL /f /q "%OUTDIR%.rep" > NUL 2>&1
DEL /f /q "%OUTDIR%.lst" > NUL 2>&1
DEL /f /q "%OUTDIR%.cab" > NUL 2>&1

REM CALL :recurcive1 %root% %root%
CALL :recurcive2 "%root%" "%root%\" ""

REM By default, MaxDiskSize is set to the floppy disk size of 1.44 MB.
REM This is the only parameter that needs to be explicitly specified based on the size, i.e., 0 is the maximum (2 GB archive, 65,535 files).
REM All other parameters related to archive size and partitioning are set to 0 by default, i.e., the maximum values.
REM MSZIP compression is used by default; it is not as efficient as LZX, but it works in all older and newer versions of Windows.

makecab.exe /d MaxDiskSize=0 /d "DiskDirectoryTemplate=%CD%" /d "CabinetNameTemplate=%OUTDIR%.cab" /d Cabinet=on /d Compress=on /d RptFileName=nul /d InfFileName=nul /F %listfile% > "%OUTDIR%.rep" 2>&1
SET el=%ERRORLEVEL%
IF NOT "%el%" == "0" GOTO :errmakecab

RD /s /q "%OUTDIR%"
DEL /f /q "%OUTDIR%.rep" > NUL 2>&1
DEL /f /q "%OUTDIR%.lst" > NUL 2>&1

PAUSE
GOTO :EOF

:: ====================================================================================================

:errmakecab
start "1" /WAIT notepad.exe "%OUTDIR%.rep"
RD /s /q "%OUTDIR%"
DEL /f /q "%OUTDIR%.rep" > NUL 2>&1
DEL /f /q "%OUTDIR%.lst" > NUL 2>&1
DEL /f /q "%OUTDIR%.cab" > NUL 2>&1
GOTO :EOF

:: ====================================================================================================

:recurcive1
CD %1
ECHO..Set DestinationDir=%2 >> %listfile%
FOR %%i IN (*) DO ECHO."%~2\%%i" >> %listfile%
FOR /D %%i IN (*) DO CALL :recurcive1 "%%i" "%~2\%%i"
CD ..
GOTO :EOF

:: ====================================================================================================

:recurcive2
CD %1
FOR %%i IN (*) DO ECHO."%~2%%i" >> %listfile%
FOR /D %%i IN (*) DO (
    ECHO..Set DestinationDir="%~3%%i" >> %listfile%
    CALL :recurcive2 "%%i" "%~2%%i\" "%~3%%i\" )
CD ..
GOTO :EOF

:: ====================================================================================================
