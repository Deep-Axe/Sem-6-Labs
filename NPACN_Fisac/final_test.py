import websocket
import json
import subprocess
import time
import os

# This script performs a full integration test of the Smart Home Controller.
# It starts the server, the device simulator, and then acts as a web client
# to verify authentication, device listing, command forwarding, and state updates.

def integration_test():
    print("--- Starting Integration Test ---")
    
    # 1. Setup: Clean up previous database
    if os.path.exists("server/smarthome.db"):
        print("[Setup] Removing old database...")
        os.remove("server/smarthome.db")
        
    # 2. Start the C Server
    print("[Server] Starting server on port 8088...")
    server_proc = subprocess.Popen(["./server", "8088"], cwd="server")
    time.sleep(2) # Give server time to bind

    try:
        # 3. Start the Python Device Simulator
        print("[Simulator] Starting simulator...")
        sim_proc = subprocess.Popen(["python3", "simulator/device_sim.py", "--port", "8088"])
        time.sleep(2) # Give devices time to register

        # 4. Connect as a Web Client
        print("[Client] Connecting to WebSocket server...")
        ws = websocket.create_connection("ws://localhost:8088")
        
        # 5. Login Flow
        print("[Client] Logging in as admin...")
        ws.send(json.dumps({
            "type": "auth", 
            "username": "admin", 
            "password": "admin123"
        }))
        res = json.loads(ws.recv())
        assert res["success"] == True, "Login failed!"
        token = res["token"]
        print(f"[Client] Login successful. Token: {token[:10]}...")

        # 6. Verify Device Registration
        print("[Client] Requesting device list...")
        ws.send(json.dumps({
            "type": "get_devices", 
            "token": token
        }))
        res = json.loads(ws.recv())
        assert res["type"] == "device_list"
        assert len(res["devices"]) == 3, f"Expected 3 devices, found {len(res['devices'])}"
        device_ids = [d['device_id'] for d in res['devices']]
        print(f"[Client] Devices found: {device_ids}")

        # 7. Test Command Forwarding
        print("[Client] Sending 'on' command to light_1...")
        ws.send(json.dumps({
            "type": "command", 
            "token": token, 
            "device_id": "light_1", 
            "action": "on"
        }))
        res = json.loads(ws.recv())
        assert res["type"] == "command_ack"
        assert res["success"] == True
        print("[Client] Command acknowledgment received")

        # 8. Verify State Update (Polling)
        print("[Client] Waiting 2s for simulator to process and server to update...")
        time.sleep(2)
        ws.send(json.dumps({
            "type": "get_devices", 
            "token": token
        }))
        res = json.loads(ws.recv())
        light = next(d for d in res["devices"] if d["device_id"] == "light_1")
        assert light["state"] == "on", f"State update failed! light_1 is still {light['state']}"
        print("[Client] State update verified: light_1 is now 'on'")

        ws.close()
        print("\n--- ALL INTEGRATION TESTS PASSED ---")

    except Exception as e:
        print(f"\n[!] TEST FAILED: {e}")
    finally:
        print("[Cleanup] Shutting down processes...")
        sim_proc.kill()
        server_proc.terminate()
        server_proc.wait()
        # Note: server/smarthome.db is preserved for your inspection

if __name__ == "__main__":
    integration_test()
