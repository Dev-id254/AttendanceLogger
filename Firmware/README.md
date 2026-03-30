# Attendance Logger Firmware

## Overview

This folder contains the **ESP32/ESP32-S3 Attendance Logger firmware** written in **Arduino C++** (PlatformIO/Arduino IDE).

**Supports:**

- ✅ **ESP32** (classic)
- ✅ **ESP32-S3** (newer variant)
- ✅ Same code for both chips (no changes needed)
- ✅ Bluetooth Serial for wireless barcode input
- ✅ WiFi connectivity for HTTP POST to backend
- ✅ Simple setup/loop architecture
- ✅ Optimized memory usage (~290 KB / 512 KB available = 55%)

---

## Quick Start

### 1. Install Arduino IDE or PlatformIO

**Arduino IDE:**

- Download Arduino IDE 2.0+
- Install ESP32 board package (Tools → Board Manager → Search "ESP32")

**PlatformIO (Recommended):**

- Install VS Code
- Install PlatformIO extension
- Open this project folder

### 2. Configure the Firmware

Edit `attendance/attendance.ino` to set your WiFi and backend details:

```cpp
const char* ssid = "YourNetwork";
const char* password = "YourPassword";
const char* backendUrl = "http://192.168.1.100:3000/api/attendance";
const char* bluetoothName = "ESP32_Attendance";
```

### 3. Build & Flash

**Arduino IDE:**

- Open `attendance/attendance.ino`
- Select board: ESP32 Dev Module or ESP32-S3 Dev Module
- Set COM port
- Click Upload

**PlatformIO:**

```bash
pio run -t upload
```

### 4. Monitor Serial Output

**Arduino IDE:** Tools → Serial Monitor (115200 baud)

**PlatformIO:**

```bash
pio device monitor
```

---

## File Structure

```
Firmware/
├── attendance/
│   └── attendance.ino          ← Main firmware code (Arduino C++)
├── CMakeLists.txt              ← ESP-IDF build files (can be removed)
├── main/CMakeLists.txt         ← ESP-IDF build files (can be removed)
├── sdkconfig.defaults          ← ESP-IDF build files (can be removed)
├── build.bat                   ← ESP-IDF build script (can be removed)
├── build.sh                    ← ESP-IDF build script (can be removed)
├── README.md                   ← This file
├── ESP_IDF_BUILD_GUIDE.md      ← ESP-IDF guide (can be removed)
└── CONVERSION_NOTES.md         ← Technical notes (can be removed)
```

---

## Configuration

Edit **`attendance/attendance.ino`** to customize:

```cpp
const char* ssid = "YourNetwork";
const char* password = "YourPassword";
const char* backendUrl = "http://192.168.1.100:3000/api/attendance";
const char* bluetoothName = "ESP32_Attendance";
```

After editing, rebuild and flash:

```bash
pio run -t upload
```

---

## Serial Monitor Output

### Expected Startup Output

```
========================================
ESP32/ESP32-S3 Attendance Logger
Built with Arduino C++
========================================
Initializing WiFi...
Connecting to YourNetwork...........
WiFi connected with IP: 192.168.1.150
Initializing Bluetooth...
Bluetooth initialized
Bluetooth device name: ESP32_Attendance
Bluetooth PIN: 1234
Firmware initialization complete
Waiting for Bluetooth connections and WiFi messages...
```

### When Barcode Received

```
[BT] Received barcode: CCT/00001/023
[HTTP] POST success: code 200
[HTTP] Response: {"id":42,"timestamp":"2026-03-30T14:35:42.123Z"}
```

### If Error Occurs

```
[HTTP] POST failed: connection refused
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

- Arduino framework: ~50 KB
- Bluetooth stack: ~80 KB
- WiFi stack: ~150 KB
- Application: ~10 KB
- **Total: ~290 KB** (57% of 512 KB available)

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

The firmware already includes detailed logging. To see more:

**Arduino IDE:** Tools → Serial Monitor (115200 baud)

**PlatformIO:**

```bash
pio device monitor --baud 115200
```

### Troubleshooting

| Problem              | Solution                                   |
| -------------------- | ------------------------------------------ |
| `Board not found`    | Check COM port in Arduino IDE              |
| Serial shows garbage | Verify baud rate is 115200                 |
| Bluetooth not found  | Ensure Bluetooth is enabled on phone       |
| WiFi auth fails      | Verify SSID/password in code               |
| HTTP fails           | Check backend URL and network connectivity |

---

## Build Commands

### PlatformIO (Recommended)

| Command              | Purpose                   |
| -------------------- | ------------------------- |
| `pio run`            | Build the project         |
| `pio run -t upload`  | Build and flash to device |
| `pio device monitor` | View serial output        |
| `pio run -t clean`   | Clean build artifacts     |
| `pio run -t size`    | Show binary size info     |

### Arduino IDE

- **Build:** Sketch → Verify/Compile
- **Upload:** Sketch → Upload
- **Monitor:** Tools → Serial Monitor

---

## Selecting Between ESP32 & ESP32-S3

The code is **100% compatible** with both chips. Simply:

1. Select the correct board in Arduino IDE or PlatformIO
2. Flash as normal

No code changes needed!

## Performance (ESP32 vs ESP32-S3)

| Feature     | ESP32         | ESP32-S3      |
| ----------- | ------------- | ------------- |
| Cores       | 2             | 2             |
| Clock Speed | 240 MHz       | 240 MHz       |
| RAM         | 520 KB        | 512 KB        |
| Flash       | 4-16 MB       | 4-16 MB       |
| Bluetooth   | Classic + BLE | Classic + BLE |
| WiFi        | 802.11 b/g/n  | 802.11 b/g/n  |
| USB         | Via FTDI      | Built-in      |
| Cost        | $6-10         | $8-12         |

For this project, both perform identically. ESP32-S3 has slight advantages in USB connectivity.

---

## Related Documentation

- [**PHONE_SETUP_GUIDE.md**](../Documentation/PHONE_SETUP_GUIDE.md) - Phone app setup
- [**API_REFERENCE.md**](../Documentation/API_REFERENCE.md) - Backend API
- [**HARDWARE_CONFIGURATION.md**](../Documentation/HARDWARE_CONFIGURATION.md) - Hardware specs

---

## Support

For Arduino ESP32 documentation: https://docs.espressif.com/projects/arduino-esp32/

For hardware info:

- [ESP32 Datasheet](https://www.espressif.com/products/socs/esp32/resources/)
- [ESP32-S3 Datasheet](https://www.espressif.com/products/socs/esp32-s3/resources/)

---

**Last Updated:** March 30, 2026  
**Converted back to Arduino C++**

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
