@echo off

cd /D %~dp0\..\emsdk
call emsdk_env

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

mkdir %~dp0\build\web
cd /D %~dp0\build\web

call python %~dp0/../emsdk/upstream/emscripten/tools/file_packager.py intermediate/index.data --preload intermediate/plugins@/plugins --wasm64 --use-preload-plugins
call cmake --install intermediate --prefix install

pause