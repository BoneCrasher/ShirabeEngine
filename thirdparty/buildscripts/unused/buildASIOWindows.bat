echo @OFF

cd asio-1.10.6

set "THIS=%cd%"
:buildOne Win 32 Debug
:buildOne Win 32 Release
:buildOne Win 64 Debug
:buildOne Win 64 Release

:buildOne 
set "Platform=%1%"
set "AddressModel=%2%"
set "Config=%3%"


set "BOOSTDIR=%THIS%/../../../sr_3rdparty_binaries/boost/include"

cd src
nmake -f Makefile.msc 
cd ..

goto:eof