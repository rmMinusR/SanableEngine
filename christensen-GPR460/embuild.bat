@echo off

echo:
echo ======Setting Emscripten environment variables======
echo:

cd /D %~dp0\emsdk
call emsdk_env


echo:
echo ======Building project======
echo:

cd /D %~dp0\christensen-GPR460
mkdir ..\build\html\index
call em++ -Iinclude src\main.cpp -sUSE_SDL=2 -o ..\build\html\index.html

pause