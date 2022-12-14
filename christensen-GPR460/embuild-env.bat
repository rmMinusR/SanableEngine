echo:
echo ======Setting Emscripten environment variables======
echo:

cd /D %~dp0\emsdk
call emsdk_env

SET EMPP_ARGS=-sUSE_SDL=2 -sLLD_REPORT_UNDEFINED -Wparentheses

SET OUTPUT_BASE_DIR=%~dp0\builds\web
SET SHARED_LIB=%OUTPUT_BASE_DIR%\PublicShared.o
SET PLUGINS_FOLDER=%OUTPUT_BASE_DIR%\plugins

mkdir %OUTPUT_BASE_DIR%
mkdir %PLUGINS_FOLDER%