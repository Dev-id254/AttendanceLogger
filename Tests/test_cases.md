# Initial Test Cases (MVP)

1. **GET /api/attendance returns empty list** – fresh database should return `[]`.
2. **POST /api/students with missing fields returns 400** – validate error handling.
3. **POST /api/students adds a new student** – followed by GET should include the student.
4. **POST /api/attendance without student_id returns 400**.
5. **POST /api/attendance records event** – subsequent GET returns entry with timestamp.
6. **Frontend loads attendance from backend** – open dashboard and verify dynamic table populates.
7. **Lecturer mode button shows placeholder alert** – UI interaction.
8. **Info buttons show student id in alert** – event delegation check.

More test cases will cover authentication, database failures, Wi-Fi dropouts, multiple devices, etc., in later phases.
