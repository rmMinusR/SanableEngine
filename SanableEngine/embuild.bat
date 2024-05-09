@echo off

cd /D %~dp0

set "SOURCE_DIR=%~dp0"
set "BUILD_DIR=%~dp0\build\web\intermediate"
set "INSTALL_DIR=%~dp0\build\web\install"
set "EMSDK=%~dp0\..\emsdk"
set "VSVARS=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"


rem Setup environment
call "%EMSDK%\emsdk_env.bat"
call "%VSVARS%" x64


echo:
echo:
echo:Configuring build...
echo:
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
call emcmake cmake -S %SOURCE_DIR% -B %BUILD_DIR% -DCMAKE_BUILD_TYPE=DEBUG
if %errorlevel% neq 0 goto :error


echo:
echo:
echo:Building %SOURCE_DIR% -^> %BUILD_DIR%
echo:
cmake --build %BUILD_DIR%
if %errorlevel% neq 0 goto :error


echo:
echo:
echo:Installing %BUILD_DIR% -^> %INSTALL_DIR%
echo:
if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"
cmake --install %BUILD_DIR% --prefix %INSTALL_DIR%
if %errorlevel% neq 0 goto :error


rem Success!
pause
exit /b 0

:error
pause
exit /b %errorlevel%