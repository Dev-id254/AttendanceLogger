// front-end functionality for MVP

// lecturer mode placeholder
document.getElementById('lec_mode').onclick = function() {
    alert('Lecturer mode is not yet available in MVP.');
};

// load attendance when page is ready
window.addEventListener('DOMContentLoaded', () => {
    loadAttendance();
    // light auto-refresh for demo (every 10s)
    setInterval(loadAttendance, 10000);
});

function loadAttendance() {
    fetch('/api/attendance')
        .then(resp => resp.json())
        .then(data => {
            populateTable(data);
            document.getElementById('lastTime').textContent = new Date().toLocaleString();
        })
        .catch(err => console.error('Failed to load attendance', err));
}

function populateTable(records) {
    const tbody = document.getElementById('attendance-body');
    tbody.innerHTML = '';

    if (records.length === 0) {
        const tr = document.createElement('tr');
        tr.innerHTML = `<td colspan="4">No attendance records yet</td>`;
        tbody.appendChild(tr);
        return;
    }

    records.forEach((rec, idx) => {
        const row = document.createElement('tr');
        row.innerHTML = `
            <td>${idx+1}</td>
            <td>${rec.student_id}${rec.student_name ? ` <small>(${rec.student_name})</small>` : ''}</td>
            <td>${new Date(rec.timestamp).toLocaleString()}</td>
            <td><a class="info-btn" href="#" data-id="${rec.student_id}">Info</a></td>
        `;
        tbody.appendChild(row);
    });
}

// delegate clicks for info buttons
document.body.addEventListener('click', e => {
    if (e.target.classList.contains('info-btn')) {
        e.preventDefault();
        const id = e.target.getAttribute('data-id');
        fetch(`/api/students/${encodeURIComponent(id)}`)
            .then(r => r.ok ? r.json() : Promise.reject(r))
            .then(s => {
                const course = s.course_name ? `\nCourse: ${s.course_name}` : '';
                alert(`Student ID: ${s.student_id}\nName: ${s.name}${course}`);
            })
            .catch(() => alert('Student details not found for ' + id));
    }
});
