@echo off

call embuild-common.bat

echo:
echo ======Building main project======
echo:

cd /D %~dp0\christensen-GPR460


::Gather main project source files
SET srcs=
FOR /F "tokens=* USEBACKQ" %%F IN (`dir /b src\*.cpp`) DO (
CALL SET "srcs=%%srcs%% src\%%F"
)
FOR /F "tokens=* USEBACKQ" %%F IN (`dir /b src\emscripten\*.cpp`) DO (
CALL SET "srcs=%%srcs%% src\emscripten\%%F"
)
FOR /F "tokens=* USEBACKQ" %%F IN (`dir /b ..\PluginShared\src\*.cpp`) DO (
CALL SET "srcs=%%srcs%% ..\PluginShared\src\%%F"
)
echo: Gathered files:
echo: %srcs%
echo:

::Make build
mkdir ..\builds\web
call em++ -Iinclude -I..\PluginShared\include%srcs% -sMAIN_MODULE %EMPP_ARGS% --use-preload-plugins --preload-file ..\builds\web\plugins@\plugins -o ..\builds\web\index.html


pause
