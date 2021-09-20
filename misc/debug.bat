@echo off

cd %~dp0
cd ..

pushd build
devenv bergbaukunst.exe
popd build