@ECHO off

cd assimp
if exist build (
    del /F /Q /S build 
)

if exist deploy (
    del /F /Q /S deploy
)

CALL :buildOne Win32 Debug
CALL :buildOne Win32 Release
CALL :buildOne Win64 Debug   " Win64"
CALL :buildOne Win64 Release " Win64"

cd ..

goto :eof

:buildOne
SET PLATFORM=%1%
SET CONFIG=%2%
SET X64FLAG=%3%
mkdir build\%PLATFORM%\%CONFIG% 
mkdir deploy\%PLATFORM%\%CONFIG%
cd    build\%PLATFORM%\%CONFIG%
cmake -G "Visual Studio 15 2017%X64FLAG%"        ^
      -DCMAKE_BUILD_TYPE=%CONFIG%                 ^
  	  -DCMAKE_INSTALL_PREFIX=..\..\..\deploy\%PLATFORM%\%CONFIG% ^
	  ..\..\..
cmake --build .
cmake --build . --target INSTALL
cd ..\..\..
goto :eof