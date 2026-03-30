#!/bin/bash

# Quick build script for Attendance Logger (ESP-IDF)
# Run this from the Firmware directory
#
# Usage:
#   ./build.sh esp32      - Set target and build for ESP32
#   ./build.sh esp32s3    - Set target and build for ESP32-S3
#   ./build.sh flash      - Flash the last built binary
#   ./build.sh monitor    - Monitor serial output
#   ./build.sh menu       - Open menuconfig

# Check if IDF_PATH is set
if [ -z "$IDF_PATH" ]; then
    echo "Error: IDF_PATH environment variable not set"
    echo ""
    echo "Set it with:"
    echo "  export IDF_PATH=~/esp/esp-idf"
    echo "  source \$IDF_PATH/export.sh"
    exit 1
fi

# Show current target
echo ""
echo "Current target:"
idf.py get-target 2>/dev/null || echo "(not set)"
echo ""

if [ $# -eq 0 ]; then
    echo "Usage: ./build.sh [command]"
    echo ""
    echo "Commands:"
    echo "  esp32       - Set target to ESP32 and build"
    echo "  esp32s3     - Set target to ESP32-S3 and build"
    echo "  build       - Build (uses current target)"
    echo "  flash       - Flash to device"
    echo "  monitor     - Monitor serial output"
    echo "  clean       - Clean build artifacts"
    echo "  menu        - Open menuconfig"
    echo "  fullclean   - Clean and reset configuration"
    echo ""
    exit 0
fi

case "$1" in
    esp32)
        echo "Setting target to ESP32..."
        idf.py set-target esp32
        echo "Building for ESP32..."
        idf.py build
        ;;
    esp32s3)
        echo "Setting target to ESP32-S3..."
        idf.py set-target esp32s3
        echo "Building for ESP32-S3..."
        idf.py build
        ;;
    build)
        echo "Building..."
        idf.py build
        ;;
    flash)
        echo "Flashing..."
        idf.py flash
        ;;
    monitor)
        echo "Monitoring (press Ctrl+] to exit)..."
        idf.py monitor
        ;;
    menu)
        echo "Opening menuconfig..."
        idf.py menuconfig
        ;;
    clean)
        echo "Cleaning build artifacts..."
        idf.py clean
        ;;
    fullclean)
        echo "Full clean..."
        idf.py fullclean
        ;;
    *)
        echo "Unknown command: $1"
        exit 1
        ;;
esac
