@echo off
title StackSound - Dev-C++ Build Script
color 0A

echo ======================================================================
echo           STACKSOUND: C++ BUILD SCRIPT (DEV-C++ COMPATIBLE)
echo ======================================================================
echo.

set GXX_PATH="C:\Program Files (x86)\Dev-Cpp\MinGW64\bin\g++.exe"

if not exist %GXX_PATH% (
    echo [!] Default Dev-C++ compiler not found at:
    echo     %GXX_PATH%
    echo [*] Checking system PATH for g++...
    where g++ >nul 2>nul
    if %ERRORLEVEL% equ 0 (
        set GXX_PATH=g++
    ) else (
        echo [ERROR] No C++ compiler found. Please ensure Dev-C++ is installed.
        pause
        exit /b 1
    )
)

echo [*] Using Compiler: %GXX_PATH%
echo.

echo [1/3] Compiling Standalone Console Application (main_standalone.exe)...
%GXX_PATH% -std=c++11 -O2 "src\main_standalone.cpp" "src\stack.cpp" "src\song_manager.cpp" -o "main_standalone.exe"
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Failed to compile main_standalone.exe
    pause
    exit /b 1
)
echo [OK] main_standalone.exe compiled successfully!
echo.

echo [2/3] Compiling Single-File Dev-C++ Helper (devcpp_single_file.exe)...
%GXX_PATH% -std=c++11 -O2 "src\devcpp_single_file.cpp" -o "devcpp_single_file.exe"
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Failed to compile devcpp_single_file.exe
    pause
    exit /b 1
)
echo [OK] devcpp_single_file.exe compiled successfully!
echo.

echo [3/3] Compiling Integrated HTTP Backend Server (backend_server.exe)...
%GXX_PATH% -std=c++11 -O2 "src\backend_server.cpp" "src\stack.cpp" "src\song_manager.cpp" -o "backend_server.exe" -lws2_32
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Failed to compile backend_server.exe
    pause
    exit /b 1
)
echo [OK] backend_server.exe compiled successfully!
echo.

echo ======================================================================
echo                      ALL TARGETS COMPILED CLEANLY!
echo ======================================================================
echo.
echo Options:
echo   [1] Run Standalone Console Program (Dev-C++ demo)
echo   [2] Run Backend Server and Launch Desktop UI
echo   [3] Run Automated Unit Tests
echo   [4] Exit
echo.
set /p choice="Enter choice [1-4]: "

if "%choice%"=="1" (
    cls
    main_standalone.exe
)
if "%choice%"=="2" (
    start_backend.bat
)
if "%choice%"=="3" (
    run_tests.bat
)

exit /b 0
