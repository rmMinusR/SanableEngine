echo:
echo ======Setting Emscripten environment variables======
echo:

cd /D %~dp0\emsdk
call emsdk_env

SET EMPP_ARGS=-sUSE_SDL=2 -sLLD_REPORT_UNDEFINED
