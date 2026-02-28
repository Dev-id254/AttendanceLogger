// Simple SQLite helper for MVP. Data persisted in a file `attendance.db` in the Backend folder.
const sqlite3 = require('sqlite3').verbose();
const path = require('path');

const dbFile = path.join(__dirname, 'attendance.db');
const db = new sqlite3.Database(dbFile);

// initialize schema if not exists
const initSql = `
PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS Students (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  student_id TEXT UNIQUE NOT NULL,
  name TEXT NOT NULL,
  course_id INTEGER
);

CREATE TABLE IF NOT EXISTS Attendance (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  student_id TEXT NOT NULL,
  course_id INTEGER,
  timestamp TEXT DEFAULT (datetime('now')),
  device_id TEXT,
  FOREIGN KEY (student_id) REFERENCES Students(student_id)
);
`;

db.serialize(() => {
  db.exec(initSql);

  // insert a couple of sample rows if the Students table is empty (MVP convenience)
  db.get('SELECT COUNT(*) AS cnt FROM Students', (err, row) => {
    if (!err && row && row.cnt === 0) {
      db.run('INSERT INTO Students (student_id, name) VALUES (?,?)', ['S1001','John Doe']);
      db.run('INSERT INTO Students (student_id, name) VALUES (?,?)', ['S1002','Jane Smith']);
    }
  });
});

module.exports = db;
