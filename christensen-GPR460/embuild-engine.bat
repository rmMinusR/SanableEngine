@echo off

call embuild-env.bat

echo:
echo ======Building main project======
echo:

cd /D %~dp0\christensen-GPR460


::Gather main project source files
SET srcs=
FOR /F "tokens=* USEBACKQ" %%F IN (`dir /b src\*.cpp`) DO (
CALL SET "srcs=%%srcs%% src\%%F"
)
echo: Gathered files:
echo: %srcs%
echo:

::Make build
call em++ -Iprivate -I%~dp0\PublicShared\public %srcs% %SHARED_LIB% -sMAIN_MODULE %EMPP_ARGS% --use-preload-plugins --preload-file %PLUGINS_FOLDER%@plugins -o %~dp0\builds\web\index.html
