# Initial Test Cases (MVP)

1. **GET /api/attendance returns 200 list** – fresh DB may return `[]` (or seeded demo data after first run).
2. **POST /api/attendance without student_id returns 400**.
3. **POST /api/attendance with unknown student_id returns 400**.
4. **POST /api/attendance with valid student_id returns 201** and an id.
5. **POST /api/attendance duplicate scan within 2 minutes returns 409**.
6. **GET /api/students returns 200 list** – includes seeded demo students on first run.
7. **GET /api/students/:student_id returns 200** for existing student (e.g., `S1001`).
8. **GET /api/students/:student_id returns 404** for missing student.
9. **POST /api/students with missing fields returns 400**.
10. **POST /api/students adds a new student** – followed by GET should include the student.
11. **Frontend loads attendance from backend** – open dashboard and verify table populates and "Last updated" changes.
12. **Frontend "Info" button fetches student details** – shows name/course where present.
13. **Failure case: backend down** – dashboard should show console error and recover after backend restarts.

Next (to reach 20+ as per academic guide): add explicit cases for DB file corruption, simulated Wi‑Fi dropout buffering on ESP32, and MQTT path once implemented.
