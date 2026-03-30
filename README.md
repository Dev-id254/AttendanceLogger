# Smart Attendance System (MVP)

Current methods in place used to log students' attendance & identity confirmation in most universities in Kenya are manual and full of potential errors. We intend to fix this by building a four-tier architecture that integrates an ESP32 node with a web dashboard and a relational database.

The MVP in this repository includes:

- A **frontend** HTML/CSS/JavaScript dashboard (`FrontEnd/`) that displays attendance logs fetched from a backend API.
- A **Node.js + Express** backend (`Backend/`) exposing simple REST endpoints (`/api/students`, `/api/attendance`). Data is stored in a lightweight SQLite database for ease of setup.
- A **database schema** (`Database/schema.sql` and `seed.sql`) showing the normalized tables (Students, Courses, Attendance, etc.) and sample seed data for MySQL.
- **ESP32 firmware** (`Firmware/attendance.ino`) with **Bluetooth and WiFi support** to receive attendance data from a mobile device instead of a physical scanner. **Written in Arduino C++** for easy development and flashing.

## Architecture Overview (Updated)

### Data Flow

1. **Mobile Phone** scans student ID barcodes using a barcode scanner app
2. **Phone** sends barcode data to ESP32 via:
   - **Bluetooth Serial** (using "Bluetooth Serial Reader" app)
   - **WiFi HTTP POST** (using "MacroDroid" automation or direct API calls)
3. **ESP32** receives barcode (student ID) and forwards to backend via HTTP POST
4. **Backend** validates student, records attendance in SQLite
5. **Dashboard** displays real-time attendance logs

### No Physical Scanner Required

- ✅ Scanner module completely removed
- ✅ Phone acts as input device (cost-effective, flexible)
- ✅ Works offline (Bluetooth mode) or online (WiFi mode)
- ✅ Supports automation (MacroDroid for hands-free operation)

This MVP provides a foundation for implementing authentication, enhanced security, and production deployment.

## Setup (Windows)

1. **Clone the repo** and `cd` into the `StudentLogger` folder.

2. **Backend**:
   - Navigate to `Backend` and run `npm install` (requires Node.js).
   - Start the server with `npm run start` (port 3000 by default).
   - The API will create an SQLite database file `attendance.db` automatically.

3. **Frontend**:
   - Open `FrontEnd/Dashboard.html` in a browser or navigate to `http://localhost:3000` once the backend is running.
   - The page will fetch attendance records from the API and display them.

4. **ESP32 Firmware**:
   - Open `Firmware/attendance.ino` in **Arduino IDE** or **PlatformIO** (VS Code extension)
   - Update configuration:
     - `ssid`: Your WiFi network name
     - `password`: Your WiFi password
     - `backendUrl`: PC's local IP address (e.g., `http://192.168.1.100:3000/api/attendance`)
     - `bluetoothName`: ESP32 Bluetooth device name (default: `ESP32_Attendance`)
   - Flash to ESP32 dev board
   - Open Serial Monitor (baud 115200) to verify WiFi and Bluetooth initialization

5. **Mobile Phone Setup** ⭐ (NEW):
   - See [`Documentation/PHONE_SETUP_GUIDE.md`](Documentation/PHONE_SETUP_GUIDE.md) for complete instructions
   - Required apps:
     - **Bluetooth Serial Reader** (for Bluetooth mode)
     - **MacroDroid** (for WiFi automation mode)
     - Barcode scanner app of your choice
   - Pair phone with ESP32 via Bluetooth
   - Test by scanning a student ID barcode

6. **Database** (optional):
   - Use MySQL or PostgreSQL to execute `Database/schema.sql` and optionally `seed.sql` when migrating to a production DB.

## Key Files

| File                                 | Purpose                                                    |
| ------------------------------------ | ---------------------------------------------------------- |
| `Firmware/attendance.ino`            | ESP32 firmware (Arduino C++) with Bluetooth + WiFi support |
| `Backend/server.js`                  | Express server with attendance API                         |
| `Backend/routes/attendance.js`       | POST endpoint for recording attendance                     |
| `FrontEnd/Dashboard.html/js/css`     | Real-time attendance dashboard                             |
| `Documentation/PHONE_SETUP_GUIDE.md` | **Setup instructions for phone integration**               |
| `Database/schema.sql`                | Database schema for production deployment                  |

## Firmware Development Notes

**Current Framework:** Arduino C++ (PlatformIO/Arduino IDE)

The firmware is written in **Arduino C++** for simplicity and ease of development. Legacy ESP-IDF files (`CMakeLists.txt`, `build.sh`, `ESP_IDF_BUILD_GUIDE.md`, etc.) remain in the repository for reference but are not used for building the current firmware.

**Why Arduino C++:**

- ✅ Simple setup/loop architecture
- ✅ Easy flashing with Arduino IDE
- ✅ Familiar syntax for most developers
- ✅ Good performance for this use case
- ✅ Cross-platform development

**Supported Boards:**

- ESP32 Dev Module
- ESP32-S3 Dev Module (same code, just select different board in IDE)

## Testing Quick Start

1. Start backend: `npm start` (from Backend folder)
2. Open Dashboard: `http://localhost:3000`
3. Flash ESP32 with updated firmware
4. Pair phone with ESP32 via Bluetooth
5. Scan a barcode using Bluetooth Serial Reader app
6. Verify attendance appears on Dashboard

## Troubleshooting

**Attendance not recording?**

- Check ESP32 Serial Monitor (115200 baud) for debug messages
- Verify student ID exists in database
- Ensure WiFi connection on ESP32
- Check backend logs: `node Backend/server.js`

**Bluetooth not connecting?**

- Verify `ESP32_Attendance` appears in phone's Bluetooth settings
- Default PIN: `1234`
- Move closer to ESP32 (Bluetooth range ~10-100 meters)

**WiFi not connecting?**

- Verify WiFi credentials in firmware
- Check firewall allows port 3000
- Get ESP32 IP from Serial Monitor or router admin panel

## Next Steps for Full Project

- Implement user authentication (JWT) and role-based access
- Add rate limiting and duplicate detection
- Enhance security (HTTPS, encrypted Bluetooth)
- Implement offline queueing (store records locally if WiFi fails)
- Add student management UI (lecturer mode)
- Create test suite and UML diagrams
- Deploy to production database (MySQL/PostgreSQL)

---

For detailed phone setup and integration troubleshooting, see [`Documentation/PHONE_SETUP_GUIDE.md`](Documentation/PHONE_SETUP_GUIDE.md).
