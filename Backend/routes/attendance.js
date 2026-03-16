const express = require('express');
const router = express.Router();
const db = require('../data');

// get all attendance logs
router.get('/', (req, res) => {
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
  `;
  db.all(sql, [], (err, rows) => {
    if (err) return res.status(500).json({ error: err.message });
    res.json(rows);
  });
});

// record attendance (from ESP32 or manual post)
router.post('/', (req, res) => {
  const { student_id, course_id, device_id, device_mac, device_location } = req.body;
  if (!student_id) {
    return res.status(400).json({ error: 'student_id required' });
  }

  const normalizedCourseId = Number.isFinite(Number(course_id)) ? Number(course_id) : null;

  db.get('SELECT student_id FROM Students WHERE student_id = ?', [student_id], (err, studentRow) => {
    if (err) return res.status(500).json({ error: err.message });
    if (!studentRow) return res.status(400).json({ error: 'unknown student_id' });

    const dedupeSql = `
      SELECT id, timestamp
      FROM Attendance
      WHERE student_id = ?
        AND (course_id IS ? OR course_id = ?)
        AND timestamp >= datetime('now', '-2 minutes')
      ORDER BY timestamp DESC
      LIMIT 1
    `;

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
