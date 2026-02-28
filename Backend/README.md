# Backend Service (Node.js + Express)

This folder contains the MVP backend for the smart attendance system. It uses a simple SQLite database (file `attendance.db`) to avoid external dependencies during early development.

## Quick start

```bash
cd Backend
npm install
npm run start       # or npm run dev with nodemon
```

The server exposes:

- `GET /api/students` – list students
- `POST /api/students` – add student (lecturer mode in future)
- `GET /api/attendance` – retrieve recent attendance logs
- `POST /api/attendance` – record an attendance event (called by ESP32 or manual)

Static files from the frontend folder are served automatically so you can view the dashboard by navigating to `http://localhost:3000`.
