const express = require('express');
const router = express.Router();
const db = require('../data');

// get all attendance logs
router.get('/', (req, res) => {
  db.all('SELECT * FROM Attendance ORDER BY timestamp DESC LIMIT 100', [], (err, rows) => {
    if (err) return res.status(500).json({ error: err.message });
    res.json(rows);
  });
});

// record attendance (from ESP32 or manual post)
router.post('/', (req, res) => {
  const { student_id, course_id, device_id } = req.body;
  if (!student_id) {
    return res.status(400).json({ error: 'student_id required' });
  }
  db.run(
    'INSERT INTO Attendance (student_id, course_id, device_id) VALUES (?,?,?)',
    [student_id, course_id || null, device_id || null],
    function (err) {
      if (err) return res.status(500).json({ error: err.message });
      res.json({ id: this.lastID, timestamp: new Date().toISOString() });
    }
  );
});

module.exports = router;
