function setStatus(id, text, cls) {
    const el = document.getElementById(id);
    el.textContent = text;
    el.className = `status ${cls || ''}`.trim();
}

async function queryPermission(name) {
    if (!navigator.permissions) {
        throw new Error('Permissions API not supported');
    }
    const status = await navigator.permissions.query({ name });
    return status.state;
}

/* ---------- Camera ---------- */

async function checkCamera() {
    setStatus('camera-status', 'Checking...', 'loading');
    try {
        const state = await queryPermission('camera');
        setStatus('camera-status', `State: ${state}`, 'success');
    } catch (e) {
        setStatus('camera-status', e.message || 'Error', 'error');
    }
}

async function requestCamera() {
    setStatus('camera-status', 'Requesting...', 'loading');
    try {
        await navigator.mediaDevices.getUserMedia({ video: true });
        setStatus('camera-status', 'Granted', 'success');
    } catch (e) {
        setStatus('camera-status', 'Denied', 'error');
    }
}

/* ---------- Microphone ---------- */

async function checkMicrophone() {
    setStatus('mic-status', 'Checking...', 'loading');
    try {
        const state = await queryPermission('microphone');
        setStatus('mic-status', `State: ${state}`, 'success');
    } catch (e) {
        setStatus('mic-status', e.message || 'Error', 'error');
    }
}

async function requestMicrophone() {
    setStatus('mic-status', 'Requesting...', 'loading');
    try {
        await navigator.mediaDevices.getUserMedia({ audio: true });
        setStatus('mic-status', 'Granted', 'success');
    } catch (e) {
        setStatus('mic-status', 'Denied', 'error');
    }
}

/* ---------- Geolocation ---------- */

async function checkGeolocation() {
    setStatus('geo-status', 'Checking...', 'loading');
    try {
        const state = await queryPermission('geolocation');
        setStatus('geo-status', `State: ${state}`, 'success');
    } catch (e) {
        setStatus('geo-status', e.message || 'Error', 'error');
    }
}

async function requestGeolocation() {
    setStatus('geo-status', 'Requesting...', 'loading');
    try {
        navigator.geolocation.getCurrentPosition(
            () => setStatus('geo-status', 'Granted', 'success'),
            () => setStatus('geo-status', 'Denied', 'error')
        );
    } catch (e) {
        setStatus('geo-status', e.message || 'Error', 'error');
    }
}

/* ---------- Notifications ---------- */

async function checkNotifications() {
    setStatus('notif-status', 'Checking...', 'loading');
    try {
        const state = Notification.permission;
        setStatus('notif-status', `State: ${state}`, 'success');
    } catch (e) {
        setStatus('notif-status', e.message || 'Error', 'error');
    }
}

async function requestNotifications() {
    setStatus('notif-status', 'Requesting...', 'loading');
    try {
        const result = await Notification.requestPermission();
        setStatus('notif-status', `${result == 'denied' ? 'Denied' : 'Granted'}`, `${result == 'denied' ? 'error' : 'success'}`);
    } catch (e) {
        setStatus('notif-status', e.message || 'Error', 'error');
    }
}

/* ---------- Clipboard ---------- */

async function checkClipboard() {
    setStatus('clipboard-status', 'Checking...', 'loading');
    try {
        const state = await queryPermission('clipboard-read');
        setStatus('clipboard-status', `State: ${state}`, 'success');
    } catch (e) {
        setStatus('clipboard-status', e.message || 'Error', 'error');
    }
}

async function requestClipboard() {
    setStatus('clipboard-status', 'Requesting...', 'loading');
    try {
        await navigator.clipboard.readText();
        setStatus('clipboard-status', 'Granted', 'success');
    } catch (e) {
        setStatus('clipboard-status', 'Denied', 'error');
    }
}

/* ---------- Persistent Storage ---------- */

async function checkStorage() {
    setStatus('storage-status', 'Checking...', 'loading');
    try {
        const granted = await navigator.storage.persisted();
        setStatus('storage-status', granted ? 'Granted' : 'Not granted', granted ? 'success' : '');
    } catch (e) {
        setStatus('storage-status', e.message, 'error');
    }
}

async function requestStorage() {
    setStatus('storage-status', 'Requesting...', 'loading');
    try {
        const granted = await navigator.storage.persist();
        setStatus('storage-status', granted ? 'Granted' : 'Denied', granted ? 'success' : 'error');
    } catch (e) {
        setStatus('storage-status', e.message, 'error');
    }
}

/* ---------- Wake Lock ---------- */

let wakeLock;

async function checkWakeLock() {
    setStatus('wakelock-status', 'Supported', 'success');
}

async function requestWakeLock() {
    setStatus('wakelock-status', 'Requesting...', 'loading');
    try {
        wakeLock = await navigator.wakeLock.request('screen');
        setStatus('wakelock-status', 'Granted', 'success');
    } catch (e) {
        setStatus('wakelock-status', 'Denied', 'error');
    }
}

/* ---------- Bluetooth ---------- */

async function checkBluetooth() {
    setStatus('bluetooth-status', 'Supported', 'success');
}

async function requestBluetooth() {
    setStatus('bluetooth-status', 'Requesting...', 'loading');
    try {
        await navigator.bluetooth.requestDevice({ acceptAllDevices: true });
        setStatus('bluetooth-status', 'Granted', 'success');
    } catch {
        setStatus('bluetooth-status', 'Denied', 'error');
    }
}

/* ---------- USB ---------- */

async function checkUSB() {
    setStatus('usb-status', 'Supported', 'success');
}

async function requestUSB() {
    setStatus('usb-status', 'Requesting...', 'loading');
    try {
        await navigator.usb.requestDevice({ filters: [] });
        setStatus('usb-status', 'Granted', 'success');
    } catch {
        setStatus('usb-status', 'Denied', 'error');
    }
}

/* ---------- Serial ---------- */

async function checkSerial() {
    setStatus('serial-status', 'Supported', 'success');
}

async function requestSerial() {
    setStatus('serial-status', 'Requesting...', 'loading');
    try {
        await navigator.serial.requestPort();
        setStatus('serial-status', 'Granted', 'success');
    } catch {
        setStatus('serial-status', 'Denied', 'error');
    }
}

/* ---------- HID ---------- */

async function checkHID() {
    setStatus('hid-status', 'Supported', 'success');
}

async function requestHID() {
    setStatus('hid-status', 'Requesting...', 'loading');
    try {
        await navigator.hid.requestDevice({ filters: [] });
        setStatus('hid-status', 'Granted', 'success');
    } catch {
        setStatus('hid-status', 'Denied', 'error');
    }
}

/* ---------- Idle Detection ---------- */

async function checkIdle() {
    setStatus('idle-status', 'Checking...', 'loading');
    try {
        const state = await queryPermission('idle-detection');
        setStatus('idle-status', `State: ${state}`, 'success');
    } catch (e) {
        setStatus('idle-status', e.message, 'error');
    }
}

async function requestIdle() {
    setStatus('idle-status', 'Requesting...', 'loading');

    try {
        const permission = await IdleDetector.requestPermission();

        if (permission === 'granted') {
            setStatus('idle-status', 'Granted', 'success');
        } else {
            setStatus('idle-status', 'Denied', 'error');
        }
    } catch (err) {
        setStatus('idle-status', 'Error requesting permission', 'error');
    }
}


/* ---------- Clipboard Write ---------- */

async function checkClipboardWrite() {
    setStatus('clipboard-write-status', 'Checking...', 'loading');
    try {
        const state = await queryPermission('clipboard-write');
        setStatus('clipboard-write-status', `State: ${state}`, 'success');
    } catch (e) {
        setStatus('clipboard-write-status', e.message, 'error');
    }
}

async function requestClipboardWrite() {
    setStatus('clipboard-write-status', 'Requesting...', 'loading');
    try {
        await navigator.clipboard.writeText('Test');
        setStatus('clipboard-write-status', 'Granted', 'success');
    } catch {
        setStatus('clipboard-write-status', 'Denied', 'error');
    }
}


/* ---------- Bindings ---------- */

function bind() {
    document.getElementById('camera-check-btn').addEventListener('click', checkCamera);
    document.getElementById('camera-request-btn').addEventListener('click', requestCamera);

    document.getElementById('mic-check-btn').addEventListener('click', checkMicrophone);
    document.getElementById('mic-request-btn').addEventListener('click', requestMicrophone);

    document.getElementById('geo-check-btn').addEventListener('click', checkGeolocation);
    document.getElementById('geo-request-btn').addEventListener('click', requestGeolocation);

    document.getElementById('notif-check-btn').addEventListener('click', checkNotifications);
    document.getElementById('notif-request-btn').addEventListener('click', requestNotifications);

    document.getElementById('clipboard-check-btn').addEventListener('click', checkClipboard);
    document.getElementById('clipboard-request-btn').addEventListener('click', requestClipboard);
	
	document.getElementById('storage-check-btn').onclick = checkStorage;
	document.getElementById('storage-request-btn').onclick = requestStorage;
	
	document.getElementById('wakelock-check-btn').onclick = checkWakeLock;
	document.getElementById('wakelock-request-btn').onclick = requestWakeLock;
	
	document.getElementById('bluetooth-check-btn').onclick = checkBluetooth;
	document.getElementById('bluetooth-request-btn').onclick = requestBluetooth;
	
	document.getElementById('usb-check-btn').onclick = checkUSB;
	document.getElementById('usb-request-btn').onclick = requestUSB;
	
	document.getElementById('serial-check-btn').onclick = checkSerial;
	document.getElementById('serial-request-btn').onclick = requestSerial;
	
	document.getElementById('hid-check-btn').onclick = checkHID;
	document.getElementById('hid-request-btn').onclick = requestHID;
	
	document.getElementById('idle-check-btn').onclick = checkIdle;
	document.getElementById('idle-request-btn').onclick = requestIdle;
	
	document.getElementById('clipboard-write-check-btn').onclick = checkClipboardWrite;
	document.getElementById('clipboard-write-request-btn').onclick = requestClipboardWrite;
	
}

window.addEventListener('DOMContentLoaded', () => {
    bind();

    const msg = 'Ready';
    setStatus('camera-status', msg);
    setStatus('mic-status', msg);
    setStatus('geo-status', msg);
    setStatus('notif-status', msg);
    setStatus('clipboard-status', msg);
    setStatus('storage-status', msg);
    setStatus('wakelock-status', msg);
    setStatus('bluetooth-status', msg);
    setStatus('usb-status', msg);
    setStatus('serial-status', msg);
    setStatus('hid-status', msg);
    setStatus('idle-status', msg);
    setStatus('clipboard-write-status', msg);
});
