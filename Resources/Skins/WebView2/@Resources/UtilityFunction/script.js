function setStatus(id, text, cls) {
    const el = document.getElementById(id);
    el.textContent = text;
    el.className = `status ${cls || ''}`.trim();
}

function getSyncApi() {
    return window.RainmeterAPI || (window.chrome && window.chrome.webview && window.chrome.webview.hostObjects && window.chrome.webview.hostObjects.sync && window.chrome.webview.hostObjects.sync.RainmeterAPI);
}

async function doReplace() {
    const input = document.getElementById('replace-input').value || '';
    setStatus('replace-status', 'Replacing...', 'loading');
    try {
        const api = getSyncApi();
        if (!api || typeof api.ReplaceVariables !== 'function') throw new Error('RainmeterAPI not available');
        const value = api.ReplaceVariables(input);
        document.getElementById('replace-output').textContent = value;
        setStatus('replace-status', 'Done', 'success');
    } catch (e) {
        setStatus('replace-status', e.message || 'Error', 'error');
    }
}

async function doGetVariable() {
    const name = document.getElementById('variable-input').value || '';
    setStatus('variable-status', 'Getting...', 'loading');
    try {
        const api = getSyncApi();
        if (!api || typeof api.GetVariable !== 'function') throw new Error('RainmeterAPI not available');
        const value = api.GetVariable(name);
        document.getElementById('variable-output').textContent = value;
        setStatus('variable-status', 'Done', 'success');
    } catch (e) {
        setStatus('variable-status', e.message || 'Error', 'error');
    }
}

async function doPathToAbsolute() {
    const path = document.getElementById('path-input').value || '';
    setStatus('path-status', 'Converting...', 'loading');
    try {
        const api = getSyncApi();
        if (!api || typeof api.PathToAbsolute !== 'function') throw new Error('RainmeterAPI not available');
        const value = api.PathToAbsolute(path);
        document.getElementById('path-output').textContent = value;
        setStatus('path-status', 'Done', 'success');
    } catch (e) {
        setStatus('path-status', e.message || 'Error', 'error');
    }
}

function doBang() {
    const cmd = document.getElementById('bang-input').value || '';
    try {
        const api = getSyncApi();
        if (!api || typeof api.Bang !== 'function') throw new Error('RainmeterAPI not available');
        api.Bang(cmd);
        setStatus('bang-status', `Executed: ${cmd}`, 'success');
    } catch (e) {
        setStatus('bang-status', e.message || 'Error', 'error');
    }
}

function doLog() {
    const msg = document.getElementById('log-input').value || '';
    const level = document.getElementById('level-select').value;
    try {
        const api = getSyncApi();
        if (!api || typeof api.Log !== 'function') throw new Error('RainmeterAPI not available');
        api.Log(msg, level);
        setStatus('log-status', `${level}: ${msg}`, 'success');
    } catch (e) {
        setStatus('log-status', e.message || 'Error', 'error');
    }
}

function bind() {
    document.getElementById('replace-btn').addEventListener('click', doReplace);
    document.getElementById('variable-btn').addEventListener('click', doGetVariable);
    document.getElementById('path-btn').addEventListener('click', doPathToAbsolute);
    document.getElementById('bang-btn').addEventListener('click', doBang);
    document.getElementById('log-btn').addEventListener('click', doLog);
    document.getElementById('demo-btn').addEventListener('click', runDemo);
}

function runDemo() {
    document.getElementById('replace-input').value = 'Config: #CURRENTCONFIG# / Var: #DemoVariable#';
    document.getElementById('variable-input').value = 'DemoVariable';
    document.getElementById('path-input').value = 'UtilityFunction.ini';
    document.getElementById('log-input').value = 'Hello from WebView2';
    document.getElementById('level-select').value = 'Notice';
    doReplace();
    doGetVariable();
    doPathToAbsolute();
    doBang();
    doLog();
}

window.addEventListener('DOMContentLoaded', () => {
    bind();
    const ready = !!getSyncApi();
    const msg = ready ? 'API Ready' : 'Waiting for API...';
    const cls = ready ? 'loading' : 'error';
    setStatus('replace-status', msg, cls);
    setStatus('variable-status', msg, cls);
    setStatus('path-status', msg, cls);
    setStatus('bang-status', msg, cls);
    setStatus('log-status', msg, cls);
});
