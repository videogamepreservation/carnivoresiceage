# Microsoft Developer Studio Generated NMAKE File, Based on C2.DSP
!IF "$(CFG)" == ""
CFG=C2 - Win32 Debug_D3D
!MESSAGE No configuration specified. Defaulting to C2 - Win32 Debug_D3D.
!ENDIF 

!IF "$(CFG)" != "C2 - Win32 Debug_Soft" && "$(CFG)" != "C2 - Win32 Debug_3DFX"\
 && "$(CFG)" != "C2 - Win32 Debug_D3D" && "$(CFG)" != "C2 - Win32 Release_Soft"\
 && "$(CFG)" != "C2 - Win32 Release_3DFX" && "$(CFG)" !=\
 "C2 - Win32 Release_D3D"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "C2.MAK" CFG="C2 - Win32 Debug_D3D"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "C2 - Win32 Debug_Soft" (based on "Win32 (x86) Application")
!MESSAGE "C2 - Win32 Debug_3DFX" (based on "Win32 (x86) Application")
!MESSAGE "C2 - Win32 Debug_D3D" (based on "Win32 (x86) Application")
!MESSAGE "C2 - Win32 Release_Soft" (based on "Win32 (x86) Application")
!MESSAGE "C2 - Win32 Release_3DFX" (based on "Win32 (x86) Application")
!MESSAGE "C2 - Win32 Release_D3D" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "C2 - Win32 Debug_Soft"

OUTDIR=.\DEBUG_S
INTDIR=.\DEBUG_S
# Begin Custom Macros
OutDir=.\DEBUG_S
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\C2.exe"

!ELSE 

ALL : "$(OUTDIR)\C2.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Audio_DLL.obj"
	-@erase "$(INTDIR)\Characters.obj"
	-@erase "$(INTDIR)\Game.obj"
	-@erase "$(INTDIR)\Hunt2.obj"
	-@erase "$(INTDIR)\Interface.obj"
	-@erase "$(INTDIR)\mathematics.obj"
	-@erase "$(INTDIR)\Render3DFX.obj"
	-@erase "$(INTDIR)\renderd3d.obj"
	-@erase "$(INTDIR)\RenderSoft.obj"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\Resources.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\C2.exe"
	-@erase "$(OUTDIR)\C2.ilk"
	-@erase "$(OUTDIR)\C2.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_soft" /Fp"$(INTDIR)\C2.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\DEBUG_S/
CPP_SBRS=.

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\resource.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\C2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=ole32.lib oleaut32.lib odbc32.lib odbccp32.lib kernel32.lib\
 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib\
 uuid.lib winmm.lib ddraw.lib dsound.lib wsock32.lib GLIDE2x.LIB SST1INIT.LIB\
 /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\C2.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)\C2.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\Audio_DLL.obj" \
	"$(INTDIR)\Characters.obj" \
	"$(INTDIR)\Game.obj" \
	"$(INTDIR)\Hunt2.obj" \
	"$(INTDIR)\Interface.obj" \
	"$(INTDIR)\mathematics.obj" \
	"$(INTDIR)\Render3DFX.obj" \
	"$(INTDIR)\renderd3d.obj" \
	"$(INTDIR)\RenderSoft.obj" \
	"$(INTDIR)\resource.res" \
	"$(INTDIR)\Resources.obj"

"$(OUTDIR)\C2.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "C2 - Win32 Debug_3DFX"

OUTDIR=.\DEBUG_FX
INTDIR=.\DEBUG_FX
# Begin Custom Macros
OutDir=.\DEBUG_FX
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\C2.exe"

!ELSE 

ALL : "$(OUTDIR)\C2.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Audio_DLL.obj"
	-@erase "$(INTDIR)\Characters.obj"
	-@erase "$(INTDIR)\Game.obj"
	-@erase "$(INTDIR)\Hunt2.obj"
	-@erase "$(INTDIR)\Interface.obj"
	-@erase "$(INTDIR)\mathematics.obj"
	-@erase "$(INTDIR)\Render3DFX.obj"
	-@erase "$(INTDIR)\renderd3d.obj"
	-@erase "$(INTDIR)\RenderSoft.obj"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\Resources.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\C2.exe"
	-@erase "$(OUTDIR)\C2.ilk"
	-@erase "$(OUTDIR)\C2.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_3dfx" /Fp"$(INTDIR)\C2.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\DEBUG_FX/
CPP_SBRS=.

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\resource.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\C2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=ole32.lib oleaut32.lib odbc32.lib odbccp32.lib kernel32.lib\
 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib\
 uuid.lib winmm.lib ddraw.lib dsound.lib wsock32.lib GLIDE2x.LIB SST1INIT.LIB\
 /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\C2.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)\C2.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\Audio_DLL.obj" \
	"$(INTDIR)\Characters.obj" \
	"$(INTDIR)\Game.obj" \
	"$(INTDIR)\Hunt2.obj" \
	"$(INTDIR)\Interface.obj" \
	"$(INTDIR)\mathematics.obj" \
	"$(INTDIR)\Render3DFX.obj" \
	"$(INTDIR)\renderd3d.obj" \
	"$(INTDIR)\RenderSoft.obj" \
	"$(INTDIR)\resource.res" \
	"$(INTDIR)\Resources.obj"

"$(OUTDIR)\C2.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "C2 - Win32 Debug_D3D"

OUTDIR=.\DEBUG_3D
INTDIR=.\DEBUG_3D
# Begin Custom Macros
OutDir=.\DEBUG_3D
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\C2.exe"

!ELSE 

ALL : "$(OUTDIR)\C2.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Audio_DLL.obj"
	-@erase "$(INTDIR)\Characters.obj"
	-@erase "$(INTDIR)\Game.obj"
	-@erase "$(INTDIR)\Hunt2.obj"
	-@erase "$(INTDIR)\Interface.obj"
	-@erase "$(INTDIR)\mathematics.obj"
	-@erase "$(INTDIR)\Render3DFX.obj"
	-@erase "$(INTDIR)\renderd3d.obj"
	-@erase "$(INTDIR)\RenderSoft.obj"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\Resources.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\C2.exe"
	-@erase "$(OUTDIR)\C2.ilk"
	-@erase "$(OUTDIR)\C2.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_d3d" /Fp"$(INTDIR)\C2.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\DEBUG_3D/
CPP_SBRS=.

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\resource.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\C2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=ole32.lib oleaut32.lib odbc32.lib odbccp32.lib kernel32.lib\
 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib\
 uuid.lib winmm.lib ddraw.lib dsound.lib wsock32.lib GLIDE2x.LIB SST1INIT.LIB\
 /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\C2.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)\C2.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\Audio_DLL.obj" \
	"$(INTDIR)\Characters.obj" \
	"$(INTDIR)\Game.obj" \
	"$(INTDIR)\Hunt2.obj" \
	"$(INTDIR)\Interface.obj" \
	"$(INTDIR)\mathematics.obj" \
	"$(INTDIR)\Render3DFX.obj" \
	"$(INTDIR)\renderd3d.obj" \
	"$(INTDIR)\RenderSoft.obj" \
	"$(INTDIR)\resource.res" \
	"$(INTDIR)\Resources.obj"

"$(OUTDIR)\C2.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "C2 - Win32 Release_Soft"

OUTDIR=.\REL_SOFT
INTDIR=.\REL_SOFT
# Begin Custom Macros
OutDir=.\REL_SOFT
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\v_soft.ren"

!ELSE 

ALL : "$(OUTDIR)\v_soft.ren"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Audio_DLL.obj"
	-@erase "$(INTDIR)\Characters.obj"
	-@erase "$(INTDIR)\Game.obj"
	-@erase "$(INTDIR)\Hunt2.obj"
	-@erase "$(INTDIR)\Interface.obj"
	-@erase "$(INTDIR)\mathematics.obj"
	-@erase "$(INTDIR)\Render3DFX.obj"
	-@erase "$(INTDIR)\renderd3d.obj"
	-@erase "$(INTDIR)\RenderSoft.obj"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\Resources.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\v_soft.ilk"
	-@erase "$(OUTDIR)\v_soft.ren"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /ML /W3 /Gi /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_soft" /Fp"$(INTDIR)\C2.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\REL_SOFT/
CPP_SBRS=.

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\resource.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\C2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=ole32.lib oleaut32.lib odbc32.lib odbccp32.lib kernel32.lib\
 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib\
 uuid.lib winmm.lib ddraw.lib dsound.lib wsock32.lib GLIDE2x.LIB SST1INIT.LIB\
 /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\v_soft.pdb"\
 /machine:I386 /out:"$(OUTDIR)\v_soft.ren" 
LINK32_OBJS= \
	"$(INTDIR)\Audio_DLL.obj" \
	"$(INTDIR)\Characters.obj" \
	"$(INTDIR)\Game.obj" \
	"$(INTDIR)\Hunt2.obj" \
	"$(INTDIR)\Interface.obj" \
	"$(INTDIR)\mathematics.obj" \
	"$(INTDIR)\Render3DFX.obj" \
	"$(INTDIR)\renderd3d.obj" \
	"$(INTDIR)\RenderSoft.obj" \
	"$(INTDIR)\resource.res" \
	"$(INTDIR)\Resources.obj"

"$(OUTDIR)\v_soft.ren" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "C2 - Win32 Release_3DFX"

OUTDIR=.\REL_3DFX
INTDIR=.\REL_3DFX
# Begin Custom Macros
OutDir=.\REL_3DFX
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\v_3dfx.ren"

!ELSE 

ALL : "$(OUTDIR)\v_3dfx.ren"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Audio_DLL.obj"
	-@erase "$(INTDIR)\Characters.obj"
	-@erase "$(INTDIR)\Game.obj"
	-@erase "$(INTDIR)\Hunt2.obj"
	-@erase "$(INTDIR)\Interface.obj"
	-@erase "$(INTDIR)\mathematics.obj"
	-@erase "$(INTDIR)\Render3DFX.obj"
	-@erase "$(INTDIR)\renderd3d.obj"
	-@erase "$(INTDIR)\RenderSoft.obj"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\Resources.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\v_3dfx.ilk"
	-@erase "$(OUTDIR)\v_3dfx.ren"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /ML /W3 /Gi /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_3dfx" /Fp"$(INTDIR)\C2.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\REL_3DFX/
CPP_SBRS=.

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\resource.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\C2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=ole32.lib oleaut32.lib odbc32.lib odbccp32.lib kernel32.lib\
 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib\
 uuid.lib winmm.lib ddraw.lib dsound.lib wsock32.lib GLIDE2x.LIB SST1INIT.LIB\
 /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\v_3dfx.pdb"\
 /machine:I386 /out:"$(OUTDIR)\v_3dfx.ren" 
LINK32_OBJS= \
	"$(INTDIR)\Audio_DLL.obj" \
	"$(INTDIR)\Characters.obj" \
	"$(INTDIR)\Game.obj" \
	"$(INTDIR)\Hunt2.obj" \
	"$(INTDIR)\Interface.obj" \
	"$(INTDIR)\mathematics.obj" \
	"$(INTDIR)\Render3DFX.obj" \
	"$(INTDIR)\renderd3d.obj" \
	"$(INTDIR)\RenderSoft.obj" \
	"$(INTDIR)\resource.res" \
	"$(INTDIR)\Resources.obj"

"$(OUTDIR)\v_3dfx.ren" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "C2 - Win32 Release_D3D"

OUTDIR=.\REL_D3D
INTDIR=.\REL_D3D
# Begin Custom Macros
OutDir=.\REL_D3D
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\v_d3d.ren"

!ELSE 

ALL : "$(OUTDIR)\v_d3d.ren"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Audio_DLL.obj"
	-@erase "$(INTDIR)\Characters.obj"
	-@erase "$(INTDIR)\Game.obj"
	-@erase "$(INTDIR)\Hunt2.obj"
	-@erase "$(INTDIR)\Interface.obj"
	-@erase "$(INTDIR)\mathematics.obj"
	-@erase "$(INTDIR)\Render3DFX.obj"
	-@erase "$(INTDIR)\renderd3d.obj"
	-@erase "$(INTDIR)\RenderSoft.obj"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\Resources.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\v_d3d.ilk"
	-@erase "$(OUTDIR)\v_d3d.ren"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /ML /W3 /Gi /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_d3d" /Fp"$(INTDIR)\C2.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\REL_D3D/
CPP_SBRS=.

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\resource.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\C2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=ole32.lib oleaut32.lib odbc32.lib odbccp32.lib kernel32.lib\
 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib\
 uuid.lib winmm.lib ddraw.lib dsound.lib wsock32.lib GLIDE2x.LIB SST1INIT.LIB\
 /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\v_d3d.pdb"\
 /machine:I386 /out:"$(OUTDIR)\v_d3d.ren" 
LINK32_OBJS= \
	"$(INTDIR)\Audio_DLL.obj" \
	"$(INTDIR)\Characters.obj" \
	"$(INTDIR)\Game.obj" \
	"$(INTDIR)\Hunt2.obj" \
	"$(INTDIR)\Interface.obj" \
	"$(INTDIR)\mathematics.obj" \
	"$(INTDIR)\Render3DFX.obj" \
	"$(INTDIR)\renderd3d.obj" \
	"$(INTDIR)\RenderSoft.obj" \
	"$(INTDIR)\resource.res" \
	"$(INTDIR)\Resources.obj"

"$(OUTDIR)\v_d3d.ren" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "C2 - Win32 Debug_Soft" || "$(CFG)" == "C2 - Win32 Debug_3DFX"\
 || "$(CFG)" == "C2 - Win32 Debug_D3D" || "$(CFG)" == "C2 - Win32 Release_Soft"\
 || "$(CFG)" == "C2 - Win32 Release_3DFX" || "$(CFG)" ==\
 "C2 - Win32 Release_D3D"
SOURCE=.\Audio_DLL.cpp

"$(INTDIR)\Audio_DLL.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Characters.cpp
DEP_CPP_CHARA=\
	".\Hunt.h"\
	

"$(INTDIR)\Characters.obj" : $(SOURCE) $(DEP_CPP_CHARA) "$(INTDIR)"


SOURCE=.\Game.cpp
DEP_CPP_GAME_=\
	".\Hunt.h"\
	

"$(INTDIR)\Game.obj" : $(SOURCE) $(DEP_CPP_GAME_) "$(INTDIR)"


SOURCE=.\Hunt2.cpp
DEP_CPP_HUNT2=\
	".\Hunt.h"\
	

"$(INTDIR)\Hunt2.obj" : $(SOURCE) $(DEP_CPP_HUNT2) "$(INTDIR)"


SOURCE=.\Interface.cpp
DEP_CPP_INTER=\
	".\Hunt.h"\
	

"$(INTDIR)\Interface.obj" : $(SOURCE) $(DEP_CPP_INTER) "$(INTDIR)"


SOURCE=.\mathematics.cpp
DEP_CPP_MATHE=\
	".\Hunt.h"\
	

"$(INTDIR)\mathematics.obj" : $(SOURCE) $(DEP_CPP_MATHE) "$(INTDIR)"


SOURCE=.\Render3DFX.cpp

!IF  "$(CFG)" == "C2 - Win32 Debug_Soft"


"$(INTDIR)\Render3DFX.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "C2 - Win32 Debug_3DFX"

DEP_CPP_RENDE=\
	"..\..\vc\include\3dfx.h"\
	"..\..\vc\include\glide.h"\
	"..\..\vc\include\glidesys.h"\
	"..\..\vc\include\glideutl.h"\
	"..\..\vc\include\sst1vid.h"\
	".\Hunt.h"\
	

"$(INTDIR)\Render3DFX.obj" : $(SOURCE) $(DEP_CPP_RENDE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "C2 - Win32 Debug_D3D"


"$(INTDIR)\Render3DFX.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "C2 - Win32 Release_Soft"


"$(INTDIR)\Render3DFX.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "C2 - Win32 Release_3DFX"

DEP_CPP_RENDE=\
	"..\..\vc\include\3dfx.h"\
	"..\..\vc\include\glide.h"\
	"..\..\vc\include\glidesys.h"\
	"..\..\vc\include\glideutl.h"\
	"..\..\vc\include\sst1vid.h"\
	".\Hunt.h"\
	

"$(INTDIR)\Render3DFX.obj" : $(SOURCE) $(DEP_CPP_RENDE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "C2 - Win32 Release_D3D"


"$(INTDIR)\Render3DFX.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\renderd3d.cpp

!IF  "$(CFG)" == "C2 - Win32 Debug_Soft"


"$(INTDIR)\renderd3d.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "C2 - Win32 Debug_3DFX"


"$(INTDIR)\renderd3d.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "C2 - Win32 Debug_D3D"

DEP_CPP_RENDER=\
	".\Hunt.h"\
	

"$(INTDIR)\renderd3d.obj" : $(SOURCE) $(DEP_CPP_RENDER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "C2 - Win32 Release_Soft"


"$(INTDIR)\renderd3d.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "C2 - Win32 Release_3DFX"


"$(INTDIR)\renderd3d.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "C2 - Win32 Release_D3D"

DEP_CPP_RENDER=\
	".\Hunt.h"\
	

"$(INTDIR)\renderd3d.obj" : $(SOURCE) $(DEP_CPP_RENDER) "$(INTDIR)"


!ENDIF 

SOURCE=.\RenderSoft.cpp

!IF  "$(CFG)" == "C2 - Win32 Debug_Soft"

DEP_CPP_RENDERS=\
	".\Hunt.h"\
	".\renderasm.cpp"\
	

"$(INTDIR)\RenderSoft.obj" : $(SOURCE) $(DEP_CPP_RENDERS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "C2 - Win32 Debug_3DFX"


"$(INTDIR)\RenderSoft.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "C2 - Win32 Debug_D3D"


"$(INTDIR)\RenderSoft.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "C2 - Win32 Release_Soft"

DEP_CPP_RENDERS=\
	".\Hunt.h"\
	".\renderasm.cpp"\
	

"$(INTDIR)\RenderSoft.obj" : $(SOURCE) $(DEP_CPP_RENDERS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "C2 - Win32 Release_3DFX"


"$(INTDIR)\RenderSoft.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "C2 - Win32 Release_D3D"


"$(INTDIR)\RenderSoft.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\resource.rc
DEP_RSC_RESOU=\
	".\action.ico"\
	

"$(INTDIR)\resource.res" : $(SOURCE) $(DEP_RSC_RESOU) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\Resources.cpp
DEP_CPP_RESOUR=\
	".\Hunt.h"\
	

"$(INTDIR)\Resources.obj" : $(SOURCE) $(DEP_CPP_RESOUR) "$(INTDIR)"



!ENDIF 

