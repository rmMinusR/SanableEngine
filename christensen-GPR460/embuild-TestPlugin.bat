@echo off

call embuild-common.bat

echo:
echo ======Building TestPlugin======
echo:

cd /D %~dp0\TestPlugin


::Gather plugin source files
SET srcs=
FOR /F "tokens=* USEBACKQ" %%F IN (`dir /b src\*.cpp`) DO (
CALL SET "srcs=%%srcs%% src\%%F"
)

echo: Gathered files:
echo: %srcs%
echo:

::Make build
mkdir ..\builds\web\plugins\TestPlugin
call em++ -Iinclude -I..\PluginShared\include%srcs% -sSIDE_MODULE %EMPP_ARGS% -o ..\builds\web\plugins\TestPlugin\TestPlugin.so


pause
