@echo off
setlocal

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" || exit /b 1

set "Qt6_DIR=C:\Qt\6.8.0\msvc2022_64\lib\cmake\Qt6"
set "CMAKE_PREFIX_PATH=C:\Qt\6.8.0\msvc2022_64"

if "%~1"=="" (
    cmd
    exit /b %errorlevel%
)

%*
exit /b %errorlevel%
