# Hardware Configuration Guide

## ESP32 Setup

### Required Components

- **ESP32 DevKit** (e.g., ESP32-WROOM, ESP32-WROVER)
- **USB cable** (Type-C or Micro-USB, depending on board variant)
- **WiFi network** (2.4GHz recommended)

### Wiring

- **Serial connection**: USB cable to computer (for first-time flashing and debugging)
- **No hardware connections needed** - Bluetooth and WiFi are built-in

### ESP32 Pinout Reference

For future expansion or additional sensors:

- **RX** (GPIO 3): Serial receive (debug only, no scanner needed)
- **TX** (GPIO 1): Serial transmit (debug output)
- **3.3V / GND**: Power reference
- Other GPIO pins: Available for future expansion

---

## What's Changed (From Previous Design)

### ❌ **Removed**

- **Physical barcode scanner hardware**
- Scanner UART/USB interface
- Scanner initialization code
- Hardware-specific scanner drivers

### ✅ **Added**

- **BluetoothSerial library** (built-in to ESP32 IDF)
- Bluetooth initialization in firmware
- Bluetooth message handling

### ⚡ **Still Present**

- WiFi connectivity (same as before)
- HTTP POST to backend (same endpoint)
- Serial debugging output

---

## Firmware Compilation

### Prerequisites

- **Arduino IDE 2.0+** or **PlatformIO**
- **ESP32 board package** installed
- **BluetoothSerial library** (included in modern ESP32 packages)

### Arduino IDE Setup

1. Install board: Tools → Board Manager → Search "ESP32" → Install "esp32 by Espressif Systems"
2. Select board: Tools → Board → ESP32 Dev Module
3. Select port: Tools → Port → (Your COM port)
4. Baud rate: 115200

### Compilation Steps

1. Open `Firmware/attendance.ino` in Arduino IDE
2. Update configuration:
   ```cpp
   const char* ssid = "YourSSID";
   const char* password = "YourPassword";
   const char* backendUrl = "http://192.168.1.100:3000/api/attendance";
   ```
3. Click **Upload** (or Sketch → Upload)
4. Wait for compilation and flashing

### Verification

After flashing, open Serial Monitor (Tools → Serial Monitor):

```
115200
Bluetooth initialized
Bluetooth device name: ESP32_Attendance
Connecting to YourSSID.......
WiFi connected
```

If you see these messages, the ESP32 is ready!

---

## Bluetooth Specifications

### BluetoothSerial Protocol

- **Standard**: Bluetooth Classic (SPP - Serial Port Profile)
- **Baud rate**: 115200 (non-configurable)
- **Device name**: `ESP32_Attendance` (configurable in firmware)
- **PIN**: `1234` (default, changeable in firmware)
- **Range**: ~10-100 meters (depends on environment)

### Pairing

- Phone discovers ESP32 as `ESP32_Attendance`
- Requires PIN entry on phone: `1234`
- After pairing, connects automatically when nearby

---

## WiFi Configuration

### Requirements

- 2.4GHz WiFi network (5GHz not recommended for ESP32)
- Network should allow local device communication
- No special router configuration needed

### Finding ESP32 IP Address

The ESP32 will print its IP address to Serial Monitor during startup:

```
WiFi connected
IP: 192.168.1.150
```

Use this IP when configuring MacroDroid or making direct HTTP requests.

### Network Troubleshooting

| Issue                           | Solution                                                                        |
| ------------------------------- | ------------------------------------------------------------------------------- |
| "WiFi not connecting"           | Check SSID/password in code. Verify 2.4GHz band. Check firewall settings.       |
| "Cannot reach ESP32 from phone" | Verify both on same WiFi. Check router allows multi-device communication.       |
| "IP address keeps changing"     | Assign static IP via router's DHCP settings or use `WiFi.config()` in firmware. |

---

## Power Consumption

### Estimated Current Draw

- **Idle (WiFi on)**: ~70-100 mA
- **Receiving Bluetooth data**: ~100-150 mA
- **Sending HTTP request**: ~200-300 mA
- **Peak (simultaneous BT + HTTP)**: ~400 mA

### Power Supply Recommendation

- **USB 5V, 500mA minimum** (standard USB port from computer)
- **External 5V supply** (1A recommended) for production

---

## Performance Metrics

### Bluetooth Serial

- **Latency**: ~50-300 ms (message sent to received by ESP32)
- **Throughput**: ~10-20 messages/second
- **Recommended density**: 1-5 scans/minute for reliable operation

### WiFi HTTP

- **Latency**: ~100-500 ms (including HTTP round-trip)
- **Throughput**: ~1-3 requests/second
- **Recommended density**: 1-2 scans/minute for stable operation

---

## Debugging

### Serial Monitor Output

Enable by opening Serial Monitor at **115200 baud**:

- `[BT] Received barcode: [ID]` - Bluetooth message received
- `[HTTP] POST success: code 200` - Attendance recorded
- `[HTTP] POST failed: ...` - Connection/server error
- WiFi connection status messages

### Low-Level Debugging

Enable verbose logging in firmware:

```cpp
#define DEBUG_MODE 1  // Add to top of sketch
```

### Capture Bluetooth Traffic

- Use external Bluetooth sniffer tools (advanced)
- Or monitor via hcidump on Linux (if needed)

---

## Future Expansion

The ESP32 has plenty of GPIO pins available for:

- **Temperature/humidity sensors** (DHT11/22)
- **RFID reader** (alternative to Bluetooth barcode input)
- **LED indicators** (WiFi/Bluetooth status)
- **Relay module** (unlock door when attendance recorded, etc.)
- **Real-time clock** (DS3231) for timestamp accuracy

---

## Safety & Best Practices

⚠️ **Important Notes**:

1. **No high-voltage circuits** near ESP32 (3.3V device)
2. **USB power only** during development (built-in protection)
3. **Keep antenna area clear** (top of board, no metal nearby)
4. **Avoid static discharge** when handling board
5. **Don't leave powered on indefinitely** without thermal monitoring

---

See [`PHONE_SETUP_GUIDE.md`](PHONE_SETUP_GUIDE.md) for mobile device configuration.
