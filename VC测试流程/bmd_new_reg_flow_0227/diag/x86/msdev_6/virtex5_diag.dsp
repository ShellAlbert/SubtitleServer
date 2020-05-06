# Microsoft Developer Studio Project File - Name="virtex5_diag" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=virtex5_diag - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "virtex5_diag.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "virtex5_diag.mak" CFG="virtex5_diag - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "virtex5_diag - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "virtex5_diag - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "virtex5_diag - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /O2 /D "i386" /D "WD_DRIVER_NAME_CHANGE" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /FD /EHsc /c
# ADD CPP /nologo /MT /W3 /O2 /I "../../../../../..//include" /I "../../../../../../" /D "i386" /D "WD_DRIVER_NAME_CHANGE" /D "NDEBUG" /D "WIN32" /D "_MT" /D "_CONSOLE" /D "_MBCS" /FD /EHsc /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wdapi1000.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /pdbtype:sept /libpath:"..\..\..\..\..\..\lib\x86\\"
# ADD LINK32 wdapi1000.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /pdbtype:sept /libpath:"..\..\..\..\..\..\lib\x86\\"

!ELSEIF  "$(CFG)" == "virtex5_diag - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /Zi /Od /D "i386" /D "WD_DRIVER_NAME_CHANGE" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /FD /EHsc /c
# ADD CPP /nologo /MTd /W3 /Gm /Zi /Od /I "../../../../../..//include" /I "../../../../../../" /D "i386" /D "WD_DRIVER_NAME_CHANGE" /D "_DEBUG" /D "WIN32" /D "_MT" /D "_CONSOLE" /D "_MBCS" /FR /FD /EHsc /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wdapi1000.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"..\..\..\..\..\..\lib\x86\\"
# ADD LINK32 wdapi1000.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"..\..\..\..\..\..\lib\x86\\"

!ENDIF 

# Begin Target

# Name "virtex5_diag - Win32 Release"
# Name "virtex5_diag - Win32 Debug"
# Begin Source File

SOURCE=..\..\../../../../samples/shared/diag_lib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\para.h
# End Source File
# Begin Source File

SOURCE=..\..\../../../../samples/shared/pci_diag_lib.c
# End Source File
# Begin Source File

SOURCE=..\..\../../../../samples/shared/print_struct.c
# End Source File
# Begin Source File

SOURCE=..\..\virtex5_diag.c
# End Source File
# Begin Source File

SOURCE=..\..\../virtex5_lib.c
# End Source File
# Begin Source File

SOURCE=..\..\../../../../samples/shared/wdc_diag_lib.c
# End Source File
# End Target
# End Project
