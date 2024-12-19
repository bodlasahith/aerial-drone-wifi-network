import socket
import os
import time
from datetime import datetime

# Receiver Configuration
HOST = "172.20.10.8"
PORT = 5001
SAVE_DIR = "received_files"
LOG_FILE = "receive_log.txt"

# Ensure save directory exists
os.makedirs(SAVE_DIR, exist_ok=True)

def receive_file():
    try:
        # Create socket
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.bind((HOST, PORT))
            s.listen(1)
            print(f"Listening on {HOST}:{PORT}")
            
            conn, addr = s.accept()
            with conn:
                print(f"Connected by {addr}")
                
                # Receive metadata
                metadata = conn.recv(1024).decode()
                file_name, file_size = metadata.split("|")
                file_size = int(file_size)
                print(f"Receiving {file_name} ({file_size} bytes)")
                
                # Receive file content
                file_path = os.path.join(SAVE_DIR, file_name)
                start_time = time.time()
                with open(file_path, "wb") as f:
                    received_size = 0
                    while received_size < file_size:
                        chunk = conn.recv(1024)
                        if not chunk:
                            break
                        f.write(chunk)
                        received_size += len(chunk)
                end_time = time.time()

                # Log receive time
                with open(LOG_FILE, "a") as log:
                    log.write(f"{datetime.now()} - Received {file_name} ({file_size} bytes) in {end_time - start_time:.2f} seconds\n")
                
                print("File received successfully.")
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    receive_file()
