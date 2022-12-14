@echo off

call %~dp0\embuild-env.bat
call %~dp0\embuild-shared.bat
call %~dp0\embuild-PrimitivesPlugin.bat
call %~dp0\embuild-UserPlugin.bat
call %~dp0\embuild-engine.bat