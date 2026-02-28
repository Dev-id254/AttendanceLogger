const express = require('express');
const router = express.Router();
const db = require('../data');

// get all students
router.get('/', (req, res) => {
  db.all('SELECT * FROM Students', [], (err, rows) => {
    if (err) return res.status(500).json({ error: err.message });
    res.json(rows);
  });
});

// add a student (lec mode)
router.post('/', (req, res) => {
  const { student_id, name, course_id } = req.body;
  if (!student_id || !name) {
    return res.status(400).json({ error: 'student_id and name required' });
  }
  db.run(
    'INSERT OR IGNORE INTO Students (student_id, name, course_id) VALUES (?,?,?)',
    [student_id, name, course_id || null],
    function (err) {
      if (err) return res.status(500).json({ error: err.message });
      res.json({ id: this.lastID });
    }
  );
});

module.exports = router;
