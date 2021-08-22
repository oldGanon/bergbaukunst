@echo off

REM MSVC
set VSWHERE="C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere"
for /f "delims=" %%i in ('%VSWHERE% -latest -property installationPath') do (
    set VS000COMNTOOLS=%%i\Common7\Tools\
)
for /f "delims=" %%i in ('%VSWHERE% -latest -property displayName') do (
    set VS000COMNTITLE=%%i
)

if defined VS000COMNTOOLS (
    echo Using %VS000COMNTITLE%
    call "%VS000COMNTOOLS%VsDevCmd.bat" -no_logo -arch=amd64 -host_arch=amd64
) else if defined VS140COMNTOOLS (
    echo Using Visual Studio 2015
    call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64
) else (
    echo Could not find Visual Studio.
    goto :end
)

echo.

REM SCRIPTS
set path=%~dp0;%path%
