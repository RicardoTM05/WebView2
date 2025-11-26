function getSyncApi() {
    const chromeApi = (window.chrome && window.chrome.webview && window.chrome.webview.hostObjects && window.chrome.webview.hostObjects.sync && window.chrome.webview.hostObjects.sync.RainmeterAPI) || null;
    return chromeApi || window.RainmeterAPI || null;
}

function setStatus(id, text, cls) {
    const el = document.getElementById(id);
    el.textContent = text;
    el.className = `status ${cls || ''}`.trim();
}

async function readProperty(prop, outputId, statusId) {
    setStatus(statusId, 'Reading...', 'loading');
    try {
        const api = getSyncApi();
        if (!api) throw new Error('RainmeterAPI not available');
        let value = api[prop];
        if (typeof value === 'function') {
            value = value();
        }
        value = await Promise.resolve(value);
        document.getElementById(outputId).textContent = value;
        setStatus(statusId, 'Updated', 'success');
    } catch (e) {
        setStatus(statusId, e.message || 'Error', 'error');
    }
}

function refreshAll() {
    readProperty('MeasureName', 'measure-output', 'measure-status');
    readProperty('SkinName', 'skin-output', 'skin-status');
    readProperty('SkinWindowHandle', 'handle-output', 'handle-status');
    readProperty('SettingsFile', 'settings-output', 'settings-status');
}

async function waitForApi(timeoutMs = 3000, intervalMs = 100) {
    const start = Date.now();
    while (Date.now() - start < timeoutMs) {
        const api = getSyncApi();
        if (api) return api;
        await new Promise(r => setTimeout(r, intervalMs));
    }
    return null;
}

window.addEventListener('DOMContentLoaded', async () => {
    const api = await waitForApi();
    const ready = !!api;
    const msg = ready ? 'API Ready' : 'Waiting for API...';
    const cls = ready ? 'loading' : 'error';
    setStatus('measure-status', msg, cls);
    setStatus('skin-status', msg, cls);
    setStatus('handle-status', msg, cls);
    setStatus('settings-status', msg, cls);
    document.getElementById('refresh-btn').addEventListener('click', refreshAll);
    if (ready) refreshAll();
});
