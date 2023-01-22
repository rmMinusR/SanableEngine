rem @echo off

cd /D %~dp0\emsdk
call emsdk_env

rem SET EMPP_ARGS=-sUSE_SDL=2 -sLLD_REPORT_UNDEFINED -Wparentheses
rem SET OUTPUT_BASE_DIR=%~dp0\builds\web

rem emconfigure cmake .
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

mkdir %~dp0\out\web
cd /D %~dp0\out\web

call emcmake cmake -DLDFLAGS="-sUSE_SDL=2" %~dp0
call ninja

pause