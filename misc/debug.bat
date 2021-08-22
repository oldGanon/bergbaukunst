@echo off

cd %~dp0
cd ..

pushd build
devenv msvc.exe
popd build