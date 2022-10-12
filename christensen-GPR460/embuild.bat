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


::Gather source files
SET srcs=
FOR /F "tokens=* USEBACKQ" %%F IN (`dir /b src\*.cpp`) DO (
CALL SET "srcs=%%srcs%% src\%%F"
)
FOR /F "tokens=* USEBACKQ" %%F IN (`dir /b src\emscripten\*.cpp`) DO (
CALL SET "srcs=%%srcs%% src\emscripten\%%F"
)
echo: Gathered files:
echo: %srcs%
echo:


::Make build
mkdir ..\builds\web
call em++ -Iinclude%srcs% -sUSE_SDL=2 -o ..\builds\web\index.html


pause