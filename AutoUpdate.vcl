<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: AutoUpdate - Win32 (WCE ARMV4I) Debug--------------------
</h3>
<h3>Command Lines</h3>
Creating command line "rc.exe /l 0x409 /fo"ARMV4IDbg/AutoUpdate.res" /d UNDER_CE=500 /d _WIN32_WCE=500 /d "UNICODE" /d "_UNICODE" /d "DEBUG" /d "WCE_PLATFORM_MI320C" /d "THUMB" /d "_THUMB_" /d "ARM" /d "_ARM_" /d "ARMV4I" /r "D:\[프로젝트]\AutoUpdate\AutoUpdate\AutoUpdate.rc"" 
Creating temporary file "C:\DOCUME~1\eMbedded\LOCALS~1\Temp\RSP351.tmp" with contents
[
/nologo /W3 /Zi /Od /D "DEBUG" /D "ARM" /D "_ARM_" /D "ARMV4I" /D UNDER_CE=500 /D _WIN32_WCE=500 /D "WCE_PLATFORM_MI320C" /D "UNICODE" /D "_UNICODE" /FR"ARMV4IDbg/" /Fp"ARMV4IDbg/AutoUpdate.pch" /Yu"stdafx.h" /Fo"ARMV4IDbg/" /Fd"ARMV4IDbg/" /QRarch4T /QRinterwork-return /MC /c 
"D:\[프로젝트]\AutoUpdate\AutoUpdate\AutoUpdate.cpp"
"D:\[프로젝트]\AutoUpdate\AutoUpdate\Connect.cpp"
]
Creating command line "clarm.exe @C:\DOCUME~1\eMbedded\LOCALS~1\Temp\RSP351.tmp" 
Creating temporary file "C:\DOCUME~1\eMbedded\LOCALS~1\Temp\RSP352.tmp" with contents
[
/nologo /W3 /Zi /Od /D "DEBUG" /D "ARM" /D "_ARM_" /D "ARMV4I" /D UNDER_CE=500 /D _WIN32_WCE=500 /D "WCE_PLATFORM_MI320C" /D "UNICODE" /D "_UNICODE" /FR"ARMV4IDbg/" /Fp"ARMV4IDbg/AutoUpdate.pch" /Yc"stdafx.h" /Fo"ARMV4IDbg/" /Fd"ARMV4IDbg/" /QRarch4T /QRinterwork-return /MC /c 
"D:\[프로젝트]\AutoUpdate\AutoUpdate\StdAfx.cpp"
]
Creating command line "clarm.exe @C:\DOCUME~1\eMbedded\LOCALS~1\Temp\RSP352.tmp" 
Creating temporary file "C:\DOCUME~1\eMbedded\LOCALS~1\Temp\RSP353.tmp" with contents
[
commctrl.lib coredll.lib /nologo /base:"0x00010000" /stack:0x10000,0x1000 /entry:"WinMainCRTStartup" /incremental:yes /pdb:"ARMV4IDbg/AutoUpdate.pdb" /debug /nodefaultlib:"libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib" /out:"ARMV4IDbg/AutoUpdate.exe" /subsystem:windowsce,5.00 /MACHINE:THUMB 
".\ARMV4IDbg\AutoUpdate.obj"
".\ARMV4IDbg\Connect.obj"
".\ARMV4IDbg\StdAfx.obj"
".\ARMV4IDbg\AutoUpdate.res"
]
Creating command line "link.exe @C:\DOCUME~1\eMbedded\LOCALS~1\Temp\RSP353.tmp"
<h3>Output Window</h3>
Compiling resources...
Compiling...
StdAfx.cpp
Compiling...
AutoUpdate.cpp
Connect.cpp
Generating Code...
Linking...
corelibc.lib(pegwmain.obj) : warning LNK4209: debugging information corrupt; recompile module; linking object as if no debug info
corelibc.lib(crt0dat.obj) : warning LNK4209: debugging information corrupt; recompile module; linking object as if no debug info
corelibc.lib(crt0init.obj) : warning LNK4209: debugging information corrupt; recompile module; linking object as if no debug info
Creating command line "bscmake.exe /nologo /o"ARMV4IDbg/AutoUpdate.bsc"  ".\ARMV4IDbg\StdAfx.sbr" ".\ARMV4IDbg\AutoUpdate.sbr" ".\ARMV4IDbg\Connect.sbr""
Creating browse info file...
<h3>Output Window</h3>




<h3>Results</h3>
AutoUpdate.exe - 0 error(s), 3 warning(s)
</pre>
</body>
</html>
