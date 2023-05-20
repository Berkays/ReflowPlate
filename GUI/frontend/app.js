const { ipcRenderer } = window.require('electron');
import bootstrap from 'bootstrap';

import moment from 'moment';
import Chart from 'chart.js/auto';
import 'chartjs-adapter-moment';

import { Colors } from 'chart.js';
import './index.scss';

import './profile.js';

Chart.register(Colors);

const ctx = document.getElementById('chart');
const colors = ['#006400', '#ff4500', '#0000ff', '#ff1493', '#f58231', '#42d4f4', '#f032e6', '#800000'];

const chart = new Chart(ctx, {
    type: 'line',
    data: {
        datasets: [
            {
                type: 'line',
                label: 'Heat',
                data: [],
                fill: true,
                borderColor: 'rgba(255, 0, 0, 0.1)',
                backgroundColor: 'rgba(255, 0, 0, 0.1)',
                yAxisID: 'state',
                stepped: true,
                pointStyle: 'line'
            },
            {
                type: 'line',
                label: 'Output',
                data: [],
                fill: false,
                borderColor: colors.pop(),
                yAxisID: 'output',
                tension: 1,
                borderWidth: 2,
                pointStyle: 'line'
            },
            {
                type: 'line',
                label: 'Temperature',
                data: [],
                fill: false,
                borderColor: colors.pop(),
                yAxisID: 'temperature',
                tension: 1,
                borderWidth: 2,
                pointStyle: 'crossRot'
            },
            {
                type: 'line',
                label: 'Target',
                data: [],
                fill: false,
                borderColor: colors.pop(),
                yAxisID: 'target',
                tension: 1,
                borderWidth: 2,
                pointStyle: 'line'
            }
        ]
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        spanGaps: true,
        datasets: {
            line: {
                pointRadius: 0
            }
        },
        normalized: true,
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
                ticks: {
                    stepSize: 10,
                    autoSkip: false
                },
                min: 0,
                max: 240
            },
            temperature: {
                type: 'linear',
                min: 25,
                max: 170,
                title: {
                    text: 'Temperature (℃)',
                    display: true
                },
                display: true,
                ticks: {
                    stepSize: 10,
                    autoSkip: true
                },
                position: 'right'
            },
            output: {
                type: 'linear',
                min: -0.1,
                max: 1.1,
                display: false,
                axis: 'y',
                ticks: {
                    stepSize: 0.25,
                    autoSkip: false
                }
            },
            heat: {
                type: 'linear',
                min: -0.1,
                max: 1.1,
                display: false,
                axis: 'y'
            },
            target: {
                type: 'linear',
                axis: 'y',
                min: 25,
                max: 170,
                title: {
                    text: 'Target (℃)',
                    display: true
                },
                display: false,
                ticks: {
                    stepSize: 10,
                    autoSkip: true
                },
                grid: {
                    display: false
                }
            }
        }
    }
});

let isConnected = false;
export function getConnected() {
    return isConnected;
}

getConnected.bind(isConnected);

let dataCount = 0;

const connectBtn = document.getElementById('connectBtn');
const portSelect = document.getElementById('portSelect');
const deviceTxt = document.getElementById('jlinkDevice');
const jlinkPathTxt = document.getElementById('jlinkPath');

const sampleCountText = document.getElementById('sampleCountText');
const statusText = document.getElementById('statusText');

const chartMin = document.getElementById('chartMin');
const chartMax = document.getElementById('chartMax');


chartMin.addEventListener('change', (event) => {
    const val = event.target.value;
    chart.options.scales['y'].min = parseFloat(val);
    chart.options.scales['target'].min = parseFloat(val);
    chart.update('none');
});
chartMax.addEventListener('change', (event) => {
    const val = event.target.value;
    chart.options.scales['y'].max = parseFloat(val);
    chart.options.scales['target'].max = parseFloat(val);
    chart.update('none');
});

connectBtn.addEventListener('click', () => {
    if (getConnected() == false) {
        ipcRenderer.send('request-serial-connect', { port: portSelect.value, device: deviceTxt.value, target: jlinkIpTxt.value, jlinkPath: jlinkPathTxt.value });
        return;
    }

    ipcRenderer.send('request-serial-disconnect');
});

ipcRenderer.on('response-serial-listports', (event, arg) => {

    portSelect.replaceChildren(portSelect.children[0]);
    arg.forEach(port => {
        let opt = document.createElement("option");
        opt.value = port;

        const slashIndex = port.lastIndexOf('/');

        if (slashIndex == -1)
            opt.innerHTML = port;
        else
            opt.innerHTML = port.substring(slashIndex, port.length - slashIndex);

        portSelect.append(opt);
    });

    portSelect.selectedIndex = portSelect.childElementCount > 1 ? 1 : 0;
});
ipcRenderer.on('response-serial-connect', (event, arg) => {
    if (arg.ok) {
        setConnected(true);

        return;
    }
});
ipcRenderer.on('response-serial-disconnect', (event, arg) => {
    if (arg.ok) {
        setConnected(false);
    }
});

ipcRenderer.on('serial-data', (event, arg) => {
    if (Object.keys(arg).length == 0)
        return;

    Object.entries(arg).forEach(([k, v]) => {


    });

    dataCount++;
    sampleCountText.textContent = dataCount;
    chart.update('none');
});


window.addEventListener('load', () => {
    ipcRenderer.send('request-serial-listports');
});

const clearValuesBtn = document.getElementById('clearValuesBtn');

clearValuesBtn.addEventListener('click', () => {
    chart.data.datasets.forEach((dataset) => {
        dataset.data = [];
    });

    chart.update('none');
    dataCount = 0;
    sampleCountText.textContent = dataCount;
});

function setConnected(state) {
    if (state == true) {
        isConnected = true;
        statusText.innerText = "Connected";
        return;
    }

    isConnected = false;
    statusText.innerText = "Disconnected";
}