# Microsoft Developer Studio Project File - Name="winsimpleraytracer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=winsimpleraytracer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "winsimpleraytracer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "winsimpleraytracer.mak" CFG="winsimpleraytracer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "winsimpleraytracer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "winsimpleraytracer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "winsimpleraytracer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1409 /d "NDEBUG"
# ADD RSC /l 0x1409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "winsimpleraytracer - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "winsimpleraytracer___Win32_Debug"
# PROP BASE Intermediate_Dir "winsimpleraytracer___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "winsimpleraytracer___Win32_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1409 /d "_DEBUG"
# ADD RSC /l 0x1409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "winsimpleraytracer - Win32 Release"
# Name "winsimpleraytracer - Win32 Debug"
# Begin Group "raytracer"

# PROP Default_Filter ""
# Begin Group "Geometry"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\geometry.h
# End Source File
# Begin Source File

SOURCE=.\rayplane.cpp
# End Source File
# Begin Source File

SOURCE=.\rayplane.h
# End Source File
# Begin Source File

SOURCE=.\raysphere.cpp
# End Source File
# Begin Source File

SOURCE=.\raysphere.h
# End Source File
# End Group
# Begin Group "Material"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\material.cpp
# End Source File
# Begin Source File

SOURCE=.\material.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\colour.h
# End Source File
# Begin Source File

SOURCE=.\light.h
# End Source File
# Begin Source File

SOURCE=.\object.cpp
# End Source File
# Begin Source File

SOURCE=.\object.h
# End Source File
# Begin Source File

SOURCE=.\ray.h
# End Source File
# Begin Source File

SOURCE=.\world.cpp
# End Source File
# Begin Source File

SOURCE=.\world.h
# End Source File
# End Group
# Begin Group "simplewin2d"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\simplewin2d\simplewin2d.cpp
# End Source File
# Begin Source File

SOURCE=..\simplewin2d\simplewin2d.h
# End Source File
# Begin Source File

SOURCE=..\simplewin2d\simwin_framework.cpp
# End Source File
# Begin Source File

SOURCE=..\simplewin2d\simwin_framework.h
# End Source File
# End Group
# Begin Group "stub"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# End Group
# Begin Group "graphics2d"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\graphics2d\graphics2d.cpp
# End Source File
# Begin Source File

SOURCE=..\graphics2d\graphics2d.h
# End Source File
# End Group
# Begin Group "utils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\utils\clock.cpp
# End Source File
# Begin Source File

SOURCE=..\utils\clock.h
# End Source File
# Begin Source File

SOURCE=..\utils\timer.h
# End Source File
# End Group
# Begin Group "maths"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\maths\mathstypes.h
# End Source File
# Begin Source File

SOURCE=..\maths\plane.h
# End Source File
# Begin Source File

SOURCE=..\maths\plane2.h
# End Source File
# Begin Source File

SOURCE=..\maths\vec2.h
# End Source File
# Begin Source File

SOURCE=..\maths\vec3.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\readme.txt
# End Source File
# End Target
# End Project
