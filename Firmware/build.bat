@echo off
REM Quick build script for Attendance Logger (ESP-IDF)
REM Run this from the Firmware directory
REM
REM Usage:
REM   build.bat esp32      - Set target and build for ESP32
REM   build.bat esp32s3    - Set target and build for ESP32-S3
REM   build.bat flash      - Flash the last built binary
REM   build.bat monitor    - Monitor serial output
REM   build.bat menu       - Open menuconfig

setlocal enabledelayedexpansion

REM Check if IDF_PATH is set
if not defined IDF_PATH (
    echo Error: IDF_PATH environment variable not set
    echo Please set it manually:
    echo   set IDF_PATH=C:\path\to\esp-idf
    echo.
    echo Or install it with:
    echo   python -m pip install esp-idf
    exit /b 1
)

REM Show current target
echo.
echo Current target: 
idf.py.exe get-target 2>nul || echo (not set)
echo.

if "%1%"=="" (
    echo Usage: build.bat [command]
    echo.
    echo Commands:
    echo   esp32       - Set target to ESP32 and build
    echo   esp32s3     - Set target to ESP32-S3 and build
    echo   build       - Build (uses current target)
    echo   flash       - Flash to device
    echo   monitor     - Monitor serial output
    echo   clean       - Clean build artifacts
    echo   menu        - Open menuconfig
    echo   fullclean   - Clean and reset configuration
    echo.
    exit /b 0
)

if "%1%"=="esp32" (
    echo Setting target to ESP32...
    idf.py.exe set-target esp32
    echo Building for ESP32...
    idf.py.exe build
    exit /b 0
)

if "%1%"=="esp32s3" (
    echo Setting target to ESP32-S3...
    idf.py.exe set-target esp32s3
    echo Building for ESP32-S3...
    idf.py.exe build
    exit /b 0
)

if "%1%"=="build" (
    echo Building...
    idf.py.exe build
    exit /b 0
)

if "%1%"=="flash" (
    echo Flashing...
    idf.py.exe flash
    exit /b 0
)

if "%1%"=="monitor" (
    echo Monitoring (press Ctrl+] to exit)...
    idf.py.exe monitor
    exit /b 0
)

if "%1%"=="menu" (
    echo Opening menuconfig...
    idf.py.exe menuconfig
    exit /b 0
)

if "%1%"=="clean" (
    echo Cleaning build artifacts...
    idf.py.exe clean
    exit /b 0
)

if "%1%"=="fullclean" (
    echo Full clean...
    idf.py.exe fullclean
    exit /b 0
)

echo Unknown command: %1%
exit /b 1
