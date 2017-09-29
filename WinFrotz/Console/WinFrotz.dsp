# Microsoft Developer Studio Project File - Name="WinFrotz" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=WinFrotz - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WinFrotz.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WinFrotz.mak" CFG="WinFrotz - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WinFrotz - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "WinFrotz - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WinFrotz - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G5 /W3 /GX /Ox /Ot /Og /Oi /Ob0 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# SUBTRACT CPP /Os
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "WinFrotz - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "WINFROTZ" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "WinFrotz - Win32 Release"
# Name "WinFrotz - Win32 Debug"
# Begin Group "Source"

# PROP Default_Filter ""
# Begin Group "Frotz"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Frotz\Alias.c
# End Source File
# Begin Source File

SOURCE=..\Frotz\Buffer.c
# End Source File
# Begin Source File

SOURCE=..\Frotz\Fastmem.c
# End Source File
# Begin Source File

SOURCE=..\Frotz\Files.c
# End Source File
# Begin Source File

SOURCE=..\Frotz\Getopt.c
# End Source File
# Begin Source File

SOURCE=..\Frotz\Hotkey.c
# End Source File
# Begin Source File

SOURCE=..\Frotz\Input.c
# End Source File
# Begin Source File

SOURCE=..\Frotz\Main.c
# End Source File
# Begin Source File

SOURCE=..\Frotz\Math.c
# End Source File
# Begin Source File

SOURCE=..\Frotz\Object.c
# End Source File
# Begin Source File

SOURCE=..\Frotz\Other.c
# End Source File
# Begin Source File

SOURCE=..\Frotz\Process.c
# End Source File
# Begin Source File

SOURCE=..\Frotz\Random.c
# End Source File
# Begin Source File

SOURCE=..\Frotz\Redirect.c
# End Source File
# Begin Source File

SOURCE=..\Frotz\Screen.c
# End Source File
# Begin Source File

SOURCE=..\Frotz\Sound.c
# End Source File
# Begin Source File

SOURCE=..\Frotz\Stream.c
# End Source File
# Begin Source File

SOURCE=..\Frotz\Table.c
# End Source File
# Begin Source File

SOURCE=..\Frotz\Text.c
# End Source File
# Begin Source File

SOURCE=..\Frotz\Variable.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\C\Win_init.c
# End Source File
# Begin Source File

SOURCE=..\C\win_input.c

!IF  "$(CFG)" == "WinFrotz - Win32 Release"

# SUBTRACT CPP /Os

!ELSEIF  "$(CFG)" == "WinFrotz - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\C\Win_pic.c
# End Source File
# Begin Source File

SOURCE=..\C\Win_scrn.c
# End Source File
# Begin Source File

SOURCE=..\C\Win_smpl.c
# End Source File
# Begin Source File

SOURCE=..\C\Win_text.c
# End Source File
# End Group
# Begin Group "Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\crosscomp.h
# End Source File
# Begin Source File

SOURCE=..\Frotz\Frotz.h
# End Source File
# Begin Source File

SOURCE=..\include\Globs.h
# End Source File
# Begin Source File

SOURCE=..\include\Windefs.h
# End Source File
# Begin Source File

SOURCE=..\include\WinFrotz.h
# End Source File
# End Group
# Begin Group "Text files"

# PROP Default_Filter "*.txt"
# Begin Source File

SOURCE=.\Dist\Files.txt
# End Source File
# Begin Source File

SOURCE=.\Dist\Readme.txt
# End Source File
# Begin Source File

SOURCE=.\Dist\Revision.txt
# End Source File
# End Group
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\WinFrotz.rc
# End Source File
# End Target
# End Project
