# Microsoft Developer Studio Project File - Name="PCSpim" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=PCSpim - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pcspim.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pcspim.mak" CFG="PCSpim - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PCSpim - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "PCSpim - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Software/SPIM/PCSpim/pcspim", UQAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PCSpim - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Os /D "_DEBUG" /D "SPIM_LITTLEENDIAN" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "PCSpim - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "PCSpim___Win32_Release"
# PROP BASE Intermediate_Dir "PCSpim___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Os /D "_DEBUG" /D "SPIM_LITTLEENDIAN" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "__STDC__" /FR /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /D "SPIM_LITTLEENDIAN" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "__STDC__" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 /nologo /subsystem:windows /incremental:no /debug /machine:I386
# SUBTRACT LINK32 /profile

!ENDIF 

# Begin Target

# Name "PCSpim - Win32 Debug"
# Name "PCSpim - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\BreakpointDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ConsoleWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\src\data.c
# End Source File
# Begin Source File

SOURCE="..\src\display-utils.c"
# End Source File
# Begin Source File

SOURCE=..\src\inst.c
# End Source File
# Begin Source File

SOURCE=..\src\lex.yy.c
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=..\src\mem.c
# End Source File
# Begin Source File

SOURCE=.\MultiStepDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PCSpim.cpp
# End Source File
# Begin Source File

SOURCE=.\hlp\PCSpim.hpj

!IF  "$(CFG)" == "PCSpim - Win32 Debug"

# Begin Custom Build - Making help file...
OutDir=.\Debug
ProjDir=.
TargetName=pcspim
InputPath=.\hlp\PCSpim.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\makehelp.bat $(OutDir)

# End Custom Build

!ELSEIF  "$(CFG)" == "PCSpim - Win32 Release"

# Begin Custom Build - Making help file...
OutDir=.\Release
ProjDir=.
TargetName=pcspim
InputPath=.\hlp\PCSpim.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\makehelp.bat $(OutDir)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PCSpim.rc
# End Source File
# Begin Source File

SOURCE=.\PCSpimDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\PCSpimView.cpp
# End Source File
# Begin Source File

SOURCE=.\Regman.cpp
# End Source File
# Begin Source File

SOURCE=..\src\run.c
# End Source File
# Begin Source File

SOURCE=.\RunDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetValueDlg.cpp
# End Source File
# Begin Source File

SOURCE="..\src\spim-utils.c"
# End Source File
# Begin Source File

SOURCE=.\SpimSupport.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE="..\src\string-stream.c"
# End Source File
# Begin Source File

SOURCE="..\src\sym-tbl.c"
# End Source File
# Begin Source File

SOURCE=..\src\syscall.c
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# Begin Source File

SOURCE=..\src\y.tab.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\BreakpointDlg.h
# End Source File
# Begin Source File

SOURCE=.\ConsoleWnd.h
# End Source File
# Begin Source File

SOURCE=..\src\data.h
# End Source File
# Begin Source File

SOURCE=..\src\inst.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=..\src\mem.h
# End Source File
# Begin Source File

SOURCE=.\MultiStepDlg.h
# End Source File
# Begin Source File

SOURCE=.\PCSpim.h
# End Source File
# Begin Source File

SOURCE=.\PCSpimDoc.h
# End Source File
# Begin Source File

SOURCE=.\PCSpimView.h
# End Source File
# Begin Source File

SOURCE=.\Regman.h
# End Source File
# Begin Source File

SOURCE=..\src\run.h
# End Source File
# Begin Source File

SOURCE=.\RunDlg.h
# End Source File
# Begin Source File

SOURCE=.\SettingsDlg.h
# End Source File
# Begin Source File

SOURCE=.\SetValueDlg.h
# End Source File
# Begin Source File

SOURCE="..\src\spim-utils.h"
# End Source File
# Begin Source File

SOURCE=.\SpimReg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE="..\src\string-stream.h"
# End Source File
# Begin Source File

SOURCE="..\src\sym-tbl.h"
# End Source File
# Begin Source File

SOURCE=..\src\syscall.h
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# Begin Source File

SOURCE=..\src\y.tab.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\document.ico
# End Source File
# Begin Source File

SOURCE=.\res\PCSpim.ico
# End Source File
# Begin Source File

SOURCE=.\res\PCSpim.rc2
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=..\src\parser.y

!IF  "$(CFG)" == "PCSpim - Win32 Debug"

# Begin Custom Build
InputDir=\Larus\Software\SPIM\src
InputPath=..\src\parser.y
InputName=parser

BuildCmds= \
	cd $(InputDir) \
	"C:\Program Files\cygwin\bin\bison"  -d -y $(InputName).y \
	

"$(InputDir)\y.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\y.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "PCSpim - Win32 Release"

# Begin Custom Build
InputDir=\Larus\Software\SPIM\src
InputPath=..\src\parser.y
InputName=parser

BuildCmds= \
	cd $(InputDir) \
	"C:\Program Files\cygwin\bin\bison"  -d -y $(InputName).y \
	

"$(InputDir)\y.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\y.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\scanner.l

!IF  "$(CFG)" == "PCSpim - Win32 Debug"

# Begin Custom Build
InputDir=\Larus\Software\SPIM\src
InputPath=..\src\scanner.l
InputName=scanner

"$(InputDir)\lex.yy.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd $(InputDir) 
	"C:\Program Files\cygwin\bin\flex" -I -8 $(InputName).l 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "PCSpim - Win32 Release"

# Begin Custom Build
InputDir=\Larus\Software\SPIM\src
InputPath=..\src\scanner.l
InputName=scanner

"$(InputDir)\lex.yy.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd $(InputDir) 
	"C:\Program Files\cygwin\bin\flex" -I -8 $(InputName).l 
	
# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
