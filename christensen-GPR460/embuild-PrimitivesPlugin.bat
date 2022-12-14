@echo off

call embuild-env.bat

echo:
echo ======Building PrimitivesPlugin======
echo:

cd /D %~dp0\PrimitivesPlugin


::Gather plugin source files
SET srcs=
FOR /F "tokens=* USEBACKQ" %%F IN (`dir /b %~dp0\PrimitivesPlugin\src\*.cpp`) DO (
CALL SET "srcs=%%srcs%% %~dp0\PrimitivesPlugin\src\%%F"
)

echo: Gathered files:
echo: %srcs%
echo:

::Make build
mkdir %PLUGINS_FOLDER%\PrimitivesPlugin\Intermediate
cd /D %PLUGINS_FOLDER%\PrimitivesPlugin\Intermediate
call em++ -I%~dp0\PrimitivesPlugin\public -I%~dp0\PublicShared\public %srcs% %SHARED_LIB% -sSIDE_MODULE %EMPP_ARGS% -c

SET objs=
FOR /F "tokens=* USEBACKQ" %%F IN (`dir /b %PLUGINS_FOLDER%\PrimitivesPlugin\Intermediate\*.o`) DO (
CALL SET "objs=%%objs%% %PLUGINS_FOLDER%\PrimitivesPlugin\Intermediate\%%F"
)
echo:
echo: Object files:
echo: %objs%
call em++ %objs% %SHARED_LIB% -sSIDE_MODULE %EMPP_ARGS% -o %PLUGINS_FOLDER%\PrimitivesPlugin\PrimitivesPlugin.so

del %PLUGINS_FOLDER%\PrimitivesPlugin\Intermediate\pluginmain.o
