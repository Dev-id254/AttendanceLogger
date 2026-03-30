// These first line of code, what and why are they?
const express = require('express');
const router = express.Router();
const db = require('../data');

// get all students [Okay!]
router.get('/', (req, res) => {
  db.all('SELECT * FROM Students', [], (err, rows) => {
    if (err) return res.status(500).json({ error: err.message });
    res.json(rows);
  });
});

// get one student by student_id (for "Info" UI)
// These must be understood
router.get('/:student_id', (req, res) => {
  const studentId = req.params.student_id;
  const sql = `
    SELECT
      s.id,
      s.student_id,
      s.name,
      s.course_id,
      c.name AS course_name
    FROM Students s
    LEFT JOIN Courses c ON c.id = s.course_id
    WHERE s.student_id = ?
    LIMIT 1
  `;
  db.get(sql, [studentId], (err, row) => { // Again, these codes are neat to know
    if (err) return res.status(500).json({ error: err.message });
    if (!row) return res.status(404).json({ error: 'student_not_found' });
    res.json(row);
  });
});

// These module also looks important
// add a student (lec mode)
router.post('/', (req, res) => {
  const { student_id, name, course_id } = req.body;
  if (!student_id || !name) {
    return res.status(400).json({ error: 'student_id and name required' });
  }
  const normalizedCourseId = Number.isFinite(Number(course_id)) ? Number(course_id) : null;
  db.run(
    'INSERT OR IGNORE INTO Students (student_id, name, course_id) VALUES (?,?,?)',
    [student_id, name, normalizedCourseId],
    function (err) {
      if (err) return res.status(500).json({ error: err.message });
      res.status(201).json({ id: this.lastID });
    }
  );
});

module.exports = router;
