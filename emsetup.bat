cd /D %~dp0
call christensen-GPR460\emsdk\emsdk.bat install tot
call christensen-GPR460\emsdk\emsdk.bat activate tot
call christensen-GPR460\emsdk\upstream\emscripten\emcc --generate-config
pause
