-- seed data for testing MVP
USE attendance_system;

INSERT IGNORE INTO Lecturers (name, username) VALUES
('Dr. Alice Mwangi','alice');

INSERT IGNORE INTO Courses (name, lecturer_id) VALUES
('Software Engineering',1);

INSERT IGNORE INTO Students (student_id, name, course_id) VALUES
('CCT/00001/023','John Champion',306),
('CCT/000002/023','Jane Smitherins',308);

INSERT IGNORE INTO Devices (esp32_mac, location) VALUES
('AA:BB:CC:DD:EE:FF','Lab 1');

-- What does INSERT *IGNORE* do? Hio ignore hio