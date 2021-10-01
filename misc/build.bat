@echo off

set NAME=bbk
set NOCRT=-nodefaultlib kernel32.lib

set DebugFlags=-Od -MTd -Z7
set ReleaseFlags=-O2 -MT -Zi
set CompilerFlags=%ReleaseFlags% -c -nologo -TC -std:c11 -GS- -Gs9999999 -Gm- -GR- -Gr -EHa- -Oi -W4 -wd4100 -wd4189 -wd4201 -wd4505

set DebugFlags=-debug -pdb:msvc.pdb -map:msvc.map -subsystem:console
set ReleaseFlags=-subsystem:windows -incremental:no
set LinkerFlags=%ReleaseFlags% -nologo -stack:0x100000,0x100000 -out:%NAME%.exe -opt:ref %NOCRT% user32.lib gdi32.lib Ws2_32.lib data.res

cd %~dp0
cd ..

IF NOT EXIST build mkdir build
pushd build

rc -nologo -fo data.res ..\data\data.rc

REM COMPILE
cl %CompilerFlags% ..\code\main.c /I..\include
link %LinkerFlags% main.obj

popd
