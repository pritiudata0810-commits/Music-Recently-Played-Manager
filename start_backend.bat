@echo off
title StackSound - Launcher
color 0B

echo ======================================================================
echo           STACKSOUND: LAUNCHING C++ BACKEND & DESKTOP UI
echo ======================================================================
echo.

set GXX_PATH="C:\Program Files (x86)\Dev-Cpp\MinGW64\bin\g++.exe"
if not exist %GXX_PATH% (
    set GXX_PATH=g++
)

if not exist "backend_server.exe" (
    echo [*] backend_server.exe not found. Compiling first...
    %GXX_PATH% -std=c++11 -O2 "src\backend_server.cpp" "src\stack.cpp" "src\song_manager.cpp" -o "backend_server.exe" -lws2_32
    if %ERRORLEVEL% neq 0 (
        echo [ERROR] Backend compilation failed.
        pause
        exit /b 1
    )
    echo [OK] Compiled backend_server.exe successfully!
)

echo [*] Launching C++ Backend Server in a dedicated terminal window...
start "StackSound C++ Backend Terminal (Real LIFO Operations)" cmd /k "backend_server.exe 8000"

echo [*] Waiting for server to initialize...
timeout /t 2 /nobreak >nul

set PORT=8000
if exist "active_port.txt" (
    set /p PORT=<active_port.txt
)

echo [*] Server is active on port: %PORT%
echo [*] Opening interface at: http://127.0.0.1:%PORT%
start "" "http://127.0.0.1:%PORT%"

echo.
echo ======================================================================
echo  The C++ Backend is now running in the separate terminal window!
echo  Every action in your browser will be executed and logged there.
echo  To shut down, close the terminal window or press Ctrl+C in it.
echo ======================================================================
echo.
pause
