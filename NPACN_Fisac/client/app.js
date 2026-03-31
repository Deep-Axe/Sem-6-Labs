/* app.js — Smart Home Controller web client */
'use strict';

/* ── State ────────────────────────────────────────────────────────────────── */
let ws               = null;
let token            = null;
let savedCredentials = null;
let reconnectTimer   = null;
let devicePollTimer  = null;
let logPollTimer     = null;

/* ── DOM refs ─────────────────────────────────────────────────────────────── */
const loginScreen  = document.getElementById('login-screen');
const dashboard    = document.getElementById('dashboard');
const loginError   = document.getElementById('login-error');
const deviceGrid   = document.getElementById('device-grid');
const logBody      = document.getElementById('log-body');
const connIndicator = document.getElementById('conn-indicator');

/* ── WebSocket URL ────────────────────────────────────────────────────────── */
function getWsUrl() {
  const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
  const host     = window.location.hostname || 'localhost';
  const params   = new URLSearchParams(window.location.search);
  const port     = params.get('port') || '8080';
  return `${protocol}//${host}:${port}`;
}

/* ── Connection ───────────────────────────────────────────────────────────── */
function connect() {
  if (ws && (ws.readyState === WebSocket.CONNECTING ||
             ws.readyState === WebSocket.OPEN)) return;
  
  clearTimeout(reconnectTimer);
  ws = new WebSocket(getWsUrl());

  ws.onopen = () => {
    console.log('[WS] Connected');
    connIndicator.className = 'conn-pill connected';
    connIndicator.querySelector('.conn-text').textContent = 'Live';
    loginError.textContent = '';
    if (savedCredentials) {
      send({ type: 'auth', username: savedCredentials.username, password: savedCredentials.password });
    }
  };

  ws.onmessage = (event) => {
    let msg;
    try { msg = JSON.parse(event.data); }
    catch (e) { return; }

    switch (msg.type) {
      case 'auth_result':
        if (msg.success) {
          token = msg.token;
          showDashboard();
          startPolling();
        } else {
          loginError.textContent = msg.message || 'Login failed.';
          savedCredentials = null;
          showLogin();
        }
        break;
      case 'device_list':
        renderDevices(msg.devices || []);
        break;
      case 'activity_log':
        renderLog(msg.entries || []);
        break;
      case 'command_ack':
        requestDevices();
        requestLog();
        break;
      case 'register_ack':
        if (msg.success) {
          closeAddModal();
          requestDevices();
        } else {
          document.getElementById('modal-error').textContent = msg.message || 'Registration failed.';
        }
        break;
    }
  };

  ws.onclose = () => {
    stopPolling();
    token = null;
    connIndicator.className = 'conn-pill disconnected';
    connIndicator.querySelector('.conn-text').textContent = 'Disconnected';
    showLogin();
    reconnectTimer = setTimeout(connect, 3000);
  };
}

/* ── Auth ─────────────────────────────────────────────────────────────────── */
function login() {
  const user = document.getElementById('username').value.trim();
  const pass = document.getElementById('password').value;
  if (!user || !pass) { loginError.textContent = 'Credentials required.'; return; }
  if (!ws || ws.readyState !== WebSocket.OPEN) { loginError.textContent = 'Connecting...'; return; }
  
  loginError.textContent = '';
  savedCredentials = { username: user, password: pass };
  send({ type: 'auth', username: user, password: pass });
}

function logout() {
  savedCredentials = null;
  token = null;
  stopPolling();
  showLogin();
  if (ws) ws.close();
}

/* ── View Switching ───────────────────────────────────────────────────────── */
function showDashboard() {
  loginScreen.classList.add('hidden');
  dashboard.classList.remove('hidden');
}
function showLogin() {
  loginScreen.classList.remove('hidden');
  dashboard.classList.add('hidden');
}
function showTab(name) {
  document.querySelectorAll('.tab').forEach(t => t.classList.add('hidden'));
  document.querySelectorAll('.nav-btn').forEach(b => b.classList.remove('active'));
  document.getElementById('tab-' + name).classList.remove('hidden');
  document.getElementById('nav-' + name).classList.add('active');
}

/* ── Polling ──────────────────────────────────────────────────────────────── */
function startPolling() {
  stopPolling();
  requestDevices();
  requestLog();
  devicePollTimer = setInterval(requestDevices, 3000);
  logPollTimer    = setInterval(requestLog, 5000);
}
function stopPolling() {
  clearInterval(devicePollTimer);
  clearInterval(logPollTimer);
}

/* ── Message Senders ──────────────────────────────────────────────────────── */
function send(obj) { if (ws && ws.readyState === WebSocket.OPEN) ws.send(JSON.stringify(obj)); }
function requestDevices() { if (token) send({ type: 'get_devices', token }); }
function requestLog()     { if (token) send({ type: 'get_log', token, limit: 20 }); }
function sendCommand(deviceId, action) { if (token) send({ type: 'command', token, device_id: deviceId, action }); }

/* ── Add Device Modal ─────────────────────────────────────────────────────── */
function openAddModal() {
  document.getElementById('modal-overlay').classList.remove('hidden');
}
function closeAddModal() {
  document.getElementById('modal-overlay').classList.add('hidden');
}
function closeModalIfBackdrop(e) {
  if (e.target === document.getElementById('modal-overlay')) closeAddModal();
}
function registerDevice() {
  const devId = document.getElementById('new-device-id').value.trim();
  const devType = document.getElementById('new-device-type').value.trim();
  const state = document.getElementById('new-device-state').value;
  if (!devId || !devType || !token) return;
  send({ 
    type: 'pre_register_device', 
    token: token,
    device_id: devId, 
    device_type: devType, 
    initial_state: state 
  });
}

/* ── Rendering ────────────────────────────────────────────────────────────── */
function renderDevices(devices) {
  deviceGrid.innerHTML = '';
  const online = devices.filter(d => d.state !== 'offline').length;
  document.getElementById('stat-total').textContent = devices.length;
  document.getElementById('stat-online').textContent = online;
  document.getElementById('stat-offline').textContent = devices.length - online;

  devices.forEach(dev => {
    const card = document.createElement('div');
    card.className = 'device-card';
    const nextAction = dev.state === 'off' ? 'on' : (dev.state === 'on' ? 'off' : (dev.state === 'active' ? 'inactive' : 'active'));
    
    card.innerHTML = `
      <div class="device-type-label">${esc(dev.device_type)}</div>
      <div class="device-id">${esc(dev.device_id)}</div>
      <div class="device-state state-${dev.state.toLowerCase()}">${dev.state}</div>
      <button class="device-toggle" ${dev.state === 'offline' ? 'disabled' : ''} onclick="sendCommand('${esc(dev.device_id)}','${nextAction}')">
        ${dev.state === 'offline' ? 'Offline' : 'Toggle State'}
      </button>`;
    deviceGrid.appendChild(card);
  });
}

function renderLog(entries) {
  logBody.innerHTML = '';
  entries.forEach(entry => {
    const row = document.createElement('tr');
    const time = new Date(entry.timestamp * 1000).toLocaleTimeString();
    row.innerHTML = `<td>${time}</td><td>${esc(entry.username)}</td><td>${esc(entry.device_id)}</td><td>${esc(entry.action)}</td>`;
    logBody.appendChild(row);
  });
}

function esc(str) {
  return String(str).replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;').replace(/"/g,'&quot;');
}

connect();
