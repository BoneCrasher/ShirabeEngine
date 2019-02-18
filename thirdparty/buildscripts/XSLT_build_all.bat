@ECHO off

CALL :buildWinThirtyTwo

CALL :buildOne Debug   Win32\Debug
CALL :buildOne Release Win32\Release

CALL :buildWinSixtyFour

CALL :buildOne Debug   Win64\Debug
CALL :buildOne Release Win64\Release

GOTO :eof

:buildOne
	SET CURRENTDIR=%cd%
	SET OUT=%CURRENTDIR%\deploy\%2
	SET XML2_DIR=%CURRENTDIR%\..\libXML2\deploy\%2
	SET XML2_INCLUDEDIR=%XML2_DIR%\include
	SET XML2_LIBDIR=%XML2_DIR%\lib
	
	SET DBG=no
	SET CRT=/MT

	if "%1"=="Debug" (
		SET DBG=yes
	    SET CRT=/MTd
	)
	
	echo %XML2_INCLUDEDIR%
	echo %XML2_LIBDIR%

	cd libxslt-1.1.26\win32
	
	cscript.exe configure.js iconv=no                    ^
		                     compiler=msvc               ^
							 cruntime=%CRT%              ^
							 debug=%DBG%                 ^
							 static=yes                  ^
							 vcmanifest=yes              ^
							 prefix="%OUT%"              ^
							 bindir="%OUT%\bin"          ^
							 libdir="%OUT%\lib"          ^
							 incdir="%OUT%\include"      ^
							 sodir="%OUT%\bin"           ^
							 include="%XML2_INCLUDEDIR%" ^
							 lib="%XML2_LIBDIR%"     
	nmake /f Makefile.msvc libxslt install
	nmake clean
	cd "%CURRENTDIR%"
GOTO :eof

:buildWinThirtyTwo
	CALL "%VS100COMNTOOLS%\..\..\VC\vcvarsall.bat" x86
GOTO :eof

:buildWinSixtyFour
	CALL "%VS100COMNTOOLS%\..\..\VC\vcvarsall.bat" amd64
GOTO :eof