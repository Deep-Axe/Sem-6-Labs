import websocket
import json
import argparse
import time
import threading

class SmartDevice:
    def __init__(self, device_id, device_type, initial_state):
        self.device_id = device_id
        self.device_type = device_type
        self.state = initial_state
        self.ws = None

    def connect(self, host, port):
        uri = f"ws://{host}:{port}"
        try:
            self.ws = websocket.create_connection(uri)
            print(f"[{self.device_id}] Connected to {uri}")
            
            # Register
            self.ws.send(json.dumps({
                "type": "register",
                "device_id": self.device_id,
                "device_type": self.device_type,
                "initial_state": self.state
            }))
            
            res = self.ws.recv()
            print(f"[{self.device_id}] Registration result: {res}")
            
            # Start threads
            threading.Thread(target=self.heartbeat_loop, daemon=True).start()
            self.message_loop()
        except Exception as e:
            print(f"[{self.device_id}] Error: {e}")

    def heartbeat_loop(self):
        while True:
            time.sleep(10)
            if self.ws:
                try:
                    self.ws.send(json.dumps({
                        "type": "heartbeat",
                        "device_id": self.device_id,
                        "state": self.state
                    }))
                    print(f"[{self.device_id}] Heartbeat sent")
                except:
                    break

    def message_loop(self):
        while True:
            try:
                message = self.ws.recv()
                msg = json.loads(message)
                if msg.get("type") == "command":
                    action = msg.get("action")
                    print(f"[{self.device_id}] Received command: {action}")
                    if action in ["on", "active"]:
                        self.state = action
                    elif action in ["off", "inactive"]:
                        self.state = action
                    
                    self.ws.send(json.dumps({
                        "type": "state_update",
                        "device_id": self.device_id,
                        "state": self.state
                    }))
            except:
                break

def run_device(device_id, device_type, initial_state, host, port):
    dev = SmartDevice(device_id, device_type, initial_state)
    dev.connect(host, port)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", default="localhost")
    parser.add_argument("--port", type=int, default=8080)
    args = parser.parse_args()

    devices = [
        ("light_1", "light", "off"),
        ("fan_1", "fan", "off"),
        ("camera_1", "camera", "inactive")
    ]

    threads = []
    for d in devices:
        t = threading.Thread(target=run_device, args=(d[0], d[1], d[2], args.host, args.port))
        t.start()
        threads.append(t)

    for t in threads:
        t.join()

if __name__ == "__main__":
    main()
