# API Reference - Attendance Logger

## Base URL

```
http://[ESP32_IP]:3000
```

Example: `http://192.168.1.150:3000`

---

## Attendance Endpoints

### POST /api/attendance

Records a new attendance entry.

#### Request

```http
POST /api/attendance HTTP/1.1
Host: 192.168.1.150:3000
Content-Type: application/json

{
  "student_id": "CCT/00001/023",
  "device_location": "Lab 1",
  "device_id": "aa:bb:cc:dd:ee:ff"
}
```

#### Request Parameters

| Field             | Type   | Required    | Description                                         |
| ----------------- | ------ | ----------- | --------------------------------------------------- |
| `student_id`      | String | ✅ Yes      | Student ID from barcode (e.g., "CCT/00001/023")     |
| `device_location` | String | ❌ Optional | Physical location label (e.g., "Lab 1", "Room 101") |
| `device_id`       | String | ❌ Optional | ESP32 MAC address or device identifier              |
| `course_id`       | Number | ❌ Optional | Course ID for filtering attendance (if applicable)  |

#### Response - Success (200/201)

```json
{
  "id": 42,
  "student_id": "CCT/00001/023",
  "timestamp": "2026-03-30T14:35:42.123Z",
  "device_id": 1
}
```

#### Response - Error (400)

```json
{
  "error": "unknown student_id"
}
```

#### Status Codes

| Code | Meaning                                           |
| ---- | ------------------------------------------------- |
| 200  | Attendance recorded successfully                  |
| 201  | Attendance created (alternative success)          |
| 400  | Bad request (missing student_id, invalid student) |
| 500  | Server error                                      |

#### Example: cURL

```bash
curl -X POST http://192.168.1.150:3000/api/attendance \
  -H "Content-Type: application/json" \
  -d '{
    "student_id": "CCT/00001/023",
    "device_location": "Lab 1"
  }'
```

#### Example: JavaScript (Phone App)

```javascript
fetch("http://192.168.1.150:3000/api/attendance", {
  method: "POST",
  headers: {
    "Content-Type": "application/json",
  },
  body: JSON.stringify({
    student_id: "CCT/00001/023",
    device_location: "Lab 1",
  }),
})
  .then((response) => response.json())
  .then((data) => console.log("Success:", data))
  .catch((error) => console.error("Error:", error));
```

#### Example: Python

```python
import requests

response = requests.post(
    'http://192.168.1.150:3000/api/attendance',
    json={
        'student_id': 'CCT/00001/023',
        'device_location': 'Lab 1'
    }
)
print(response.json())
```

---

### GET /api/attendance

Retrieves all attendance records (latest 100).

#### Request

```http
GET /api/attendance HTTP/1.1
Host: 192.168.1.150:3000
```

#### Response (200)

```json
[
  {
    "id": 42,
    "student_id": "CCT/00001/023",
    "student_name": "John Champion",
    "course_id": 1,
    "course_name": "Software Engineering",
    "timestamp": "2026-03-30T14:35:42.000Z",
    "device_id": 1,
    "device_mac": "aa:bb:cc:dd:ee:ff",
    "device_location": "Lab 1"
  },
  ...
]
```

#### Example: JavaScript (Dashboard)

```javascript
fetch("/api/attendance")
  .then((response) => response.json())
  .then((records) => {
    records.forEach((record) => {
      console.log(`${record.student_name} on ${record.timestamp}`);
    });
  });
```

---

## Students Endpoints

### POST /api/students

Add a new student (Lecturer mode).

#### Request

```json
{
  "student_id": "CCT/00001/023",
  "name": "John Champion",
  "course_id": 1
}
```

#### Response (201)

```json
{
  "id": 1
}
```

---

### GET /api/students

Retrieve all students.

#### Response (200)

```json
[
  {
    "id": 1,
    "student_id": "CCT/00001/023",
    "name": "John Champion",
    "course_id": 1
  }
]
```

---

### GET /api/students/:student_id

Get a specific student by ID.

#### Response (200)

```json
{
  "id": 1,
  "student_id": "CCT/00001/023",
  "name": "John Champion",
  "course_name": "Software Engineering"
}
```

---

## Error Handling

### Common Errors

#### 400 Bad Request - Missing student_id

```json
{
  "error": "student_id required"
}
```

**Fix**: Ensure the scanned barcode is being sent in the student_id field.

#### 400 Bad Request - Unknown student

```json
{
  "error": "unknown student_id"
}
```

**Fix**: Add the student to the database first using POST /api/students.

#### 500 Internal Server Error

```json
{
  "error": "database connection failed"
}
```

**Fix**: Verify backend is running and SQLite database exists.

#### Connection Refused

```
Error: connect ECONNREFUSED 192.168.1.150:3000
```

**Fix**:

- Verify ESP32 is powered and connected to WiFi
- Check correct IP address in request URL
- Ensure phone and ESP32 are on same WiFi network
- Check firewall rules

---

## Rate Limiting (Not Yet Implemented)

⚠️ **Current version has no built-in rate limiting.**

Recommended limits for production:

- **Per IP**: 5 requests/second
- **Per student ID**: 1 request/minute (prevents duplicate entries from same card)
- **Global**: 100 requests/second

See security recommendations in project documentation.

---

## Data Models

### Attendance Schema

```
{
  id: INTEGER,
  student_id: TEXT (unique reference to Students),
  course_id: INTEGER (optional, nullable),
  timestamp: TEXT (ISO 8601),
  device_id: INTEGER (ESP32 device identifier)
}
```

### Student Schema

```
{
  id: INTEGER,
  student_id: TEXT (unique, e.g., "CCT/00001/023"),
  name: TEXT,
  course_id: INTEGER (optional)
}
```

### Device Schema

```
{
  id: INTEGER,
  esp32_mac: TEXT (MAC address, e.g., "aa:bb:cc:dd:ee:ff"),
  location: TEXT (physical location label),
  status: TEXT ("active" or "inactive")
}
```

---

## Testing the API

### Quick Test: Using Postman

1. Import collection from project (if available)
2. Update base URL to `http://192.168.1.150:3000`
3. Test POST request with sample student ID
4. Verify response in attendance GET

### Quick Test: Using a Browser

1. Open: `http://192.168.1.150:3000/api/attendance`
2. Should display JSON array of all attendance records

### Quick Test: Using Terminal

```bash
# Get all attendance
curl http://192.168.1.150:3000/api/attendance

# Post a new record
curl -X POST http://192.168.1.150:3000/api/attendance \
  -H "Content-Type: application/json" \
  -d '{"student_id":"CCT/00001/023"}'
```

---

## Webhook Integration (Future)

The current system uses polling (dashboard refreshes every 10 seconds). For real-time integration:

### Recommended: Server-Sent Events (SSE)

```javascript
const eventSource = new EventSource("/api/attendance/stream");
eventSource.onmessage = (event) => {
  const newRecord = JSON.parse(event.data);
  updateDashboard(newRecord);
};
```

### Alternative: WebSocket

```javascript
const ws = new WebSocket("ws://192.168.1.150:3000/api/attendance/ws");
ws.onmessage = (event) => {
  const newRecord = JSON.parse(event.data);
  updateDashboard(newRecord);
};
```

Not implemented in MVP but recommended for production.

---

## CORS Policy

Current CORS settings allow all origins (for development):

```javascript
app.use(cors());
```

**For production**, restrict to specific origins:

```javascript
app.use(
  cors({
    origin: ["http://localhost:3000", "https://yourdomain.com"],
  }),
);
```

---

For implementation examples and app setup, see:

- [`PHONE_SETUP_GUIDE.md`](PHONE_SETUP_GUIDE.md) - Mobile app configuration
- [`HARDWARE_CONFIGURATION.md`](HARDWARE_CONFIGURATION.md) - ESP32 setup
