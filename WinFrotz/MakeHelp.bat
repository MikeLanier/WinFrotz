@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by WINFROTZ.HPJ. >"hlp\WinFrotz.hm"
echo. >>"hlp\WinFrotz.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\WinFrotz.hm"
d:\progra~1\devstu~1\vc\bin\makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\WinFrotz.hm"
echo. >>"hlp\WinFrotz.hm"
echo // Prompts (IDP_*) >>"hlp\WinFrotz.hm"
d:\progra~1\devstu~1\vc\bin\makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\WinFrotz.hm"
echo. >>"hlp\WinFrotz.hm"
echo // Resources (IDR_*) >>"hlp\WinFrotz.hm"
d:\progra~1\devstu~1\vc\bin\makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\WinFrotz.hm"
echo. >>"hlp\WinFrotz.hm"
echo // Dialogs (IDD_*) >>"hlp\WinFrotz.hm"
d:\progra~1\devstu~1\vc\bin\makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\WinFrotz.hm"
echo. >>"hlp\WinFrotz.hm"
echo // Frame Controls (IDW_*) >>"hlp\WinFrotz.hm"
d:\progra~1\devstu~1\vc\bin\makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\WinFrotz.hm"
REM -- Make help for Project WinFrotz


echo Building Win32 Help files
start /wait D:\progra~1\devstudio\vc\bin\hcw /C /E /M "hlp\WinFrotz.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\WinFrotz.hlp" goto :Error
if not exist "hlp\WinFrotz.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\WinFrotz.hlp" Debug
if exist Debug\nul copy "hlp\WinFrotz.cnt" Debug
if exist Release\nul copy "hlp\WinFrotz.hlp" Release
if exist Release\nul copy "hlp\WinFrotz.cnt" Release
echo.
goto :done

:Error
echo hlp\WinFrotz.hpj(1) : error: Problem encountered creating help file

:done
echo.
