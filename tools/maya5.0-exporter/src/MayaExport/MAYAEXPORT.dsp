# Microsoft Developer Studio Project File - Name="MAYAEXPORT" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=MAYAEXPORT - Win32 DebugMaya30
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MAYAEXPORT.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MAYAEXPORT.mak" CFG="MAYAEXPORT - Win32 DebugMaya30"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MAYAEXPORT - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MAYAEXPORT - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MAYAEXPORT - Win32 ReleaseMaya30" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MAYAEXPORT - Win32 DebugMaya30" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MAYAEXPORT - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MAYAEXPORT_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "C:\AW\Maya4.0\include\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NT_ENV" /D "NT_PLUGIN" /D "MAYAEXPORT_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 OpenMaya.lib OpenMayaAnim.lib Image.lib Foundation.lib libMDtAPI.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"c:\AW\Maya4.0\bin\plug-ins\SD2GeoExport.mll" /libpath:"C:\AW\Maya4.0\lib\\" /export:initializePlugin /export:uninitializePlugin
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "MAYAEXPORT - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MAYAEXPORT_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "C:\AW\Maya4.0\include\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NT_ENV" /D "NT_PLUGIN" /D "MAYAEXPORT_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 OpenMaya.lib OpenMayaAnim.lib Image.lib Foundation.lib libMDtAPI.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"c:\AW\Maya4.0\bin\plug-ins\SD2GeoExport.mll" /pdbtype:sept /libpath:"C:\AW\Maya4.0\lib\\" /export:initializePlugin /export:uninitializePlugin
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "MAYAEXPORT - Win32 ReleaseMaya30"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MAYAEXPORT___Win32_ReleaseMaya30"
# PROP BASE Intermediate_Dir "MAYAEXPORT___Win32_ReleaseMaya30"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MAYAEXPORT___Win32_ReleaseMaya30"
# PROP Intermediate_Dir "MAYAEXPORT___Win32_ReleaseMaya30"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NT_ENV" /D "NT_PLUGIN" /D "MAYAEXPORT_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "C:\AW\Maya3.0\include\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NT_ENV" /D "NT_PLUGIN" /D "MAYAEXPORT_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 OpenMaya.lib OpenMayaAnim.lib Image.lib Foundation.lib libMDtAPI.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"c:\AW\Maya4.0\bin\plug-ins\AtlaExport.mll" /export:initializePlugin /export:uninitializePlugin
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 OpenMaya.lib OpenMayaAnim.lib Image.lib Foundation.lib libMDtAPI.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"c:\pub\mayaplug\SD2GeoMaya3.mll" /libpath:"C:\AW\Maya3.0\lib\\" /export:initializePlugin /export:uninitializePlugin
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "MAYAEXPORT - Win32 DebugMaya30"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MAYAEXPORT___Win32_DebugMaya30"
# PROP BASE Intermediate_Dir "MAYAEXPORT___Win32_DebugMaya30"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "MAYAEXPORT___Win32_DebugMaya30"
# PROP Intermediate_Dir "MAYAEXPORT___Win32_DebugMaya30"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "C:\AW\Maya4.0\include\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NT_ENV" /D "NT_PLUGIN" /D "MAYAEXPORT_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "C:\AW\Maya4.0\include\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NT_ENV" /D "NT_PLUGIN" /D "MAYAEXPORT_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 OpenMaya.lib OpenMayaAnim.lib Image.lib Foundation.lib libMDtAPI.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"c:\AW\Maya4.0\bin\plug-ins\AtlaExport.mll" /pdbtype:sept /libpath:"C:\AW\Maya4.0\lib\\" /export:initializePlugin /export:uninitializePlugin
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 OpenMaya.lib OpenMayaAnim.lib Image.lib Foundation.lib libMDtAPI.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"c:\AW\Maya3.0\bin\plug-ins\AtlaExportMaya3.mll" /pdbtype:sept /libpath:"C:\AW\Maya3.0\lib\\" /export:initializePlugin /export:uninitializePlugin
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "MAYAEXPORT - Win32 Release"
# Name "MAYAEXPORT - Win32 Debug"
# Name "MAYAEXPORT - Win32 ReleaseMaya30"
# Name "MAYAEXPORT - Win32 DebugMaya30"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AtlaExport.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AtlaExport.h
# End Source File
# Begin Source File

SOURCE=.\GEOM_LIB\rdf_exp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=..\..\AW\Maya3.0\scripts\others\AtlaExport.mel
# End Source File
# End Target
# End Project
