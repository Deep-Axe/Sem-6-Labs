# PROTOCOL.md — WebSocket Message Protocol (Complete)

All messages are JSON text frames over WebSocket.
Every message has a `"type"` field. Unknown types are silently ignored after logging.

---

## Connection Identity Phase
*First message after WebSocket handshake determines client type.*

### Web Client Login
**Direction:** Browser → Server
```json
{
  "type": "auth",
  "username": "admin",
  "password": "admin123"
}
```

### Auth Success Response
**Direction:** Server → Browser
```json
{
  "type": "auth_result",
  "success": true,
  "token": "a3f9bc12d4e87f310a2b56cd78e9f012a3f9bc12d4e87f310a2b56cd78e9f012"
}
```

### Auth Failure Response
**Direction:** Server → Browser
```json
{
  "type": "auth_result",
  "success": false,
  "message": "Invalid username or password"
}
```

### IoT Device Registration
**Direction:** Device → Server
```json
{
  "type": "register",
  "device_id": "light_1",
  "device_type": "light",
  "initial_state": "off"
}
```

### Registration Acknowledgement
**Direction:** Server → Device
```json
{
  "type": "register_ack",
  "success": true,
  "device_id": "light_1"
}
```

---

## Authenticated Session Messages
*All messages from web client must include token field after auth.*

### Request Device List
**Direction:** Browser → Server
```json
{
  "type": "get_devices",
  "token": "<session_token>"
}
```

### Device List Response
**Direction:** Server → Browser
```json
{
  "type": "device_list",
  "devices": [
    {
      "device_id": "light_1",
      "device_type": "light",
      "state": "off",
      "last_seen": 1718000000
    },
    {
      "device_id": "fan_1",
      "device_type": "fan",
      "state": "on",
      "last_seen": 1718000010
    },
    {
      "device_id": "camera_1",
      "device_type": "camera",
      "state": "active",
      "last_seen": 1718000005
    }
  ]
}
```

### Send Command to Device
**Direction:** Browser → Server
```json
{
  "type": "command",
  "token": "<session_token>",
  "device_id": "light_1",
  "action": "on"
}
```
Valid actions:
- lights: `"on"`, `"off"`
- fans: `"on"`, `"off"`
- cameras: `"active"`, `"inactive"`

### Command Forwarded to Device
**Direction:** Server → Device
```json
{
  "type": "command",
  "action": "on"
}
```
*(No token — device channel is trusted by registration, not user auth)*

### Command Acknowledgement to Browser
**Direction:** Server → Browser
```json
{
  "type": "command_ack",
  "success": true,
  "device_id": "light_1",
  "action": "on"
}
```

### Command Failure (device offline)
**Direction:** Server → Browser
```json
{
  "type": "command_ack",
  "success": false,
  "device_id": "light_1",
  "message": "Device is offline"
}
```

---

## Device → Server Messages

### Heartbeat
**Direction:** Device → Server (every 10 seconds)
```json
{
  "type": "heartbeat",
  "device_id": "light_1",
  "state": "on"
}
```
*Server updates last_seen in registry and DB. No response sent.*

### State Update (after receiving command)
**Direction:** Device → Server
```json
{
  "type": "state_update",
  "device_id": "light_1",
  "state": "on"
}
```

### Web Clients Poll for Updates
**Direction:** Browser → Server (every 3 seconds)
```json
{
  "type": "get_devices",
  "token": "<session_token>"
}
```
*Note: Due to child process isolation, real-time broadcasts are replaced by client-side polling in this architecture.*

---

## Activity Log Messages

### Request Activity Log
**Direction:** Browser → Server
```json
{
  "type": "get_log",
  "token": "<session_token>",
  "limit": 20
}
```

### Activity Log Response
**Direction:** Server → Browser
```json
{
  "type": "activity_log",
  "entries": [
    {
      "timestamp": 1718000050,
      "username": "admin",
      "device_id": "light_1",
      "action": "on",
      "source_ip": "127.0.0.1"
    }
  ]
}
```

---

## Error Messages

### Generic Error
**Direction:** Server → Client
```json
{
  "type": "error",
  "code": "INVALID_TOKEN",
  "message": "Session token is invalid or expired"
}
```

### Error Codes
| Code | Meaning |
|------|---------|
| `INVALID_TOKEN` | Session token missing or wrong |
| `DEVICE_NOT_FOUND` | device_id does not exist in registry |
| `DEVICE_OFFLINE` | Device registered but currently disconnected |
| `INVALID_ACTION` | Action not valid for this device type |
| `INVALID_MESSAGE` | JSON parse error or missing required field |
| `AUTH_FAILED` | Wrong username or password |
| `PERMISSION_DENIED` | Authenticated but not authorized for action |

---

## Device Simulator Behavior (device_sim.py)

On startup, connects 3 WebSocket clients with these identities:
```
device_id: light_1,  device_type: light,  initial_state: off
device_id: fan_1,    device_type: fan,    initial_state: off
device_id: camera_1, device_type: camera, initial_state: inactive
```

On receiving `{"type":"command","action":"on"}`:
- Update internal state
- Send `{"type":"state_update","device_id":"<id>","state":"on"}`

On receiving `{"type":"command","action":"off"}`:
- Update internal state
- Send `{"type":"state_update","device_id":"<id>","state":"off"}`

Heartbeat loop: every 10 seconds, send heartbeat with current state.

---

## JSON Parsing Strategy in C
Use a minimal hand-rolled parser OR a single-file header library.
Recommended: `cJSON` (single file, MIT license, include directly in server/).
- `cJSON_Parse(buf)` to parse incoming message
- `cJSON_GetObjectItem(root, "type")` to get type
- `cJSON_Print(obj)` to serialize outgoing messages
- Always `cJSON_Delete(root)` after processing
