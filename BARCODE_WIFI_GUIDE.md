# Steps to Scan & Log Admission Numbers via WiFi (Phone → ESP32)

## Overview
This guide walks you through scanning barcodes on your phone and sending admission numbers to the ESP32 via WiFi, which then logs them in the database.

---

## Architecture Flow
```
Phone (Barcode Scanner) 
    ↓ (WiFi HTTP POST)
ESP32 (HTTP Server on port 80)
    ↓ (HTTP POST to Backend)
Backend API (port 3000)
    ↓ (Store in SQLite)
Database (attendance.db)
    ↓
Dashboard (http://localhost:3000)
```

---

## 🔧 STEP 1: Setup & Configuration

### 1.1 Configure ESP32 Firmware

Edit [Firmware/attendance/attendance.cpp](Firmware/attendance/attendance.cpp#L114-L116):

```cpp
// Line 114-116: Update WiFi credentials
const char *ssid = "YourSSID";                    // Replace with your WiFi name
const char *password = "YourPassword";            // Replace with your WiFi password
const char *backendUrl = "http://192.168.1.100:3000/api/attendance";  // Replace with your PC's IP
const char *deviceLocation = "Lab 1";             // Location label (optional)
```

**To find your PC's IP address on Windows:**
```powershell
ipconfig
# Look for "IPv4 Address" under your active network adapter
# Example: 192.168.1.100
```

### 1.2 Flash Firmware to ESP32

Option A: Using Arduino IDE
```
1. Open Firmware/attendance/attendance.cpp in Arduino IDE
2. Select Board: ESP32 (or ESP32-S3)
3. Select Port: COM3 (or your ESP32's port)
4. Click Upload (→)
5. Wait for "Done uploading" message
```

Option B: Using PlatformIO
```bash
cd Firmware
platformio run --target upload
```

### 1.3 Verify ESP32 is Running

1. Open Serial Monitor (115200 baud) in Arduino IDE
2. You should see:
   ```
   WiFi connecting...
   WiFi connected to AP
   WiFi connected with IP: 192.168.1.150
   HTTP server started on port 80
   BLE initialized: ESP32_Attendance
   ```
3. **Note the ESP32's IP address** (e.g., 192.168.1.150) for use in phone steps

---

## 🚀 STEP 2: Start Backend API

The ESP32 will POST data to your backend, which stores it in the database.

### 2.1 Start Backend Server

```bash
cd Backend
npm install          # If first time
npm start
# Server runs on port 3000
```

Output:
```
Attendance backend listening on port 3000
```

### 2.2 Verify Backend is Running

Open in browser:
```
http://localhost:3000
```
You should see the attendance dashboard.

---

## 📱 STEP 3: Setup Phone (WiFi Method)

### Option A: MacroDroid Automation (Recommended)

**Requirements:**
- Android phone with WiFi
- MacroDroid app (Google Play Store)
- Barcode scanner app (built-in camera or "Barcode Scanner+" app)

#### Step 1: Connect Phone to WiFi
1. On your phone: Settings → WiFi
2. Connect to the **same WiFi network** as your ESP32
3. Note the ESP32's IP (e.g., 192.168.1.150)

#### Step 2: Create Automation in MacroDroid
1. Open **MacroDroid** app
2. Tap **"Create Macro"** or **"+"** button
3. Name it: `"Log Attendance"`

#### Step 3: Add Trigger
1. Tap **"Add Trigger"**
2. Select **"App/System Event"** → **"App Opened"**
3. Choose your barcode scanner app (or choose a clipboard trigger)

#### Step 4: Add Action - HTTP Request
1. Tap **"Add Action"**
2. Select **"Network"** → **"HTTP Request"**
3. Configure:
   ```
   Method:           POST
   URL:              http://192.168.1.150/api/attendance
   Content-Type:     application/json
   Body (JSON):
   {
     "student_id": "[SCANNED_BARCODE]"
   }
   ```

#### Step 5: Test the Scan
1. Scan an admission number using your barcode scanner app
2. The barcode should automatically POST to ESP32
3. Check Serial Monitor to see: `[HTTP] Received POST: CCT/00001/023`
4. Check dashboard at `http://localhost:3000` for the logged entry

---

### Option B: Manual HTTP Request (Testing)

Use a phone browser or HTTP client app to test:

```
URL: http://192.168.1.150/api/attendance

Headers:
  Content-Type: application/json

Body:
{
  "student_id": "CCT/00001/023"
}
```

---

## 📊 STEP 4: Verify Integration

### 4.1 Check ESP32 Serial Monitor

When a barcode is sent, you should see:
```
[HTTP] Received POST: CCT/00001/023
[HTTP] Forwarding to backend: http://192.168.1.100:3000/api/attendance
[HTTP] Backend response: 200 OK
```

### 4.2 Check Backend Console

```
POST /api/attendance
Body: { student_id: 'CCT/00001/023', device_location: 'Lab 1' }
Response: 200 OK - Attendance recorded
```

### 4.3 Check Dashboard

1. Open: `http://localhost:3000`
2. You should see the new attendance entry in the table
3. The entry should refresh automatically every 10 seconds

---

## 🔍 STEP 5: Troubleshooting

### Problem: ESP32 Won't Connect to WiFi

**Solution:**
1. Verify WiFi SSID and password in firmware (case-sensitive)
2. Ensure 2.4 GHz WiFi (ESP32 doesn't support 5 GHz on most boards)
3. Check signal strength near ESP32
4. Recompile and re-upload firmware

### Problem: Phone Can't Reach ESP32

**Solution:**
1. Ensure phone is on **same WiFi network** as ESP32
2. Ping ESP32 from Windows:
   ```bash
   ping 192.168.1.150
   ```
3. If no response:
   - Check ESP32 is powered on
   - Restart ESP32
   - Check WiFi SSID in firmware matches your network

### Problem: Barcode Sent but Not Logged

**Solution:**
1. Check ESP32 Serial Monitor for `[HTTP] Forw...` message
2. Verify `backendUrl` in firmware matches your PC's IP
3. Ensure backend is running (`npm start`)
4. Test backend directly:
   ```bash
   curl -X POST http://192.168.1.100:3000/api/attendance \
     -H "Content-Type: application/json" \
     -d '{"student_id":"CCT/00001/023"}'
   ```

### Problem: Dashboard Shows No Data

**Solution:**
1. Ensure backend is running
2. Check database file exists: `Backend/attendance.db`
3. Refresh dashboard (Ctrl+Shift+R for hard refresh)
4. Check browser console for errors (F12 → Console tab)

---

## 📝 API Request Format

### POST to ESP32 (Phone sends this)

```http
POST /api/attendance HTTP/1.1
Host: 192.168.1.150:80
Content-Type: application/json

{
  "student_id": "CCT/00001/023",
  "device_location": "Lab 1"
}
```

### POST to Backend (ESP32 forwards this)

```http
POST /api/attendance HTTP/1.1
Host: 192.168.1.100:3000
Content-Type: application/json

{
  "student_id": "CCT/00001/023",
  "device_location": "Lab 1",
  "device_id": "aa:bb:cc:dd:ee:ff"
}
```

---

## ✅ Quick Checklist

- [ ] WiFi credentials updated in `Firmware/attendance/attendance.cpp`
- [ ] ESP32's PC IP address updated in `backendUrl`
- [ ] Firmware uploaded to ESP32
- [ ] Backend running: `npm start`
- [ ] Phone connected to same WiFi as ESP32
- [ ] MacroDroid automation configured
- [ ] Test barcode scanned and logged
- [ ] Entry visible on dashboard

---

## 🚨 Important Notes

1. **ESP32 HTTP Server (Port 80)**: Listens for incoming barcode data from phone
2. **Backend API (Port 3000)**: ESP32 forwards requests here for storage
3. **Make sure firewall allows**: Port 80 (ESP32) and Port 3000 (Backend) on your PC
4. **Phone must be on same WiFi**: The phone and ESP32 must be on the same network
5. **Student ID must exist**: The scanned admission number must be in the `students` table in the database

---

## Testing Flow (End-to-End)

```
1. Scan barcode on phone
   ↓
2. Phone sends HTTP POST to ESP32:80
   ↓
3. ESP32 receives POST on /api/attendance
   ↓
4. ESP32 validates & forwards to Backend:3000
   ↓
5. Backend stores in attendance.db
   ↓
6. Dashboard auto-refreshes and displays entry
```

---

## Next Steps

Once this is working:
1. **Add authentication**: Implement lecturer login for security
2. **Add offline buffering**: Queue scans on ESP32 when WiFi drops
3. **Add QR codes**: Support QR code scanning
4. **Add statistics**: Show attendance charts and analytics

