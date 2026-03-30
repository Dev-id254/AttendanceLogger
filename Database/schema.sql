-- MySQL schema for Smart Attendance System (MVP & future expansion)
-- run in MySQL workbench or via CLI
-- study this db and try to run it too in the sql CLI

CREATE DATABASE IF NOT EXISTS attendance_system DEFAULT CHARACTER SET utf8mb4;
USE attendance_system;

CREATE TABLE IF NOT EXISTS Lecturers (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    username VARCHAR(50) UNIQUE NOT NULL,
    role VARCHAR(20) DEFAULT 'lecturer'
);

CREATE TABLE IF NOT EXISTS Courses (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    lecturer_id INT,
    FOREIGN KEY (lecturer_id) REFERENCES Lecturers(id)
);

CREATE TABLE IF NOT EXISTS Students (
    id INT AUTO_INCREMENT PRIMARY KEY,
    student_id VARCHAR(50) UNIQUE NOT NULL,
    name VARCHAR(100) NOT NULL,
    course_id INT,
    FOREIGN KEY (course_id) REFERENCES Courses(id)
);

CREATE TABLE IF NOT EXISTS Devices (
    id INT AUTO_INCREMENT PRIMARY KEY,
    esp32_mac VARCHAR(50) UNIQUE,
    location VARCHAR(100),
    status VARCHAR(20) DEFAULT 'active'
);

CREATE TABLE IF NOT EXISTS Attendance (
    id INT AUTO_INCREMENT PRIMARY KEY,
    student_id VARCHAR(50) NOT NULL,
    course_id INT,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    device_id INT,
    FOREIGN KEY (student_id) REFERENCES Students(student_id),
    FOREIGN KEY (course_id) REFERENCES Courses(id),
    FOREIGN KEY (device_id) REFERENCES Devices(id)
);
