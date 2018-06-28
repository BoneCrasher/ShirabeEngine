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
	SET OUT=%CURRENTDIR%\..\..\json\%2

	echo Current Directory %CURRENTDIR%
	
	md %CURRENTDIR%\build\%2
	cd %CURRENTDIR%\build\%2
	
	echo Building for %2...
	
	cmake -DCMAKE_BUILD_TYPE=%1 -DCMAKE_INSTALL_PREFIX=%OUT% %CURRENTDIR%/json_master
	cmake --build . 
	cmake --build . --target INSTALL
	
	cd %CURRENTDIR%
GOTO :eof

:buildWinThirtyTwo
    set ORIGINAL=%cd%
	CALL "%PROGRAMFILES(x86)%\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
	cd /D %ORIGINAL%
	GOTO :eof

:buildWinSixtyFour
    set ORIGINAL=%cd%
	CALL "%PROGRAMFILES(x86)%\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
    cd /D %ORIGINAL%
	GOTO :eof
