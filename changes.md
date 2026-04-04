# Changes Made

## Added WiFi HTTP POST support

- Added inclusion of `WebServer.h` to support HTTP request handling alongside BLE.
- Added a global `WebServer webServer(80)` instance and `httpServerStarted` flag.
- Implemented `startHttpServer()` to configure the POST endpoint and start the HTTP server.
- Added `handlePostAttendance()` to accept HTTP POST requests at `/attendance`.
- Added JSON parsing helper `parseStudentIdFromJson()` to support payloads containing `student_id`.
- Added `handleNotFound()` to reply with 404 for unknown HTTP routes.

## Preserved existing features

- Kept BLE initialization, advertising, GATT service, and characteristic write handling unchanged.
- Kept `sendAttendance()` logic for forwarding attendance records to the backend.
- Kept the WiFi connection management and reconnect behavior.

## Behavior updates

- Both BLE and WiFi are now active simultaneously.
- The ESP32-S3 accepts attendance input from whichever interface arrives first: BLE writes or HTTP POST.
- The main loop now runs `webServer.handleClient()` so HTTP POST requests are serviced.
- The HTTP server is started at boot and once WiFi receives an IP.

## Notes

- WiFi POST endpoint: `/attendance`
- Accepted data formats:
  - Raw body with plain student ID text
  - Form field `student_id`
  - JSON body containing `"student_id"`
- The backend URL and WiFi credentials remain configurable via constants in `src/main.cpp`.
