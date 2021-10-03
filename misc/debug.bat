@echo off

cd %~dp0
cd ..

pushd build
devenv bbk.exe
popd build