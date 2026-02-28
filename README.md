# Smart Attendance System (MVP)

Current methods in place used to log students' attendance & identity confirmation in most universities in Kenya are manual and full of potential errors. We intend to fix this by building a four-tier architecture that integrates an ESP32 node with a web dashboard and a relational database.

The MVP in this repository includes:

- A **frontend** HTML/CSS/JavaScript dashboard (`FrontEnd/`) that displays attendance logs fetched from a backend API.
- A **Node.js + Express** backend (`Backend/`) exposing simple REST endpoints (`/api/students`, `/api/attendance`). Data is stored in a lightweight SQLite database for ease of setup.
- A **database schema** (`Database/schema.sql` and `seed.sql`) showing the normalized tables (Students, Courses, Attendance, etc.) and sample seed data for MySQL.
- **ESP32 firmware example** (`Firmware/attendance.ino`) demonstrating Wi‑Fi connectivity and HTTP POST to the backend.

This MVP can be run locally with minimal dependencies. It provides a foundation for implementing authentication, real‑time MQTT, and full module integration described in the project prompt.

## Setup (Windows)

1. **Clone the repo** and `cd` into the `StudentLogger` folder.
2. **Backend**:
   - Navigate to `Backend` and run `npm install` (requires Node.js).
   - Start the server with `npm run start` (port 3000 by default).
   - The API will create an SQLite database file `attendance.db` automatically.
3. **Frontend**:
   - Open `FrontEnd/Dashboard.html` in a browser or navigate to `http://localhost:3000` once the backend is running.
   - The page will fetch attendance records from the API and display them.
4. **Database** (optional):
   - Use MySQL or PostgreSQL to execute `Database/schema.sql` and optionally `seed.sql` when migrating to a production DB.
5. **ESP32 firmware**:
   - Open `Firmware/attendance.ino` in Arduino IDE or PlatformIO to flash an ESP32 dev board. Update `ssid`, `password`, and `backendUrl` accordingly.

## Next steps for full project

- Implement user authentication (JWT) and role-based access.
- Add MQTT broker support for real-time telemetry.
- Develop FreeRTOS-based firmware with USB HID scanner integration.
- Write 20+ test cases and create UML diagrams as per academic requirements.

---

This README will expand as the system evolves through incremental integration phases.
