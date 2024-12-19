import socket
import os
import time
from datetime import datetime

# Sender Configuration
SERVER_IP = "172.20.10.8"  # Replace with the receiver Pi's IP
PORT = 5001
FILE_PATH = "/src/10mb_file"  # File to send
LOG_FILE = "send_log.txt"

def send_file():
    try:
        # Establish socket connection
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((SERVER_IP, PORT))
            print(f"Connected to {SERVER_IP}:{PORT}")
            
            # Get file size
            file_size = os.path.getsize(FILE_PATH)
            
            # Send file metadata
            s.sendall(f"{os.path.basename(FILE_PATH)}|{file_size}".encode())
            time.sleep(1)  # Short delay to ensure header is sent
            
            # Send file content
            start_time = time.time()
            with open(FILE_PATH, "rb") as f:
                while chunk := f.read(1024):
                    s.sendall(chunk)
            end_time = time.time()

            # Log send time
            with open(LOG_FILE, "a") as log:
                log.write(f"{datetime.now()} - Sent {FILE_PATH} ({file_size} bytes) in {end_time - start_time:.2f} seconds\n")
            
            print("File sent successfully.")
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    send_file()
