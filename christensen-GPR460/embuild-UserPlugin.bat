@echo off

call embuild-env.bat

echo:
echo ======Building UserPlugin======
echo:

cd /D %~dp0\UserPlugin


::Gather plugin source files
SET srcs=
FOR /F "tokens=* USEBACKQ" %%F IN (`dir /b src\*.cpp`) DO (
CALL SET "srcs=%%srcs%% src\%%F"
)

SET objs=
FOR /F "tokens=* USEBACKQ" %%F IN (`dir /b %PLUGINS_FOLDER%\PrimitivesPlugin\Intermediate\*.o`) DO (
CALL SET "objs=%%objs%% %PLUGINS_FOLDER%\PrimitivesPlugin\Intermediate\%%F"
)

echo: Gathered files:
echo: %srcs%
echo: %objs%
echo:

::Make build
mkdir %PLUGINS_FOLDER%\UserPlugin
call em++ -Iinclude -I..\PublicShared\public -I..\PrimitivesPlugin\public %srcs% %objs% %SHARED_LIB% -sSIDE_MODULE %EMPP_ARGS% -o %PLUGINS_FOLDER%\UserPlugin\UserPlugin.so
