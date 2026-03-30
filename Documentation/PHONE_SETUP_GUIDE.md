# Phone Setup Guide - Attendance Logger

This document explains how to set up a mobile device to scan student ID barcodes and send them to the ESP32 Attendance Logger.

## Overview

The ESP32 now receives attendance data from a phone via two methods:

1. **Bluetooth Serial** - Using "Bluetooth Serial Reader" app
2. **WiFi HTTP** - Using "MacroDroid" or direct HTTP requests

---

## Method 1: Bluetooth Serial Reader (Recommended for Offline)

### Requirements

- Android phone with Bluetooth capability
- **Bluetooth Serial Reader** app (Google Play Store)
- Barcode scanner app (e.g., built-in camera with barcode detection or "Barcode Scanner" app)

### Step 1: Pair Phone with ESP32

1. On the ESP32:
   - The device broadcasts as `ESP32_Attendance`
   - Pairing PIN: `1234`

2. On your Android phone:
   - Go to Settings → Bluetooth
   - Enable Bluetooth
   - Search for nearby devices
   - Select `ESP32_Attendance`
   - Enter PIN: `1234`
   - Confirm pairing

### Step 2: Open Bluetooth Serial Reader App

1. Launch **Bluetooth Serial Reader** app
2. Tap **Connect** or **Devices**
3. Select `ESP32_Attendance` from the paired devices list
4. Should show `Connected` status

### Step 3: Scan Barcodes and Send

1. Use your phone's barcode scanner app to scan student ID cards
2. The scanned barcode (student ID) will appear in the Bluetooth Serial Reader app
3. Send the barcode through the app - it will be transmitted to the ESP32
4. The ESP32 will record attendance and send to the backend
5. You'll receive feedback: `OK: Attendance recorded for [ID]` or error message

### Advantages

- Works offline (when WiFi is unavailable)
- Works within Bluetooth range (~10-100 meters)
- Real-time feedback on the phone

---

## Method 2: MacroDroid (WiFi Automation)

### Requirements

- Android phone with WiFi
- **MacroDroid** app (Google Play Store - free version available)
- Barcode scanner app

### Step 1: Ensure WiFi Connection

1. Connect your phone to the same WiFi network as the ESP32
2. Find the ESP32's IP address (check your router or router settings)

### Step 2: Create Automation in MacroDroid

1. Open **MacroDroid** app
2. Tap **Create Macro** or **+** button
3. Name it: "Log Attendance"

### Step 3: Add Trigger

1. Select **Trigger** → **App/System Event** → **App Opened**
2. Choose your barcode scanner app
   - OR select **Generic / Sensor Event** if using clipboard-based scanning

### Step 4: Add Action - HTTP Request

1. Tap **Add Action**
2. Select **Network** → **HTTP Request** (or **Network Event**)
3. Configure:
   - **Method**: POST
   - **URL**: `http://[ESP32_IP]:3000/api/attendance`
     - Example: `http://192.168.1.150:3000/api/attendance`
   - **Headers**: Add header
     - Name: `Content-Type`
     - Value: `application/json`
   - **Body** (JSON):
     ```json
     {
       "student_id": "[STUDENT_ID]",
       "device_location": "Lab 1"
     }
     ```
4. **Enable macro** ✓

### Step 5: Test

1. Scan a barcode (e.g., `CCT/00001/023`)
2. Check that:
   - Dashboard updates with attendance
   - Backend logs show the entry

### Advantages

- Works over WiFi (longer range)
- Can be fully automated
- No manual sending required

---

## Finding ESP32 IP Address

If you don't know the ESP32's IP address:

### Option 1: Check Router Admin Panel

1. Open browser and go to your router's IP (usually 192.168.1.1 or 192.168.0.1)
2. Login with admin credentials
3. Look for "Connected Devices" or "DHCP Clients"
4. Find device named "esp32" or the MAC address

### Option 2: Use Serial Monitor

1. Connect ESP32 to computer via USB
2. Open Arduino IDE → Tools → Serial Monitor
3. Set baud rate to **115200**
4. Reboot ESP32 (press RESET button)
5. Watch for WiFi connection message showing IP address

### Option 3: Use Network Scanner

- Download a WiFi scanner app (e.g., "Network Analyzer" or "Fing")
- Scan your WiFi network
- Look for ESP32 or any device you just added

---

## Troubleshooting

### Bluetooth Connection Issues

- **ESP32 not appearing**: Check if Bluetooth is enabled on both devices
- **Pairing fails**: Forget the device and re-pair. Make sure PIN is `1234`
- **Connection drops**: Move closer to ESP32. Reboot ESP32 if persistent

### WiFi HTTP Issues

- **Cannot reach ESP32**:
  - Verify both phone and ESP32 are on same WiFi network
  - Check firewall/router settings
  - Verify correct IP address in MacroDroid URL
- **HTTP 400 errors**:
  - Verify student ID exists in database
  - Check JSON format in MacroDroid body

### Attendance Not Recording

1. Check Serial Monitor on ESP32 (baud: 115200)
2. Look for messages like `[BT] Received barcode: CCT/00001/023`
3. If error, check backend logs on PC
4. Verify student ID is already in the Students table

---

## Barcode Scanner Recommendations

### Built-in Android Camera (No App Needed)

- Use Google Lens or Android 12+ native barcode detection
- Simple but may require manual entry

### Free Barcode Scanner Apps

- **Barcode Scanner** by ZXing Team
- **QR Scanner** - ML Kit
- Copy scanned result to clipboard, then use with Bluetooth Serial Reader or MacroDroid

### Integrated QR/Barcode Generation

For testing, generate barcode images with student IDs using:

- Online: zxing.org/w/decode.jspx
- Generate QR codes with student IDs for quick testing

---

## Performance Notes

- **Bluetooth**: Best for ~10-20 scans/minute. Latency: 100-500ms
- **WiFi HTTP**: Best for 5-15 scans/minute (depends on network)
- Maximum: 1 scan/second without database issues

---

## Backend API Reference

### Endpoint

```
POST http://[ESP32_IP]:3000/api/attendance
```

### Request Body

```json
{
  "student_id": "CCT/00001/023",
  "device_location": "Lab 1",
  "device_id": "AA:BB:CC:DD:EE:FF"
}
```

### Response

- **Success (200/201)**:
  ```json
  { "id": 123, "timestamp": "2026-03-30T14:30:00.000Z" }
  ```
- **Error (400)**:
  ```json
  { "error": "unknown student_id" }
  ```

---

## Security Considerations

⚠️ **Current Implementation is for Development/Testing**

For production:

1. Add authentication to attendance API
2. Use HTTPS instead of HTTP
3. Implement rate limiting (prevent duplicate submissions)
4. Add device registration/whitelisting
5. Use stronger Bluetooth PIN or passkey
6. Encrypt student data transmission
7. Log all access attempts

See `Documentation/SECURITY.md` for detailed recommendations.
