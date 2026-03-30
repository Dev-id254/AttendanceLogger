// Server side. This is part of the main things we should know

const express = require('express');
const cors = require('cors');
const path = require('path');

const attendanceRoutes = require('./routes/attendance');
const studentRoutes = require('./routes/students');

const app = express();
const PORT = process.env.PORT || 3000;    // Why 3000? Will other ports work?

// middleware : What are these
app.use(cors());
app.use(express.json());

// serve frontend static files if any (during development you can open Dashboard.html directly)
app.use(express.static(path.join(__dirname, '../FrontEnd')));

// api routes
app.use('/api/attendance', attendanceRoutes);
app.use('/api/students', studentRoutes);

app.get('/', (req, res) => {  // This req is called but never used, why? What is it in the 1st place
  res.sendFile(path.join(__dirname, '../FrontEnd/Dashboard.html'));
});

app.listen(PORT, () => {
  console.log(`Attendance backend listening on port ${PORT}`);
});
