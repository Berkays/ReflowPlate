const net = require('net');
const { ipcMain } = require('electron');
const { sendMessage, startJLink, stopJLink, readFlash, writeFlash } = require('./main');

let isConnected = false;
let activeDevice = null;

const DATA_SIZE = 4; // All data is 4 bytes int
const CODE_START = 0xAF;
const CODE_END = 0xFA;
const CODE_TELEMETRY = 0xAB;
const CODE_PROFILE = 0xAC;

const CODE_HOST_ENTER_PROFILE = 0xFB;
const CODE_HOST_EXIT_PROFILE = 0xFC;

const MARK_STATUS_TIME = 0xC0;
const MARK_STATUS_TEMPERATURE = 0xC1;
const MARK_STATUS_TARGET = 0xC2;
const MARK_STATUS_OUTPUT = 0xC3;
const MARK_STATUS_STATE = 0xC4;

const MARK_PROFILE_FLASH_ADDR = 0xD0;
const MARK_PROFILE_COUNT = 0xD1;
const MARK_PROFILE_SIZE = 0xD2;

const host_command_buffer = new Uint8Array([CODE_START, 0, CODE_END]);
let mcuCallback;

let profileCount = 1;
let flashBaseAddr = 0x0800EC00;
let profileSize = 64;

async function connectJLink(event, jlinkPath = '/Applications/SEGGER/JLink_V784b/JLinkExe', device = 'STM32F030C8', target = '127.0.0.1') {
    if (activeDevice) {
        activeDevice.end();
        isConnected = false;
        activeDevice = null;
    }

    var promise = new Promise((resolve, reject) => {
        // Start jlink instance
        stopJLink();

        startJLink(jlinkPath, device, target, isRemote, () => {
            activeDevice = new net.Socket();
            activeDevice.connect(19021, '127.0.0.1', function () {
                console.log('Connected to JLink Server');
                isConnected = true;
                event.sender.send('response-serial-connect', { ok: true, msg: 'OK' });
                resolve();
            });

            activeDevice.on('data', onData);

            activeDevice.on('close', function () {
                console.log('JLink Connection closed');
            });
        });
    });

    await promise;
}

async function onData(data) {
    if (!data || data.length > 64)
        return;

    console.log(data);
    const startCode = data[0];
    const endCode = data[data.length - 1];

    if (startCode != CODE_START || endCode != CODE_END) {
        console.warn("Invalid data");
        return;
    }

    const command = data[1];
    let arg = {};
    switch (command) {
        case CODE_TELEMETRY:
            var dataSlice = Buffer.from(data.slice(2, data.length - 1));

            const marks = [MARK_STATUS_TIME, MARK_STATUS_TEMPERATURE, MARK_STATUS_TARGET, MARK_STATUS_OUTPUT];
            const keys = ["time", "temperature", "target", "output"];
            arg = { temperature: 0, target: 0, output: 0 };
            for (let i = 0; i < marks.length; i++) {
                const indexStart = i * (DATA_SIZE + 1);
                if (dataSlice[indexStart] == marks[i])
                    arg[keys[i]] = dataSlice.readInt32BE(indexStart + 1);
            }

            sendMessage('serial-data', arg);
            break;

        case CODE_PROFILE:
            var dataSlice = Buffer.from(data.slice(2, data.length - 1));

            if (dataSlice[0] == MARK_PROFILE_FLASH_ADDR)
                flashBaseAddr = dataSlice.readUInt32BE(1);
            else
                mcuCallback();
            if (dataSlice[1 + DATA_SIZE] == MARK_PROFILE_COUNT)
                profileCount = dataSlice.readUInt32BE(2 + (DATA_SIZE * 2));
            else
                mcuCallback();
            if (dataSlice[2 + (DATA_SIZE * 2)] == MARK_PROFILE_SIZE)
                profileSize = dataSlice.readUInt32BE(3 + (DATA_SIZE * 3));
            else
                mcuCallback();

            arg = {
                flashBaseAddr,
                profileCount,
                profileSize
            };
            mcuCallback(arg);
            break;


        default:
            break;
    }


}

ipcMain.on('request-serial-listports', async (event, _) => {
    console.log('REQUEST:SERIAL-LIST');

    let ports = [{ path: 'JLink' }];
    event.sender.send('response-serial-listports', ports.map(n => n.path));
});

ipcMain.on('request-serial-connect', async (event, arg) => {
    console.log('REQUEST:SERIAL-CONNECT');

    if (arg && arg.port == 'JLink') {
        await connectJLink(event, false, arg.jlinkPath, arg.device);
        return;
    }
});

ipcMain.on('request-serial-disconnect', async (event, _) => {
    console.log('REQUEST:SERIAL-DISCONNECT');
    if (isConnected == false)
        return;

    if (activeDevice) {
        activeDevice.end();
        activeDevice = null;
    }

    event.sender.send('response-serial-disconnect', { ok: true, msg: 'OK' });

    isConnected = false;
});

//EVENTS
ipcMain.on('request-device-enter-profile-mode', async (event, arg) => {
    console.log('REQUEST:DEVICE-ENTERPROFILEMODE');
    if (isConnected == false || activeDevice == null)
        return;
    mcuCallback = (arg) => {
        if (arg.profileSize && arg.profileCount && arg.profileFlashAddress) {
            readFlash(arg.profileFlashAddress, arg.profileCount, arg.profileSize, (profiles) => {
                event.sender.send('response-device-enter-profile-mode', { ok: true, profiles });
            });
            return;
        }

        event.sender.send('response-device-enter-profile-mode', { ok: false });
    };

    host_command_buffer[1] = CODE_HOST_ENTER_PROFILE;
    activeDevice.write(host_command_buffer);
});
ipcMain.on('request-device-write-profile', async (event, arg) => {
    console.log('REQUEST:DEVICE-WRITE-PROFILE');

    if (isConnected == false || activeDevice == null)
        return;

    if (profileCount == 0 || profileSize == 0)
        return;

    if (arg.profile) {
        writeFlash(flashBaseAddr, profileSize, profileCount, arg.profile, () => {
            event.sender.send('response-device-write-profile', { ok: true });
        });
        return;
    }

    event.sender.send('response-device-write-profile', { ok: false });
});

