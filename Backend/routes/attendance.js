const express = require('express');
const router = express.Router();
const db = require('../data');

// Promisify SQLite methods to enable async/await
const dbAll = (sql, params = []) => {
  return new Promise((resolve, reject) => {
    db.all(sql, params, (err, rows) => {
      if (err) reject(err);
      else resolve(rows);
    });
  });
};

const dbGet = (sql, params = []) => {
  return new Promise((resolve, reject) => {
    db.get(sql, params, (err, row) => {
      if (err) reject(err);
      else resolve(row);
    });
  });
};

const dbRun = (sql, params = []) => {
  return new Promise((resolve, reject) => {
    db.run(sql, params, function(err) {
      if (err) reject(err);
      else resolve({ lastID: this.lastID, changes: this.changes });
    });
  });
};

// GET all attendance logs
router.get('/', async (req, res) => {
  try {
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

    const rows = await dbAll(sql, []);
    res.json(rows);
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

// Record attendance (from ESP32 or manual post)
router.post('/', async (req, res) => {
  try {
    const { student_id, course_id, device_id, device_mac, device_location } = req.body;

    // Validate student_id is provided
    if (!student_id) {
      return res.status(400).json({ error: 'student_id required' });
    }

    // Normalize course_id to number or null
    const normalizedCourseId = Number.isFinite(Number(course_id)) ? Number(course_id) : null;

    // Verify student exists
    const studentRow = await dbGet('SELECT student_id FROM Students WHERE student_id = ?', [student_id]);
    if (!studentRow) {
      return res.status(400).json({ error: 'unknown student_id' });
    }

    // Check for duplicate scan within 2 minutes (deduplication)
    const dedupeSql = `
      SELECT id, timestamp
      FROM Attendance
      WHERE student_id = ?
        AND (course_id IS ? OR course_id = ?)
        AND timestamp >= datetime('now', '-2 minutes')
      ORDER BY timestamp DESC
      LIMIT 1
    `;

    const dupeRow = await dbGet(dedupeSql, [student_id, normalizedCourseId, normalizedCourseId]);
    if (dupeRow) {
      return res.status(409).json({
        error: 'duplicate_scan_recent',
        existing: { id: dupeRow.id, timestamp: dupeRow.timestamp },
      });
    }

    // Helper function to insert attendance record
    const insertAttendance = async (resolvedDeviceId) => {
      const resolvedId = resolvedDeviceId ?? (Number.isFinite(Number(device_id)) ? Number(device_id) : null);
      const result = await dbRun(
        'INSERT INTO Attendance (student_id, course_id, device_id) VALUES (?,?,?)',
        [student_id, normalizedCourseId, resolvedId]
      );
      res.status(201).json({ id: result.lastID });
    };

    // Handle device registration if device_mac is provided
    if (device_mac) {
      await dbRun(
        'INSERT OR IGNORE INTO Devices (esp32_mac, location) VALUES (?,?)',
        [device_mac, device_location || null]
      );

      const devRow = await dbGet('SELECT id FROM Devices WHERE esp32_mac = ?', [device_mac]);
      await insertAttendance(devRow ? devRow.id : null);
    } else {
      await insertAttendance(null);
    }
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

module.exports = router;
