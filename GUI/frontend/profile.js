const { ipcRenderer } = window.require('electron');

import Chart from 'chart.js/auto';
import { Colors } from 'chart.js';
import { Modal } from 'bootstrap';

import { getConnected } from './app.js';
Chart.register(Colors);

const ctx = document.getElementById('profileChart');
const colors = ['#426cf5', '#29c254'];

const chart = new Chart(ctx, {
    type: 'line',
    data: {
        datasets: [
            {
                type: 'line',
                label: "Time",
                data: [],
                fill: false,
                borderColor: colors.pop(),
                yAxisID: 'x',
                tension: 1,
                borderWidth: 2
            },
            {
                type: 'line',
                label: 'Target',
                data: [],
                fill: false,
                borderColor: colors.pop(),
                yAxisID: 'y',
                tension: 1,
                borderWidth: 2,
                pointStyle: 'crossRot'
            }
        ]
    },
    options: {
        responsive: true,
        maintainAspectRatio: true,
        plugins: {
            colors: {
                enabled: true,
                forceOverride: false
            },
            legend: {
                position: 'bottom'
            }
        },
        scales: {
            x: {
                type: 'linear',
                display: true,
                ticks: {
                    stepSize: 10,
                    autoSkip: false
                },
                min: 0,
                max: 240
            },
            y: {
                type: 'linear',
                min: 25,
                max: 170,
                title: {
                    text: 'Target (℃)',
                    display: true
                },
                display: true,
                ticks: {
                    stepSize: 10,
                    autoSkip: true
                },
                position: 'right'
            }
        }
    }
});

const manageProfilesBtn = document.getElementById('manageProfilesBtn');

const modal = Modal.getOrCreateInstance(document.getElementById('profileModal'));

const profiles = [];

manageProfilesBtn.addEventListener('click', () => {
    if (getConnected() == false)
        return;

    ipcRenderer.send('request-serial-enter-profile-mode');
});

ipcRenderer.on('response-serial-enter-profile-mode', (event, arg) => {
    if (arg.ok) {
        modal.show();

        // Load profiles

        console.log(arg.profiles);

        let mock = [{
            data:[30,], 
        }];

        return;
    }

    alert("No response from MCU. Device needs to be on profile selection screen.");
});