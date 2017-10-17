@echo off

:CountLines
setlocal
set /a totalNumLines = 0
for /r %1 %%F in (*.cpp *.h *.hpp *.cs *.c) do (
  for /f %%N in ('find /v /c "" ^<"%%F"') do set /a totalNumLines+=%%N
)

echo Total number of code lines for %1 = %totalNumLines% 
PAUSE