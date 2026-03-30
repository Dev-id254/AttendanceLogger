# Attendance Logger Firmware

## Overview

This folder contains the **ESP32/ESP32-S3 Attendance Logger firmware** written in **ESP-IDF C** (not Arduino C++).

**Supports:**

- ✅ **ESP32** (classic)
- ✅ **ESP32-S3** (newer variant)
- ✅ Same code for both chips (no changes needed)

**Features:**

- Bluetooth Classic (SPP) for wireless barcode input
- WiFi connectivity for HTTP POST to backend
- FreeRTOS-based multitasking
- Optimized memory usage (~285 KB / 512 KB)
- Full ESP-IDF integration

---

## Quick Start

### 1. Install ESP-IDF (One-time)

**Windows (PowerShell as Administrator):**

```powershell
python -m pip install esp-idf
```

**Linux/macOS:**

```bash
git clone --recursive https://github.com/espressif/esp-idf.git ~/esp/esp-idf
cd ~/esp/esp-idf
./install.sh
source export.sh
```

### 2. Set up Environment

**Windows (PowerShell):**

```powershell
python -m idf
# This sets up the environment automatically
```

**Linux/macOS:**

```bash
export IDF_PATH=~/esp/esp-idf
source $IDF_PATH/export.sh
```

### 3. Build & Flash

**Windows:**

```cmd
cd Firmware
build.bat esp32           # For ESP32
REM or
build.bat esp32s3         # For ESP32-S3
REM Then flash:
build.bat flash
REM Monitor:
build.bat monitor
```

**Linux/macOS:**

```bash
cd Firmware
chmod +x build.sh
./build.sh esp32          # For ESP32
# or
./build.sh esp32s3        # For ESP32-S3
# Then flash:
./build.sh flash
# Monitor:
./build.sh monitor
```

---

## File Structure

```
Firmware/
├── attendance/
│   └── attendance.ino          ← Main firmware code (ESP-IDF C)
├── main/
│   └── CMakeLists.txt          ← Component configuration
├── CMakeLists.txt              ← Project configuration
├── sdkconfig.defaults          ← Default ESP-IDF settings
├── build.bat                   ← Windows build script
├── build.sh                    ← Linux/Mac build script
├── ESP_IDF_BUILD_GUIDE.md      ← Detailed build instructions
├── CONVERSION_NOTES.md         ← Arduino → ESP-IDF changes
└── README.md                   ← This file
```

---

## Configuration

Edit `attendance/attendance.ino` to configure:

```c
#define EXAMPLE_SSID "YourSSID"
#define EXAMPLE_PASS "YourPassword"
#define BACKEND_URL "http://192.168.1.100:3000/api/attendance"
#define DEVICE_LOCATION "Lab 1"
#define BLUETOOTH_DEVICE_NAME "ESP32_Attendance"
```

After editing, rebuild:

```bash
idf.py build
idf.py -p COM3 flash  # Windows: COM3, COM4, etc.
idf.py -p /dev/ttyUSB0 flash  # Linux: /dev/ttyUSB0, /dev/ttyUSB1, etc.
```

---

## Key Differences from Arduino

| Feature            | Arduino               | ESP-IDF               |
| ------------------ | --------------------- | --------------------- |
| **Entry point**    | `setup()` / `loop()`  | `app_main()`          |
| **Threading**      | Single-threaded       | FreeRTOS tasks        |
| **Build system**   | PlatformIO            | CMake                 |
| **Debugging**      | Limited               | Full GDB support      |
| **Memory control** | Automatic             | Fine-grained          |
| **Bluetooth**      | BluetoothSerial class | SPP callbacks + queue |

See [CONVERSION_NOTES.md](CONVERSION_NOTES.md) for detailed comparison.

---

## Troubleshooting

### Build fails: "CMake not found"

```bash
python -m pip install cmake
```

### Serial monitor shows garbage

- Ensure baud rate is **115200**
- Check `menuconfig` → Serial flasher config

### Device not found during flash

```bash
# List available ports:
# Windows:
python -m serial.tools.list_ports

# Linux/Mac:
ls /dev/tty*
```

### Bluetooth not initializing

1. Run: `idf.py menuconfig`
2. Go to: `Component config → Bluetooth → Enable Bluetooth`
3. Rebuild: `idf.py build`

### WiFi won't connect

- Verify SSID and password in code
- Check SSID is 2.4 GHz (not 5 GHz)
- Inspect Serial Monitor for WiFi event messages

---

## Build & Flash Commands

### Quick Commands (using scripts)

**Windows (PowerShell):**

```batch
.\build.bat esp32            # Set target and build
.\build.bat flash            # Flash to device
.\build.bat monitor          # View serial output
.\build.bat menu             # Configuration
.\build.bat clean            # Clean builds
.\build.bat fullclean        # Reset everything
```

**Linux/Mac (Bash):**

```bash
./build.sh esp32             # Set target and build
./build.sh flash             # Flash to device
./build.sh monitor           # View serial output
./build.sh menu              # Configuration
./build.sh clean             # Clean builds
./build.sh fullclean         # Reset everything
```

### Manual Commands

```bash
# Set target chip
idf.py set-target esp32      # ESP32
idf.py set-target esp32s3    # ESP32-S3

# Build
idf.py build

# Flash (auto-detect COM port)
idf.py flash

# Flash to specific port
idf.py -p /dev/ttyUSB0 flash     # Linux
idf.py -p COM3 flash              # Windows

# Monitor serial output
idf.py monitor
idf.py -p /dev/ttyUSB0 monitor    # Specific port

# Combined (build + flash + monitor)
idf.py -p /dev/ttyUSB0 flash monitor

# Configuration menu
idf.py menuconfig

# Clean artifacts
idf.py clean
idf.py fullclean
```

---

## Serial Monitor Output

### Expected Startup Output

```
I (243) ATTENDANCE_LOGGER: ========================================
I (243) ATTENDANCE_LOGGER: ESP32/ESP32-S3 Attendance Logger
I (243) ATTENDANCE_LOGGER: Built with ESP-IDF (C language)
I (243) ATTENDANCE_LOGGER: ========================================
I (253) ATTENDANCE_LOGGER: Initializing WiFi...
I (263) ATTENDANCE_LOGGER: Connecting to YourSSID...........
I (3456) ATTENDANCE_LOGGER: WiFi connected with IP: 192.168.1.150
I (3456) ATTENDANCE_LOGGER: Initializing Bluetooth...
I (3500) ATTENDANCE_LOGGER: Bluetooth initialized: ESP32_Attendance
I (3500) ATTENDANCE_LOGGER: Bluetooth reader task started
I (3500) ATTENDANCE_LOGGER: Firmware initialization complete
I (3510) ATTENDANCE_LOGGER: Waiting for Bluetooth connections and WiFi messages...
```

### When Barcode Received

```
I (45230) ATTENDANCE_LOGGER: [BT] Received barcode: CCT/00001/023
I (45240) ATTENDANCE_LOGGER: [HTTP] POST success: code 200
```

### If Error Occurs

```
E (45240) ATTENDANCE_LOGGER: [HTTP] POST failed: ESP_ERR_HTTP_CONNECT
```

---

## Memory Usage

```
Partition layout:
  Factory app    0x1000      ~1.5 MB
  OTA data       0xd000      0x2000
  NVRAM (NVS)    0xf000      12 KB
  PHY_INIT       0x11000     4 KB
```

**Runtime RAM:**

- FreeRTOS kernel: ~50 KB
- Bluetooth stack: ~80 KB
- WiFi stack: ~150 KB
- Application: ~5 KB
- **Total: ~285 KB** (55% of available)

---

## Power Consumption

| State             | Current    |
| ----------------- | ---------- |
| Deep Sleep        | 10 μA      |
| Idle              | 70-100 mA  |
| Bluetooth receive | 100-150 mA |
| HTTP POST         | 200-300 mA |
| Peak              | ~400 mA    |

**Recommended power supply:** 5V, 1A minimum (USB is sufficient)

---

## Development & Debugging

### Enable Debug Logging

Edit `sdkconfig.defaults`:

```
CONFIG_LOG_DEFAULT_LEVEL_DEBUG=y
```

Then rebuild:

```bash
idf.py fullclean
idf.py build
```

### GDB Debugging

```bash
idf.py monitor --gdb
```

Then in another terminal:

```bash
xtensa-esp32-elf-gdb build/attendance_logger_esp32.elf -ex "target remote :3333"
```

### Performance Monitoring

```bash
idf.py build
idf.py size                # Total binary size
idf.py size-components     # Size by component
```

---

## Firmware Update (OTA)

Future enhancement to update firmware over-the-air without USB:

```c
#include "esp_ota_ops.h"
// Download binary from server
// Verify signature
// Flash new partition
// Reboot on success
```

Not implemented in MVP but recommended for production.

---

## Related Documentation

- [**ESP_IDF_BUILD_GUIDE.md**](ESP_IDF_BUILD_GUIDE.md) - Detailed build instructions
- [**CONVERSION_NOTES.md**](CONVERSION_NOTES.md) - Arduino → ESP-IDF comparison
- [**PHONE_SETUP_GUIDE.md**](../Documentation/PHONE_SETUP_GUIDE.md) - Phone app setup
- [**API_REFERENCE.md**](../Documentation/API_REFERENCE.md) - Backend API

---

## Support

For ESP-IDF documentation: https://docs.espressif.com/projects/esp-idf/en/latest/

For hardware info:

- [ESP32 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf)
- [ESP32-S3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)

---

**Last Updated:** March 30, 2026  
**Converted from Arduino to ESP-IDF C**
