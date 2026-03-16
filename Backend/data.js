// Simple SQLite helper for MVP. Data persisted in a file `attendance.db` in the Backend folder.
const sqlite3 = require('sqlite3').verbose();
const path = require('path');

const dbFile = path.join(__dirname, 'attendance.db');
const db = new sqlite3.Database(dbFile);

// initialize schema if not exists
const initSql = `
PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS Lecturers (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name TEXT NOT NULL,
  username TEXT UNIQUE NOT NULL,
  role TEXT DEFAULT 'lecturer'
);

CREATE TABLE IF NOT EXISTS Courses (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name TEXT NOT NULL,
  lecturer_id INTEGER,
  FOREIGN KEY (lecturer_id) REFERENCES Lecturers(id)
);

CREATE TABLE IF NOT EXISTS Students (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  student_id TEXT UNIQUE NOT NULL,
  name TEXT NOT NULL,
  course_id INTEGER,
  FOREIGN KEY (course_id) REFERENCES Courses(id)
);

CREATE TABLE IF NOT EXISTS Devices (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  esp32_mac TEXT UNIQUE,
  location TEXT,
  status TEXT DEFAULT 'active'
);

CREATE TABLE IF NOT EXISTS Attendance (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  student_id TEXT NOT NULL,
  course_id INTEGER,
  timestamp TEXT DEFAULT (datetime('now')),
  device_id INTEGER,
  FOREIGN KEY (student_id) REFERENCES Students(student_id),
  FOREIGN KEY (course_id) REFERENCES Courses(id),
  FOREIGN KEY (device_id) REFERENCES Devices(id)
);
`;

db.serialize(() => {
  db.exec(initSql);

  // seed minimal demo rows (MVP convenience)
  db.get('SELECT COUNT(*) AS cnt FROM Students', (err, row) => {
    if (err || !row || row.cnt !== 0) return;

    db.run(
      'INSERT OR IGNORE INTO Lecturers (name, username) VALUES (?,?)',
      ['Dr. Alice Mwangi', 'alice']
    );
    db.run(
      'INSERT OR IGNORE INTO Courses (id, name, lecturer_id) VALUES (?,?,?)',
      [1, 'Software Engineering', 1]
    );
    db.run(
      'INSERT OR IGNORE INTO Students (student_id, name, course_id) VALUES (?,?,?)',
      ['S1001', 'John Doe', 1]
    );
    db.run(
      'INSERT OR IGNORE INTO Students (student_id, name, course_id) VALUES (?,?,?)',
      ['S1002', 'Jane Smith', 1]
    );
    db.run(
      'INSERT OR IGNORE INTO Devices (id, esp32_mac, location) VALUES (?,?,?)',
      [1, 'AA:BB:CC:DD:EE:FF', 'Lab 1']
    );
  });
});

module.exports = db;
