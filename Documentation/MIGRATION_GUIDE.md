# Migration Guide: Scanner Module → Phone Input

This document explains the architectural changes made to remove the physical barcode scanner module and integrate Bluetooth/WiFi phone input.

---

## Why This Change?

### Previous Architecture

- **Hardware**: Physical barcode scanner connected to ESP32 UART
- **Cost**: $50-100 for quality scanner hardware
- **Flexibility**: Limited to single location
- **Maintenance**: Scanner breakdowns disrupt service

### New Architecture

- **Hardware**: Any smartphone with barcode scanner app
- **Cost**: $0 (uses existing student phones)
- **Flexibility**: Can be used anywhere, multiple locations simultaneously
- **Maintenance**: Software updates via app, no hardware maintenance

---

## Code Changes

### Firmware (attendance.ino)

#### Removed

```cpp
// Old: MQTT support (unused)
#include <PubSubClient.h>
PubSubClient mqttClient(espClient);
mqttClient.setServer("broker.hivemq.com", 1883);
mqttClient.loop();
mqttReconnect();
```

#### Added

```cpp
// New: Bluetooth Serial
#include <BluetoothSerial.h>
BluetoothSerial SerialBT;

void setup() {
  SerialBT.begin("ESP32_Attendance");  // Initialize Bluetooth
}

void loop() {
  if (SerialBT.available()) {
    String code = SerialBT.readStringUntil('\n');  // Read from Bluetooth
    sendAttendance(code);
  }
}
```

#### Changed

```cpp
// Old: serial.readStringUntil() - could be from any serial source
String code = Serial.readStringUntil('\n');

// New: explicitly Bluetooth serial
String code = SerialBT.readStringUntil('\n');
```

#### Improved Logging

```cpp
// Old: Generic logging
Serial.printf("Read barcode: %s", code.c_str());

// New: Specific logging with source
Serial.printf("[BT] Received barcode: %s\n", code.c_str());
SerialBT.printf("OK: Attendance recorded for %s\n", studentId.c_str());
```

### Backend (server.js, routes/attendance.js)

**No changes required** - Already designed to accept student IDs via HTTP POST

- Accepts POST requests with student_id
- Validates student exists
- Records timestamp and device info
- Returns JSON response

### Frontend (Dashboard.html/js)

**No changes required** - Already fetches and displays attendance records

- Polls /api/attendance every 10 seconds
- Works with new data format immediately

### Database (schema.sql)

**No changes required** - Schema already supports multiple input sources

- Attendance table already has device_id field
- Can track which ESP32/location recorded attendance
- Student authentication independent of input method

---

## Configuration Changes

### WiFi Configuration

**Unchanged** - Still needed for backend communication

```cpp
const char* ssid = "YourSSID";
const char* password = "YourPassword";
const char* backendUrl = "http://192.168.1.100:3000/api/attendance";
```

### Bluetooth Configuration

**New** - Configure for your deployment

```cpp
const char* bluetoothName = "ESP32_Attendance";      // Device name (phone sees this)
const char* bluetoothPin = "1234";                   // Pairing PIN
```

### Device Location

**Still needed** - Labels where device is deployed

```cpp
const char* deviceLocation = "Lab 1";  // For tracking which station recorded attendance
```

---

## Testing Migration

### Before (Old Scanner Setup)

```
1. Connect physical barcode scanner to ESP32 UART
2. Calibrate scanner
3. Test serial output
4. Monitor ESP32 for received codes
```

### After (New Phone Setup)

```
1. Configure WiFi in firmware
2. Flash ESP32
3. Install Bluetooth Serial Reader on phone
4. Pair phone with ESP32
5. Scan barcode - should appear on phone
6. Verify Dashboard updates
```

### Testing Checklist

- [ ] Bluetooth Serial Reader app installed on phone
- [ ] Phone paired with ESP32 (PIN 1234)
- [ ] WiFi connected on ESP32 (check Serial Monitor)
- [ ] Backend running on PC (port 3000)
- [ ] Dashboard accessible at http://localhost:3000
- [ ] Test barcode scanned and recorded:
  - [ ] Appears in Serial Monitor on ESP32
  - [ ] Shows in Dashboard within 10 seconds
- [ ] Multiple scans work (no duplicates expected yet)

---

## Rollback (If Needed)

If you need to revert to physical scanner:

1. Switch back to old attendance.ino (from git history)
2. Connect physical scanner to UART pins on ESP32
3. No backend/frontend changes needed
4. Firmware automatically reads Serial input

---

## Known Limitations

### Current Implementation

1. **No deduplication** - Same barcode twice = two attendance records
   - **Solution**: Add duplicate detection (1-minute window per student)
2. **No offline queueing** - WiFi drop loses the record
   - **Solution**: Queue IDs locally if WiFi unavailable
3. **Security** - Anyone with Bluetooth can connect
   - **Solution**: Implement PIN-based validation or device whitelisting
4. **Rate limiting** - No protection against spam
   - **Solution**: Add request rate limiting per student ID

All listed above are recommended for **production deployment**.

---

## Future Enhancements

### Short-term (v1.1)

- [ ] Add duplicate prevention (same student within 1 minute)
- [ ] Add offline queue (store up to 100 local records)
- [ ] Improve error feedback to phone
- [ ] Add LED status indicators

### Medium-term (v2.0)

- [ ] Real-time WebSocket updates to dashboard
- [ ] Student authentication (not just ID)
- [ ] Lecturer device management UI
- [ ] Multi-language app support

### Long-term (v3.0)

- [ ] NFC/RFID integration (alternative to barcode)
- [ ] Cloud sync (Firebase/AWS)
- [ ] Advanced analytics and reporting
- [ ] Mobile native app (replace MacroDroid)

---

## Files Affected

| File                           | Change         | Impact               |
| ------------------------------ | -------------- | -------------------- |
| `Firmware/attendance.ino`      | Major refactor | ✅ Functional change |
| `Backend/server.js`            | No change      | ✅ No issues         |
| `Backend/routes/attendance.js` | No change      | ✅ No issues         |
| `FrontEnd/Dashboard.html`      | No change      | ✅ No issues         |
| `Database/schema.sql`          | No change      | ✅ No issues         |
| `Documentation/`               | 4 new files    | ℹ️ Setup guidance    |

---

## Support & Troubleshooting

### Debugging Steps

1. **Check Serial Monitor** (115200 baud):

   ```
   Bluetooth initialized
   Bluetooth device name: ESP32_Attendance
   [BT] Received barcode: CCT/00001/023
   [HTTP] POST success: code 200
   ```

2. **Check phone Bluetooth**:
   - ESP32_Attendance appears in paired devices
   - Shows "Connected" in Bluetooth Serial Reader

3. **Check Dashboard**:
   - Records appear within 10 seconds of scan
   - Correct student ID and timestamp

### Common Issues

| Issue                         | Cause                        | Fix                                      |
| ----------------------------- | ---------------------------- | ---------------------------------------- |
| Bluetooth not found           | ESP32 not powered            | Power ESP32, wait 2 seconds              |
| Bluetooth connects then drops | Out of range or interference | Move closer, check for WiFi interference |
| Barcode not received          | App not connected            | Tap "Connect" in Bluetooth Serial Reader |
| Dashboard blank               | Backend not running          | Run `npm start` in Backend folder        |
| HTTP 400 error                | Unknown student ID           | Add student via API or UI first          |

---

For detailed setup instructions, see:

- [`PHONE_SETUP_GUIDE.md`](PHONE_SETUP_GUIDE.md)
- [`HARDWARE_CONFIGURATION.md`](HARDWARE_CONFIGURATION.md)
- [`API_REFERENCE.md`](API_REFERENCE.md)
