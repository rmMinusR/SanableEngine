@echo off

call embuild-env.bat

echo:
echo ======Building PublicShared======
echo:

::Gather PublicShared source files
SET srcs=
FOR /F "tokens=* USEBACKQ" %%F IN (`dir /b %~dp0\PublicShared\src\*.cpp`) DO (
CALL SET "srcs=%%srcs%% %~dp0\PublicShared\src\%%F"
)
echo: Gathered files:
echo: %srcs%
echo:

::Make build
call em++ -I%~dp0\PublicShared\public -I%~dp0\PublicShared\private %srcs% -sSIDE_MODULE %EMPP_ARGS% -r -o %SHARED_LIB%
