@echo off

cd /D %~dp0\..\emsdk
call emsdk_env

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

mkdir %~dp0\out\web
cd /D %~dp0\out\web

call emcmake cmake %~dp0
call ninja

pause