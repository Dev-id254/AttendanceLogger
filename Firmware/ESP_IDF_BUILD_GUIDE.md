# Building with ESP-IDF (C Language)

This project has been converted from Arduino C++ (`PlatformIO`) to **ESP-IDF C** for enhanced control and support for both **ESP32** and **ESP32-S3** microcontrollers.

## Requirements

- **ESP-IDF v5.0 or later** (Install from: https://github.com/espressif/esp-idf)
- **ESP32 or ESP32-S3** development board
- **Linux/macOS/Windows** (with WSL for Windows)

## Installation (One-time Setup)

### 1. Install ESP-IDF

**Linux/macOS:**

```bash
mkdir -p ~/esp
cd ~/esp
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh
```

**Windows (using PowerShell):**

```powershell
mkdir "$env:USERPROFILE\esp"
cd "$env:USERPROFILE\esp"
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
./install.ps1
```

### 2. Set up Environment Variables

After installation, set the `IDF_PATH` environment variable:

**Linux/macOS:**

```bash
export IDF_PATH=~/esp/esp-idf
source ~/esp/esp-idf/export.sh
```

**Windows (PowerShell):**

```powershell
$env:IDF_PATH = "$env:USERPROFILE\esp\esp-idf"
& "$env:USERPROFILE\esp\esp-idf\export.ps1"
```

## Building the Project

### Step 1: Navigate to Firmware Directory

```bash
cd StudentLogger/Firmware
```

### Step 2: Set Target Chip

For **ESP32**:

```bash
idf.py set-target esp32
```

For **ESP32-S3**:

```bash
idf.py set-target esp32s3
```

You can check the current target:

```bash
idf.py get-target
```

### Step 3: Configure the Project

```bash
idf.py menuconfig
```

This opens an interactive configuration menu. Navigate to:

1. **Serial flasher config** → Set the correct COM port/device
2. **WiFi Configuration** (Optional - under Component config if needed)
3. Press **Q** to save and exit

Or configure via environment variables:

```bash
idf.py menuconfig
```

### Step 4: Build the Project

```bash
idf.py build
```

Compilation output will be in `build/` directory.

### Step 5: Flash to Device

Connect your ESP32/ESP32-S3 via USB cable, then:

```bash
idf.py -p /dev/ttyUSB0 flash
```

Replace `/dev/ttyUSB0` with your actual COM port:

- **Linux/macOS**: `/dev/ttyUSB0`, `/dev/ttyUSB1`, or `/dev/cu.usbserial-*`
- **Windows**: `COM3`, `COM4`, etc. (can use `idf.py.exe -p COM3 flash`)

### Step 6: Monitor Serial Output

```bash
idf.py -p /dev/ttyUSB0 monitor
```

Press **Ctrl+]** to exit monitor.

### Combined (Build + Flash + Monitor)

```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

## Configuration: WiFi & Bluetooth

### Edit firmware settings

Open `Firmware/attendance/attendance.ino` and modify these lines:

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
```

Then flash again:

```bash
idf.py -p /dev/ttyUSB0 flash
```

## Project Structure

```
Firmware/
├── CMakeLists.txt          # Root project configuration
├── main/
│   └── CMakeLists.txt      # Main component configuration
├── attendance/
│   └── attendance.ino      # Main firmware code (C language)
└── build/                  # Compilation output (auto-generated)
    └── attendance_logger_esp32.bin
```

## Troubleshooting

### Device Not Found

```bash
# List available ports
idf.py.exe list-defs
```

### Build Errors

**Error: `IDF_PATH` not set**

```bash
export IDF_PATH=~/esp/esp-idf  # Linux/macOS
# or
$env:IDF_PATH = "$env:USERPROFILE\esp\esp-idf"  # Windows
source ~/esp/esp-idf/export.sh  # Linux/macOS
# or
& "$env:USERPROFILE\esp\esp-idf\export.ps1"  # Windows
```

**Error: Wrong target chip**

```bash
idf.py set-target esp32s3  # For ESP32-S3
# Then rebuild
idf.py fullclean
idf.py build
```

**CMake errors**

```bash
idf.py fullclean
idf.py build
```

### Flashing Errors

**Device stays in bootloader mode**

- Hold RESET button while flashing
- try a different USB cable

**Permission denied (Linux)**

```bash
sudo usermod -a -G dialout $USER
# Then log out and log back in
```

### Monitor Shows Garbage

- Verify baud rate: Default is **115200**
- Check `menuconfig` Serial flasher config
- Try different USB cable or port

## Useful Commands

| Command                     | Purpose                   |
| --------------------------- | ------------------------- |
| `idf.py build`              | Compile the project       |
| `idf.py flash`              | Flash to device           |
| `idf.py monitor`            | View serial output        |
| `idf.py fullclean`          | Clean build artifacts     |
| `idf.py menuconfig`         | Interactive configuration |
| `idf.py set-target esp32s3` | Change target chip        |
| `idf.py size`               | Show binary size info     |
| `idf.py size-components`    | Show size by component    |

## Selecting Between ESP32 & ESP32-S3

The code is **100% compatible** with both chips. Simply:

1. Set the target: `idf.py set-target esp32` or `esp32s3`
2. Plug in your device
3. Flash: `idf.py flash`

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

## Arduino IDE Alternative

To use **Arduino IDE** instead (simpler for beginners):

1. Download Arduino IDE 2.0+
2. Install ESP32 board package (Tools → Board Manager → Search "ESP32")
3. Open `Firmware/attendance/attendance.ino`
4. Change `#include` headers back:
   - Replace ESP-IDF headers with Arduino headers
   - This is not recommended as ESP-IDF is more powerful

**Stick with ESP-IDF** for this project - it's the official development framework.

## Build Artifacts

After successful build, the binary is at:

```
build/attendance_logger_esp32.bin
```

This can be flashed manually:

```bash
esptool.py -p /dev/ttyUSB0 -b 460800 write_flash 0x0 build/address_bin.bin
```

But `idf.py flash` handles this automatically.

---

For more information:

- [ESP-IDF Official Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/)
- [ESP32-S3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)
- [Getting Started with ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/index.html)

---

See [`Documentation/HARDWARE_CONFIG.md`](../Documentation/HARDWARE_CONFIGURATION.md) for hardware setup and wiring instructions.
