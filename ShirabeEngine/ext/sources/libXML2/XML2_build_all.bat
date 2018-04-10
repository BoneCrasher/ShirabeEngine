@ECHO off

CALL :buildWinThirtyTwo

CALL :buildOne Debug   Win32\Debug
CALL :buildOne Release Win32\Release

CALL :buildWinSixtyFour

CALL :buildOne Debug   Win64\Debug
CALL :buildOne Release Win64\Release

GOTO :eof

:buildOne
	SET CURRENTDIR="%cd%"
	SET OUT=%CURRENTDIR%\deploy\%2%
	SET INCLUDE_PATHS=%CURRENTDIR%\..\libicu\deploy\%2\include
	SET LIB_PATHS=%CURRENTDIR%\..\libicu\deploy\%2\lib
	
	SET DBG=no
	SET CRT=/MT

	if "%1"=="Debug" (
		SET DBG=yes
	    SET CRT=/MTd
	)

	cd libxml2-2.9.7/win32
	
	echo Building for %2%...
	
    cscript.exe configure.js iconv=no                  ^
						     icu=no                    ^
							 compiler=msvc             ^
							 cruntime=%CRT%            ^
							 debug=%DBG%               ^
							 static=yes                ^
							 vcmanifest=yes            ^
							 prefix="%OUT%"            ^
							 bindir="%OUT%\bin"        ^
							 libdir="%OUT%\lib"        ^
							 incdir="%OUT%\include"    ^
							 sodir="%OUT%\bin"         ^
							 include="%INCLUDE_PATHS%" ^
							 lib="%LIB_PATHS%"
	nmake /f Makefile.msvc libxml install
	nmake clean
	
	cd %CURRENTDIR%
GOTO :eof

:buildWinThirtyTwo
	CALL "%PROGRAMFILES(x86)%\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
GOTO :eof

:buildWinSixtyFour
	CALL "%PROGRAMFILES(x86)%\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
GOTO :eof
