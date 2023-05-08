@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

mkdir %~dp0\build\web
cd /D %~dp0\build\web

call cmake --build intermediate --target clean
rmdir install /s /q