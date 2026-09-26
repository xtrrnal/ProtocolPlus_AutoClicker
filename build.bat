@echo off
setlocal
title Protocol+ Auto Clicker Build

if not exist build mkdir build
if not exist release mkdir release

where cl >nul 2>nul
if errorlevel 1 (
    echo.
    echo Microsoft C++ compiler was not found.
    echo Open this script from a Visual Studio Developer Command Prompt,
    echo or install Visual Studio 2022 Build Tools with Desktop C++.
    pause
    exit /b 1
)

where iscc >nul 2>nul
if errorlevel 1 (
    echo.
    echo Inno Setup compiler was not found.
    echo Install Inno Setup 6 and ensure ISCC.exe is in PATH.
    pause
    exit /b 1
)

echo Building Protocol+ Auto Clicker...
cl /nologo /O2 /EHsc /DUNICODE /D_UNICODE src\ProtocolPlus_Auto_Clicker.cpp user32.lib gdi32.lib shell32.lib /link /SUBSYSTEM:WINDOWS /OUT:build\ProtocolPlus_Auto_Clicker.exe

if errorlevel 1 (
    echo Native build failed.
    pause
    exit /b 1
)

echo Building installer...
iscc installer.iss

if errorlevel 1 (
    echo Installer build failed.
    pause
    exit /b 1
)

echo.
echo ============================================
echo BUILD COMPLETE
echo ============================================
echo release\ProtocolPlus_Auto_Clicker_Setup.exe
echo.
pause
