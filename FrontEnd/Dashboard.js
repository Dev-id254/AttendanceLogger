// front-end functionality for MVP

// lecturer mode placeholder
document.getElementById('lec_mode').onclick = function() {
    alert('Lecturer mode is not yet available in MVP.');
};

// load attendance when page is ready
window.addEventListener('DOMContentLoaded', () => {
    loadAttendance();
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
    const table = document.querySelector('.main_table table');
    // clear old rows except header
    const old = table.querySelectorAll('tr');
    old.forEach((r, i) => { if (i>0) r.remove(); });

    if (records.length === 0) {
        const tr = document.createElement('tr');
        tr.innerHTML = `<td colspan="4">No attendance records yet</td>`;
        table.appendChild(tr);
        return;
    }

    records.forEach((rec, idx) => {
        const row = document.createElement('tr');
        row.innerHTML = `
            <td>${idx+1}</td>
            <td>${rec.student_id}</td>
            <td colspan="12">${new Date(rec.timestamp).toLocaleString()}</td>
            <td><a class="info-btn" href="#" data-id="${rec.student_id}">Info</a></td>
        `;
        table.appendChild(row);
    });
}

// delegate clicks for info buttons
document.body.addEventListener('click', e => {
    if (e.target.classList.contains('info-btn')) {
        e.preventDefault();
        const id = e.target.getAttribute('data-id');
        alert('Info for student ' + id);
    }
});
