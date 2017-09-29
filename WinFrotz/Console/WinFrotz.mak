# Microsoft Developer Studio Generated NMAKE File, Based on WinFrotz.dsp
!IF "$(CFG)" == ""
CFG=WinFrotz - Win32 Debug
!MESSAGE No configuration specified. Defaulting to WinFrotz - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "WinFrotz - Win32 Release" && "$(CFG)" !=\
 "WinFrotz - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WinFrotz - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\WinFrotz.exe"

!ELSE 

ALL : "$(OUTDIR)\WinFrotz.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Alias.obj"
	-@erase "$(INTDIR)\Buffer.obj"
	-@erase "$(INTDIR)\Fastmem.obj"
	-@erase "$(INTDIR)\Files.obj"
	-@erase "$(INTDIR)\Getopt.obj"
	-@erase "$(INTDIR)\Hotkey.obj"
	-@erase "$(INTDIR)\Input.obj"
	-@erase "$(INTDIR)\Main.obj"
	-@erase "$(INTDIR)\Math.obj"
	-@erase "$(INTDIR)\Object.obj"
	-@erase "$(INTDIR)\Other.obj"
	-@erase "$(INTDIR)\Process.obj"
	-@erase "$(INTDIR)\Random.obj"
	-@erase "$(INTDIR)\Redirect.obj"
	-@erase "$(INTDIR)\Screen.obj"
	-@erase "$(INTDIR)\Sound.obj"
	-@erase "$(INTDIR)\Stream.obj"
	-@erase "$(INTDIR)\Table.obj"
	-@erase "$(INTDIR)\Text.obj"
	-@erase "$(INTDIR)\Variable.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\Win_init.obj"
	-@erase "$(INTDIR)\win_input.obj"
	-@erase "$(INTDIR)\Win_pic.obj"
	-@erase "$(INTDIR)\Win_scrn.obj"
	-@erase "$(INTDIR)\Win_smpl.obj"
	-@erase "$(INTDIR)\Win_text.obj"
	-@erase "$(INTDIR)\WinFrotz.res"
	-@erase "$(OUTDIR)\WinFrotz.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /ML /W3 /GX /Ox /Ot /Og /Oi /Ob0 /D "WIN32" /D "NDEBUG" /D\
 "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\WinFrotz.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\WinFrotz.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinFrotz.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib winmm.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)\WinFrotz.pdb" /machine:I386 /out:"$(OUTDIR)\WinFrotz.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Alias.obj" \
	"$(INTDIR)\Buffer.obj" \
	"$(INTDIR)\Fastmem.obj" \
	"$(INTDIR)\Files.obj" \
	"$(INTDIR)\Getopt.obj" \
	"$(INTDIR)\Hotkey.obj" \
	"$(INTDIR)\Input.obj" \
	"$(INTDIR)\Main.obj" \
	"$(INTDIR)\Math.obj" \
	"$(INTDIR)\Object.obj" \
	"$(INTDIR)\Other.obj" \
	"$(INTDIR)\Process.obj" \
	"$(INTDIR)\Random.obj" \
	"$(INTDIR)\Redirect.obj" \
	"$(INTDIR)\Screen.obj" \
	"$(INTDIR)\Sound.obj" \
	"$(INTDIR)\Stream.obj" \
	"$(INTDIR)\Table.obj" \
	"$(INTDIR)\Text.obj" \
	"$(INTDIR)\Variable.obj" \
	"$(INTDIR)\Win_init.obj" \
	"$(INTDIR)\win_input.obj" \
	"$(INTDIR)\Win_pic.obj" \
	"$(INTDIR)\Win_scrn.obj" \
	"$(INTDIR)\Win_smpl.obj" \
	"$(INTDIR)\Win_text.obj" \
	"$(INTDIR)\WinFrotz.res"

"$(OUTDIR)\WinFrotz.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "WinFrotz - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\WinFrotz.exe"

!ELSE 

ALL : "$(OUTDIR)\WinFrotz.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Alias.obj"
	-@erase "$(INTDIR)\Buffer.obj"
	-@erase "$(INTDIR)\Fastmem.obj"
	-@erase "$(INTDIR)\Files.obj"
	-@erase "$(INTDIR)\Getopt.obj"
	-@erase "$(INTDIR)\Hotkey.obj"
	-@erase "$(INTDIR)\Input.obj"
	-@erase "$(INTDIR)\Main.obj"
	-@erase "$(INTDIR)\Math.obj"
	-@erase "$(INTDIR)\Object.obj"
	-@erase "$(INTDIR)\Other.obj"
	-@erase "$(INTDIR)\Process.obj"
	-@erase "$(INTDIR)\Random.obj"
	-@erase "$(INTDIR)\Redirect.obj"
	-@erase "$(INTDIR)\Screen.obj"
	-@erase "$(INTDIR)\Sound.obj"
	-@erase "$(INTDIR)\Stream.obj"
	-@erase "$(INTDIR)\Table.obj"
	-@erase "$(INTDIR)\Text.obj"
	-@erase "$(INTDIR)\Variable.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\Win_init.obj"
	-@erase "$(INTDIR)\win_input.obj"
	-@erase "$(INTDIR)\Win_pic.obj"
	-@erase "$(INTDIR)\Win_scrn.obj"
	-@erase "$(INTDIR)\Win_smpl.obj"
	-@erase "$(INTDIR)\Win_text.obj"
	-@erase "$(INTDIR)\WinFrotz.res"
	-@erase "$(OUTDIR)\WinFrotz.exe"
	-@erase "$(OUTDIR)\WinFrotz.ilk"
	-@erase "$(OUTDIR)\WinFrotz.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\
 /D "_MBCS" /D "WINFROTZ" /Fp"$(INTDIR)\WinFrotz.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\WinFrotz.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinFrotz.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib winmm.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)\WinFrotz.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\WinFrotz.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\Alias.obj" \
	"$(INTDIR)\Buffer.obj" \
	"$(INTDIR)\Fastmem.obj" \
	"$(INTDIR)\Files.obj" \
	"$(INTDIR)\Getopt.obj" \
	"$(INTDIR)\Hotkey.obj" \
	"$(INTDIR)\Input.obj" \
	"$(INTDIR)\Main.obj" \
	"$(INTDIR)\Math.obj" \
	"$(INTDIR)\Object.obj" \
	"$(INTDIR)\Other.obj" \
	"$(INTDIR)\Process.obj" \
	"$(INTDIR)\Random.obj" \
	"$(INTDIR)\Redirect.obj" \
	"$(INTDIR)\Screen.obj" \
	"$(INTDIR)\Sound.obj" \
	"$(INTDIR)\Stream.obj" \
	"$(INTDIR)\Table.obj" \
	"$(INTDIR)\Text.obj" \
	"$(INTDIR)\Variable.obj" \
	"$(INTDIR)\Win_init.obj" \
	"$(INTDIR)\win_input.obj" \
	"$(INTDIR)\Win_pic.obj" \
	"$(INTDIR)\Win_scrn.obj" \
	"$(INTDIR)\Win_smpl.obj" \
	"$(INTDIR)\Win_text.obj" \
	"$(INTDIR)\WinFrotz.res"

"$(OUTDIR)\WinFrotz.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(CFG)" == "WinFrotz - Win32 Release" || "$(CFG)" ==\
 "WinFrotz - Win32 Debug"
SOURCE=..\Frotz\Alias.c
DEP_CPP_ALIAS=\
	"..\Frotz\Frotz.h"\
	

"$(INTDIR)\Alias.obj" : $(SOURCE) $(DEP_CPP_ALIAS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Frotz\Buffer.c
DEP_CPP_BUFFE=\
	"..\Frotz\Frotz.h"\
	

"$(INTDIR)\Buffer.obj" : $(SOURCE) $(DEP_CPP_BUFFE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Frotz\Fastmem.c
DEP_CPP_FASTM=\
	"..\Frotz\Frotz.h"\
	

"$(INTDIR)\Fastmem.obj" : $(SOURCE) $(DEP_CPP_FASTM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Frotz\Files.c
DEP_CPP_FILES=\
	"..\Frotz\Frotz.h"\
	

"$(INTDIR)\Files.obj" : $(SOURCE) $(DEP_CPP_FILES) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Frotz\Getopt.c

"$(INTDIR)\Getopt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Frotz\Hotkey.c
DEP_CPP_HOTKE=\
	"..\Frotz\Frotz.h"\
	

"$(INTDIR)\Hotkey.obj" : $(SOURCE) $(DEP_CPP_HOTKE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Frotz\Input.c
DEP_CPP_INPUT=\
	"..\Frotz\Frotz.h"\
	

"$(INTDIR)\Input.obj" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Frotz\Main.c
DEP_CPP_MAIN_=\
	"..\Frotz\Frotz.h"\
	

"$(INTDIR)\Main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Frotz\Math.c
DEP_CPP_MATH_=\
	"..\Frotz\Frotz.h"\
	

"$(INTDIR)\Math.obj" : $(SOURCE) $(DEP_CPP_MATH_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Frotz\Object.c
DEP_CPP_OBJEC=\
	"..\Frotz\Frotz.h"\
	

"$(INTDIR)\Object.obj" : $(SOURCE) $(DEP_CPP_OBJEC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Frotz\Other.c

"$(INTDIR)\Other.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Frotz\Process.c
DEP_CPP_PROCE=\
	"..\Frotz\Frotz.h"\
	

"$(INTDIR)\Process.obj" : $(SOURCE) $(DEP_CPP_PROCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Frotz\Random.c
DEP_CPP_RANDO=\
	"..\Frotz\Frotz.h"\
	

"$(INTDIR)\Random.obj" : $(SOURCE) $(DEP_CPP_RANDO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Frotz\Redirect.c
DEP_CPP_REDIR=\
	"..\Frotz\Frotz.h"\
	

"$(INTDIR)\Redirect.obj" : $(SOURCE) $(DEP_CPP_REDIR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Frotz\Screen.c
DEP_CPP_SCREE=\
	"..\Frotz\Frotz.h"\
	

"$(INTDIR)\Screen.obj" : $(SOURCE) $(DEP_CPP_SCREE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Frotz\Sound.c
DEP_CPP_SOUND=\
	"..\Frotz\Frotz.h"\
	

"$(INTDIR)\Sound.obj" : $(SOURCE) $(DEP_CPP_SOUND) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Frotz\Stream.c
DEP_CPP_STREA=\
	"..\Frotz\Frotz.h"\
	

"$(INTDIR)\Stream.obj" : $(SOURCE) $(DEP_CPP_STREA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Frotz\Table.c
DEP_CPP_TABLE=\
	"..\Frotz\Frotz.h"\
	

"$(INTDIR)\Table.obj" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Frotz\Text.c
DEP_CPP_TEXT_=\
	"..\Frotz\Frotz.h"\
	

"$(INTDIR)\Text.obj" : $(SOURCE) $(DEP_CPP_TEXT_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Frotz\Variable.c
DEP_CPP_VARIA=\
	"..\Frotz\Frotz.h"\
	

"$(INTDIR)\Variable.obj" : $(SOURCE) $(DEP_CPP_VARIA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\C\Win_init.c
DEP_CPP_WIN_I=\
	"..\Frotz\Frotz.h"\
	"..\include\crosscomp.h"\
	"..\include\states.h"\
	"..\include\Windefs.h"\
	

"$(INTDIR)\Win_init.obj" : $(SOURCE) $(DEP_CPP_WIN_I) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\C\win_input.c
DEP_CPP_WIN_IN=\
	"..\Frotz\Frotz.h"\
	"..\include\crosscomp.h"\
	"..\include\states.h"\
	"..\include\Windefs.h"\
	

!IF  "$(CFG)" == "WinFrotz - Win32 Release"

CPP_SWITCHES=/nologo /G5 /ML /W3 /GX /Ox /Ot /Og /Oi /Ob0 /D "WIN32" /D\
 "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\WinFrotz.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\win_input.obj" : $(SOURCE) $(DEP_CPP_WIN_IN) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "WinFrotz - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_CONSOLE" /D "_MBCS" /D "WINFROTZ" /Fp"$(INTDIR)\WinFrotz.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\win_input.obj" : $(SOURCE) $(DEP_CPP_WIN_IN) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\C\Win_pic.c
DEP_CPP_WIN_P=\
	"..\Frotz\Frotz.h"\
	"..\include\crosscomp.h"\
	"..\include\states.h"\
	"..\include\Windefs.h"\
	

"$(INTDIR)\Win_pic.obj" : $(SOURCE) $(DEP_CPP_WIN_P) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\C\Win_scrn.c
DEP_CPP_WIN_S=\
	"..\Frotz\Frotz.h"\
	"..\include\crosscomp.h"\
	"..\include\states.h"\
	"..\include\Windefs.h"\
	

"$(INTDIR)\Win_scrn.obj" : $(SOURCE) $(DEP_CPP_WIN_S) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\C\Win_smpl.c
DEP_CPP_WIN_SM=\
	"..\Frotz\Frotz.h"\
	"..\include\crosscomp.h"\
	"..\include\states.h"\
	"..\include\Windefs.h"\
	

"$(INTDIR)\Win_smpl.obj" : $(SOURCE) $(DEP_CPP_WIN_SM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\C\Win_text.c
DEP_CPP_WIN_T=\
	"..\Frotz\Frotz.h"\
	"..\include\crosscomp.h"\
	"..\include\states.h"\
	"..\include\Windefs.h"\
	

"$(INTDIR)\Win_text.obj" : $(SOURCE) $(DEP_CPP_WIN_T) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\WinFrotz.rc

"$(INTDIR)\WinFrotz.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

