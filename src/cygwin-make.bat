@echo off
setlocal

set CYGWIN_PATH=C:\cygwin

set TOP=%CYGWIN_PATH:\=/%
set TMP=%CYGWIN_PATH%\cygwin_tmp
set PATH=%CYGWIN_PATH%\bin
set HOME=%CD%

IF EXIST %CYGWIN_PATH%"\bin\bash.exe" set SHELL=/bin/bash

echo command:%*
%CYGWIN_PATH%"\bin\bash.exe" --login -c "%*"

endlocal

