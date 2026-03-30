# QUICK REFERENCE - Attendance Logger (Phone-Based)

## 📱 New Architecture

```
Phone + Barcode App
        ↓
   Bluetooth OR WiFi
        ↓
     ESP32 ← WiFi
        ↓
   Backend API
        ↓
     Dashboard
```

## 🚀 Quick Start

### 1. Start Backend

```bash
cd Backend
npm install
npm start
# Server runs on port 3000
```

### 2. Open Dashboard

```
http://localhost:3000
```

### 3. Configure ESP32

1. Open `Firmware/attendance.ino` in Arduino IDE
2. Update:
   - `ssid = "YourWiFiName"`
   - `password = "YourWiFiPassword"`
   - `backendUrl = "http://192.168.1.XXX:3000/api/attendance"` (use your PC's IP)
3. Flash to ESP32 board
4. Check Serial Monitor (115200 baud) for `Bluetooth initialized`

### 4. Setup Phone

See [`Documentation/PHONE_SETUP_GUIDE.md`](Documentation/PHONE_SETUP_GUIDE.md)

**Option A - Bluetooth** (Offline capable):

- Download "Bluetooth Serial Reader" app
- Pair with `ESP32_Attendance` (PIN: 1234)
- Scan barcodes in the app

**Option B - WiFi** (Automated):

- Download "MacroDroid" app
- Create automation to POST barcode to `http://[ESP32_IP]:3000/api/attendance`

### 5. Test

Scan a barcode → Should appear on Dashboard in 10 seconds

---

## 📝 Key Files

| File                                      | Purpose                         |
| ----------------------------------------- | ------------------------------- |
| `Firmware/attendance.ino`                 | 🆕 ESP32 with Bluetooth + WiFi  |
| `Backend/server.js`                       | Express API server              |
| `Backend/routes/attendance.js`            | POST endpoint (no changes)      |
| `FrontEnd/Dashboard.html`                 | Attendance display (no changes) |
| `Documentation/PHONE_SETUP_GUIDE.md`      | 🆕 Step-by-step phone setup     |
| `Documentation/HARDWARE_CONFIGURATION.md` | 🆕 ESP32 wiring & specs         |
| `Documentation/API_REFERENCE.md`          | 🆕 API endpoints & examples     |
| `Documentation/MIGRATION_GUIDE.md`        | 🆕 What changed & why           |

---

## 🔌 Removed Components

- ❌ Physical barcode scanner
- ❌ Scanner UART interface code
- ❌ MQTT support (unused)
- ❌ Hardware-specific drivers

---

## ✅ Added Components

- ✅ BluetoothSerial library
- ✅ Phone input support
- ✅ Better logging & feedback
- ✅ Complete documentation

---

## 🐛 Debugging

### Check ESP32

1. Serial Monitor → 115200 baud
2. Look for:
   ```
   Bluetooth initialized
   WiFi connected
   [BT] Received barcode: [ID]
   ```

### Check Phone

- Bluetooth Serial Reader shows "Connected"
- Barcode appears when scanned

### Check Backend

- Terminal shows `listening on port 3000`
- No error messages

### Check Dashboard

- Records appear within 10 seconds
- Correct student names shown

---

## 🔐 Security Notes (For Production)

⚠️ Current version is development/MVP only. Recommended for production:

1. ❌ Add authentication (JWT tokens)
2. ❌ Use HTTPS instead of HTTP
3. ❌ Add rate limiting (prevent spam)
4. ❌ Add duplicate detection (same ID within 1 min)
5. ❌ Whitelist devices (only specific ESP32s)
6. ❌ Encrypt Bluetooth communication

See `Documentation/` for detailed recommendations.

---

## 📊 API Endpoint

**POST** attendance record:

```bash
curl -X POST http://192.168.1.150:3000/api/attendance \
  -H "Content-Type: application/json" \
  -d '{"student_id":"CCT/00001/023","device_location":"Lab 1"}'
```

**GET** all records:

```bash
curl http://192.168.1.150:3000/api/attendance
```

See `Documentation/API_REFERENCE.md` for complete API docs.

---

## 📚 Documentation Map

```
Documentation/
├── PHONE_SETUP_GUIDE.md      ← START HERE (phone setup)
├── HARDWARE_CONFIGURATION.md ← For ESP32 technical details
├── API_REFERENCE.md          ← For backend integration
├── MIGRATION_GUIDE.md        ← Understand what changed
└── UML.md                    ← System architecture diagram
```

---

## 💡 Typical Workflow

1. **Lecturer brings phone** to class with Bluetooth Serial Reader or MacroDroid
2. **Students scan their ID cards** using phone barcode scanner
3. **Phone sends barcode** to ESP32 (Bluetooth or WiFi)
4. **ESP32 forwards to backend** via HTTP POST
5. **Dashboard updates** - attendance recorded
6. **End of class** - attendance log complete

No scanner hardware. No manual entry. Done!

---

## 🎯 Next Steps

- [ ] Test with real student IDs
- [ ] Set up second ESP32 in different location
- [ ] Add lecturer authentication
- [ ] Implement duplicate detection
- [ ] Add offline queue capability
- [ ] Deploy to production database

---

For detailed information, see the full documentation in the `Documentation/` folder.
