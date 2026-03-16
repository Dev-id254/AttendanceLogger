# PROJECT_OVERVIEW.md — Smart Attendance System (ESP32 + Web Stack)

## Project Summary (MVP)
This project is a **Smart Attendance System** that captures a student’s ID from a barcode/QR scan, sends it from an **ESP32** over Wi‑Fi to a **backend REST API**, stores it in a **relational database**, and shows recent attendance logs on a **web dashboard**.
In the current MVP demo, the barcode scan is simulated via **Serial input** on the ESP32 (or via manual API calls), the backend stores records in **SQLite** for quick setup, and the dashboard auto-refreshes to show new logs.
Primary users are **lecturers/admins** (view attendance) and **students** (being recorded). The viva/demo story is: “scan ID → API validates → DB stores → dashboard updates”.

## Current Repository Status

### What is implemented vs. missing
- **Implemented now**
  - **Backend API (Node.js + Express)** with SQLite persistence: `Backend/server.js`, `Backend/routes/*`, `Backend/data.js`
  - **Web dashboard (HTML/CSS/JS)** that fetches and displays attendance: `FrontEnd/Dashboard.html`, `FrontEnd/Dashboard.js`, `FrontEnd/Dashboard.css`
  - **Firmware sketch (Arduino framework)** showing Wi‑Fi + HTTP POST: `Firmware/attendance/attendance.ino`
  - **Normalized SQL schema (MySQL-style)** for the “target” relational model: `Database/schema.sql`, `Database/seed.sql`
- **Currently missing (but planned in `project_context_prompt.md`)**
  - **Authentication/roles** (JWT / lecturer login)
  - **MQTT ingestion path** to backend (firmware has a placeholder MQTT client, but backend does not subscribe)
  - **Offline buffering on ESP32** (SPIFFS queue) for Wi‑Fi dropouts
  - **Full UML artefacts** (only placeholders exist): `Documentation/UML.md`
  - **20+ test cases** (repo currently has a starter list; expand to 20 for the guide)

### Major folders/files and purpose (tree)
```
StudentLogger/
  Backend/                 # Node.js + Express API, SQLite DB file created at runtime
    server.js              # Express app entry point + static serving
    data.js                # SQLite connection + schema initialization + demo seed
    routes/
      attendance.js        # GET/POST attendance
      students.js          # GET/POST students + GET by id
  FrontEnd/                # Static dashboard UI
    Dashboard.html         # Main dashboard page
    Dashboard.js           # Fetch + render attendance, student info popup
    Dashboard.css          # Styling
  Firmware/                # ESP32 (Arduino framework) sketch
    attendance/attendance.ino
  Database/                # MySQL schema + seed aligned to 3NF target
    schema.sql
    seed.sql
  Tests/
    test_cases.md          # starter test list (expandable)
  project_context_prompt.md # North-star architecture/spec
```

## Architecture (Aligned to project_context_prompt.md)
The intended architecture is a **4-tier N-tier** system:
- **Presentation layer**: browser dashboard (`FrontEnd/`)
- **Application/Business logic**: backend REST API (`Backend/`)
- **Data layer**: relational DB (MVP uses SQLite; target schema provided for MySQL in `Database/`)
- **Edge/Embedded layer**: ESP32 firmware (`Firmware/`)

### Communication paths (HTTP/MQTT) and what is actually implemented right now
- **UI → Backend**: **HTTP REST** (implemented) via `fetch('/api/attendance')` in `FrontEnd/Dashboard.js`
- **Backend → DB**: **SQLite** (implemented) via `sqlite3` in `Backend/data.js`
- **ESP32 → Backend**: **HTTP POST** (implemented) to `POST /api/attendance` in `Firmware/attendance/attendance.ino`
- **ESP32 → Backend via MQTT**: **CURRENTLY MISSING** on the backend side (firmware contains an MQTT client placeholder, but the backend does not run an MQTT subscriber)

## Data Flow (End-to-End)
### Current MVP flow (implemented)
1. **Barcode scanned**: firmware reads a line from Serial (acts like scanner output).
2. **ESP32 sends event**: firmware `HTTP POST` to backend `POST /api/attendance` with `student_id` (and optional device metadata).
3. **Backend validates**:
   - Rejects missing `student_id` with **400**
   - Rejects unknown `student_id` with **400**
   - Rejects duplicates within ~2 minutes with **409** (simple “anti double-scan”)
4. **DB stores record**: backend inserts into `Attendance` table in SQLite.
5. **Dashboard displays**: browser calls `GET /api/attendance` and renders the latest 100 logs.

### Missing parts (simplest defensible MVP extensions)
- **MQTT path (CURRENTLY MISSING)**: run Mosquitto locally, have backend subscribe to a topic like `attendance/events`, and map incoming messages to the same DB insert logic as `POST /api/attendance`.
- **Offline buffering (CURRENTLY MISSING)**: queue scan events into SPIFFS (JSON lines) when `WiFi.status()!=WL_CONNECTED`, flush on reconnection.

## Tech Stack (As-Is)
- **Frontend**: HTML5, CSS3, Vanilla JavaScript (`FrontEnd/`)
- **Backend**: Node.js + Express, CORS (`Backend/`)
- **Database (MVP runtime)**: SQLite (`sqlite3` npm package), file `Backend/attendance.db` created at runtime
- **Database (target schema docs)**: MySQL SQL scripts (`Database/schema.sql`, `Database/seed.sql`)
- **Firmware**: ESP32 Arduino framework (`WiFi.h`, `HTTPClient.h`, optional `PubSubClient`)
- **OS/Dev**: Windows 10 (repo includes `run_backend.bat`)

## Key Modules & Responsibilities

### Frontend
- **Entry point**: `FrontEnd/Dashboard.html`
- **Logic**: `FrontEnd/Dashboard.js`
  - Loads logs from `GET /api/attendance`
  - Auto-refreshes every 10 seconds for demo-readiness
  - “Info” button fetches student details from `GET /api/students/:student_id`
- **How to run**:
  - With backend: open `http://localhost:3000`
  - Or open `FrontEnd/Dashboard.html` directly (API calls require backend running)

### Backend API
- **Entry point**: `Backend/server.js`
- **DB initialization**: `Backend/data.js` (creates tables + seeds demo data)
- **Routes**:
  - `Backend/routes/attendance.js`
  - `Backend/routes/students.js`
- **How to run**:
  - `cd Backend && npm install && npm run start`

### Database
- **MVP runtime DB**: SQLite tables created by `Backend/data.js`
  - `Students(student_id, name, course_id)`
  - `Courses`, `Lecturers`, `Devices`, `Attendance`
- **Target relational schema (3NF direction)**: `Database/schema.sql` (+ seed in `Database/seed.sql`)

### ESP32 firmware
- **Sketch**: `Firmware/attendance/attendance.ino`
- **Functionality**:
  - Connect to Wi‑Fi
  - Read scanned codes from Serial
  - POST attendance to backend
- **How to run/build**:
  - Open in Arduino IDE / PlatformIO
  - Update `ssid`, `password`, and `backendUrl` to your PC’s LAN IP

## API Contract (If applicable)
Base URL (local): `http://localhost:3000`

### `GET /api/attendance`
- **Response 200**: array of last 100 attendance records (newest first)
- **Example fields**: `id`, `student_id`, `student_name`, `course_id`, `course_name`, `timestamp`, `device_mac`

### `POST /api/attendance`
- **Request body** (JSON):
  - **Required**: `student_id` (string)
  - Optional: `course_id` (number), `device_id` (number), `device_mac` (string), `device_location` (string)
- **Responses**
  - **201** `{ "id": <number> }`
  - **400** `{ "error": "student_id required" | "unknown student_id" }`
  - **409** `{ "error": "duplicate_scan_recent", "existing": { ... } }`

### `GET /api/students`
- **Response 200**: array of students

### `GET /api/students/:student_id`
- **Response 200**: student details (includes `course_name` when available)
- **Response 404**: `{ "error": "student_not_found" }`

### `POST /api/students`
- **Request body**: `student_id` (string), `name` (string), optional `course_id` (number)
- **Response 201**: `{ "id": <number> }`
- **Response 400**: `{ "error": "student_id and name required" }`

## Database Schema (If applicable)

### Target schema (MySQL scripts)
See `Database/schema.sql` and `Database/seed.sql`.
- **3NF direction**: separate `Students`, `Courses`, `Lecturers`, `Devices`, `Attendance`
- **Foreign keys**: attendance references students/courses/devices through the backend (ESP32 never writes to DB directly)

### MVP runtime (SQLite)
Created in `Backend/data.js`. Key points:
- **Keying**: students keyed by `student_id` (unique).
- **Deduplication (MVP)**: backend blocks repeated scans for same `(student_id, course_id)` within ~2 minutes (application-level rule for demo integrity).

### Example rows (conceptual)
- Students: (`S1001`, `John Doe`, course `1`)
- Attendance: (`S1001`, course `1`, timestamp, device `1`)

## How to Run / Demo (Step-by-Step)

### Prerequisites
- **Node.js** (LTS recommended)
- (Optional) Arduino IDE / PlatformIO for firmware

### Backend (Windows)
1. From repo root, run `run_backend.bat` or:
   - `cd Backend`
   - `npm install`
   - `npm run start`
2. Open `http://localhost:3000` in a browser.

### Happy-path demo script (1–2 minutes)
1. Show the dashboard (initially has 0 or a few logs).
2. Add (or confirm) a student exists: `S1001` (already seeded by backend).
3. Simulate a scan by sending `S1001` to the ESP32 over Serial (or use an API client to `POST /api/attendance`).
4. Refresh (or wait ~10 seconds) and show the new row appear on the dashboard.
5. Click **Info** to show student details returned from the backend.

## Testing (MVP)
Quick test plan (10 now; expand to 20+ for the guide):
1. **GET /api/attendance returns 200** and an array.
2. **POST /api/attendance without student_id → 400**.
3. **POST /api/attendance with unknown student_id → 400**.
4. **POST /api/attendance with known student_id → 201** and returns an id.
5. **Duplicate scan within 2 minutes → 409**.
6. **GET /api/students returns seeded students**.
7. **GET /api/students/:id for existing student → 200**.
8. **GET /api/students/:id for missing student → 404**.
9. **Dashboard loads rows** (no console errors; table updates).
10. **Network drop simulation**: stop backend; dashboard should log fetch failure and recover after restart.

## Viva Defense Notes
- **Why ESP32?**
  - Built-in Wi‑Fi, FreeRTOS support, adequate RAM/CPU for connectivity + buffering; strong fit for edge node.
- **Why backend-mediated DB writes (not ESP32 → DB)?**
  - Central validation + security, prevents direct DB exposure, consistent business rules, easier scaling and auditing.
- **HTTP vs MQTT choice**
  - HTTP is simplest and demo-friendly; MQTT is better for real-time pub/sub and multi-device scaling. This repo implements HTTP first, with MQTT planned.
- **Offline buffering approach**
  - Buffer scan events locally on ESP32 (SPIFFS) when Wi‑Fi is down; flush later to preserve integrity.
- **Scalability story**
  - Multiple ESP32 devices post/ publish to a central backend; DB indexing + topic partitioning (MQTT) supports multiple lecture halls.

## Known Gaps + Next Steps (Minimal Roadmap)
- **Add MQTT ingestion** in backend (subscribe and reuse attendance insert logic).
- **Add authentication** (JWT) + lecturer role to protect write endpoints.
- **Implement ESP32 buffering** for Wi‑Fi dropouts.
- **Produce UML diagrams** as images linked from `Documentation/`.
- **Expand test cases to 20+** with integration tests and failure-mode coverage.

