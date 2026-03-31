import websocket
import sys

def test():
    try:
        ws = websocket.create_connection("ws://localhost:8084")
        print("Connected")
        msg = ws.recv()
        print(f"Received: {msg}")
        ws.close()
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    test()
