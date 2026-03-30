const express = require('express'); // imports Express framework from node_modues to handle routing and HTTP requests
const router = express.Router(); // creates a new router object to define routes for attendance-related endpoints
const db = require('../data');

// get all attendance logs
// 1. Build Query
router.get('/', (req, res) => {
  // Fetches the latest attendance records and returns them as JSON to the API client(the person asking)
  const sql = `
    SELECT
      a.id,
      a.student_id,
      s.name AS student_name,
      a.course_id,
      c.name AS course_name,
      a.timestamp,
      a.device_id,
      d.esp32_mac AS device_mac,
      d.location AS device_location
    FROM Attendance a
    LEFT JOIN Students s ON s.student_id = a.student_id
    LEFT JOIN Courses c ON c.id = a.course_id
    LEFT JOIN Devices d ON d.id = a.device_id
    ORDER BY a.timestamp DESC
    LIMIT 100
  `; // SQL query to retrieve attendance records, joining with Students, Courses, and Devices tables to get related information. It orders the results by timestamp in descending order and limits the output to the 100 most recent records.

  // 2. Run Query
  db.all(sql, [], (err, rows) => {  // Executes the SQL with no extra parameters (empty array). db.all means "run this and return all resulting rows"

    // 3. Handle Result
    if (err) return res.status(500).json({ error: err.message }); // If something goes wrong (err), respomd with a 500 status code and the error message in JSON format.
    res.json(rows);  // If successful, send back rows as JSON response.
  });
});

// record attendance (from ESP32 or manual post)
router.post('/', (req, res) => {      // Tells the server to listen for POST requests at the root path of this router (which will be mounted at /attendance). This endpoint is used to record a new attendance entry, either from an ESP32 device or through a manual POST request.
  const { student_id, course_id, device_id, device_mac, device_location } = req.body;   // When data arrives, it comes in a package called req.body. This line opens that package and grabs specific items out of it.
  if (!student_id) {
    return res.status(400).json({ error: 'student_id required' });  // 400 = Bad request
  }

            // What does this next line do?. What does ? mean in JS
  const normalizedCourseId = Number.isFinite(Number(course_id)) ? Number(course_id) : null;

  db.get('SELECT student_id FROM Students WHERE student_id = ?', [student_id], (err, studentRow) => {
    if (err) return res.status(500).json({ error: err.message });
    if (!studentRow) return res.status(400).json({ error: 'unknown student_id' });
        // dedupe?
    const dedupeSql = `
      SELECT id, timestamp
      FROM Attendance
      WHERE student_id = ?
        AND (course_id IS ? OR course_id = ?)
        AND timestamp >= datetime('now', '-2 minutes')
      ORDER BY timestamp DESC
      LIMIT 1
    `;
        // R: db.run && db.get ++ Look at the next code blocks, they are really essential
    db.get(dedupeSql, [student_id, normalizedCourseId, normalizedCourseId], (dupeErr, dupeRow) => {
      if (dupeErr) return res.status(500).json({ error: dupeErr.message });
      if (dupeRow) {
        return res.status(409).json({
          error: 'duplicate_scan_recent',
          existing: { id: dupeRow.id, timestamp: dupeRow.timestamp },
        });
      }

      const insertAttendance = (resolvedDeviceId) => {
        db.run(
          'INSERT INTO Attendance (student_id, course_id, device_id) VALUES (?,?,?)',
          [student_id, normalizedCourseId, resolvedDeviceId ?? (Number.isFinite(Number(device_id)) ? Number(device_id) : null)],
          function (insertErr) {
            if (insertErr) return res.status(500).json({ error: insertErr.message });
            res.status(201).json({ id: this.lastID });
          }
        );
      };

      if (device_mac) {
        db.run(
          'INSERT OR IGNORE INTO Devices (esp32_mac, location) VALUES (?,?)',
          [device_mac, device_location || null],
          (devInsErr) => {
            if (devInsErr) return res.status(500).json({ error: devInsErr.message });
            db.get('SELECT id FROM Devices WHERE esp32_mac = ?', [device_mac], (devGetErr, devRow) => {
              if (devGetErr) return res.status(500).json({ error: devGetErr.message });
              insertAttendance(devRow ? devRow.id : null);
            });
          }
        );
      } else {
        insertAttendance(null);
      }
    });
  });
});

module.exports = router;
