-- seed data for testing MVP
USE attendance_system;

INSERT IGNORE INTO Lecturers (name, username) VALUES
('Dr. Alice Mwangi','alice');

INSERT IGNORE INTO Courses (name, lecturer_id) VALUES
('Software Engineering',1);

INSERT IGNORE INTO Students (student_id, name, course_id) VALUES
('S1001','John Doe',1),
('S1002','Jane Smith',1);

INSERT IGNORE INTO Devices (esp32_mac, location) VALUES
('AA:BB:CC:DD:EE:FF','Lab 1');
