try {
    require('electron-reloader')(module)
} catch (_) { }

const { app, BrowserWindow } = require('electron')
const { spawn } = require('cross-spawn');
const { readFileSync, writeFileSync } = require('fs');

const { parseProfileData, packProfileData } = require('./util.js');
let jlinkProcess;
let jlinkOut = ''

let win;
const createWindow = () => {
    win = new BrowserWindow({
        width: 912,
        height: 513,
        webPreferences: {
            nodeIntegration: true,
            nodeIntegrationInWorker: true,
            nodeIntegrationInSubFrames: true,
            enableRemoteModule: true,
            contextIsolation: false
        }
    })

    win.loadFile('./client/index.html')
}

app.whenReady().then(() => {
    createWindow();

    require('./serial.js');

    app.on('activate', () => {
        if (BrowserWindow.getAllWindows().length === 0) createWindow();
    })
})

app.on('window-all-closed', () => {
    this.stopJLink();
    app.quit();
})


module.exports.sendMessage = function (event, arg) {
    if (win == null || win == undefined)
        return;
    win.webContents.send(event, arg);
};

module.exports.startJLink = function (path, device, ip = '', isRemote = false, onConnected) {
    if (win == null || win == undefined)
        return;

    stopJLink();

    jlinkProcess = spawn(path, ['-Device', device, '-AutoConnect', '1', '-If', 'SWD', '-Speed', '40000'], { shell: true, cwd: process.cwd() });
    jlinkOut = '';

    const regex = /Cortex-[A-Z]\d{1,2}\sidentified/gm;

    let dataTimeout = setTimeout(() => { }, 0);

    if (process.platform == 'darwin') {
        let connectedFlag = false;
        jlinkProcess.stdout.on('data', (data) => {
            if (connectedFlag)
                return;

            console.log(data.toString().trim());
            jlinkOut += data.toString().trim();

            clearTimeout(dataTimeout);
            dataTimeout = setTimeout(() => {
                // Check for chip identification
                if (regex.test(data)) {
                    connectedFlag = true;
                    jlinkOut = '';
                    onConnected();
                }
            }, 4000);
        });
    }

    jlinkProcess.stderr.on('data', (data) => {
        console.log("JLink Error");
    });

    jlinkProcess.on('error', (error) => {
        console.error(`JLink Error: ${error.message}`);
    });

    jlinkProcess.on('close', (_) => {
        console.log(`Exited JLink`);
    });
};

module.exports.stopJLink = function () {
    if (jlinkProcess === null || jlinkProcess === undefined)
        return;

    jlinkProcess.kill();
};

module.exports.readFlash = function (baseAddress, count, size, onRead) {
    if (jlinkProcess === null || jlinkProcess === undefined)
        return;

    let addrHex = Buffer.alloc(4);
    addrHex.writeUint32BE(baseAddress);

    let numBytesHex = Buffer.alloc(4);
    numBytesHex.writeUint32BE(count * size);

    jlinkProcess.stdin.write(`SaveBin profiles.bin, 0x${addrHex.toString('hex')}, 0x${numBytesHex.toString('hex')}\n`);

    setTimeout(() => {
        const binImage = readFileSync('profiles.bin');
        onRead(parseProfileData(binImage));
    }, 1000);
}
module.exports.writeFlash = function (baseAddress, size, index, profile, onWrite) {
    let writeAddr = baseAddress + (index * size);

    let addrHex = Buffer.alloc(4);
    addrHex.writeUint32BE(writeAddr);

    const packedProfile = packProfileData(profile);
    writeFileSync('profiles_out.bin', packedProfile, { encoding: 'hex' });

    jlinkProcess.stdin.write(`LoadBin profiles_out.bin, 0x${addrHex.toString('hex')}\n`);

    setTimeout(() => {
        onWrite(data);
    }, 1000);
}

