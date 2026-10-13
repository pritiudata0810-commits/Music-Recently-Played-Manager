@echo off
title StackSound - Unit Test Runner
color 0E

echo ======================================================================
echo           STACKSOUND: AUTOMATED C++ TEST SUITE RUNNER
echo ======================================================================
echo.

set GXX_PATH="C:\Program Files (x86)\Dev-Cpp\MinGW64\bin\g++.exe"
if not exist %GXX_PATH% (
    set GXX_PATH=g++
)

echo [*] Compiling unit tests (tests\test_stack.cpp)...
%GXX_PATH% -std=c++11 -O2 "tests\test_stack.cpp" "src\stack.cpp" "src\song_manager.cpp" -o "tests\test_stack.exe"
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Test suite compilation failed.
    pause
    exit /b 1
)

echo [OK] Compilation successful. Executing 60 automated test cases...
echo.
"tests\test_stack.exe"

echo.
pause
