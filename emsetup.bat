cd /D %~dp0
call emsdk\emsdk.bat install tot
call emsdk\emsdk.bat activate tot
call emsdk\upstream\emscripten\emcc --generate-config
pause
