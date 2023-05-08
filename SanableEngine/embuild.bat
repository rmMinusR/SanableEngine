@echo off

cd /D %~dp0\..\emsdk
call emsdk_env

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

mkdir %~dp0\build\web
cd /D %~dp0\build\web

rem call emcmake cmake -S %~dp0 -B intermediate -DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_VERBOSE_MAKEFILE=ON

call emcmake cmake -S %~dp0 -B intermediate -DCMAKE_BUILD_TYPE=DEBUG
call cmake --build intermediate
call cmake --install intermediate --prefix install

pause