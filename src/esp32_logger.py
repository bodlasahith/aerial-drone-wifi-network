import serial
import time

# sudo usermod -a -G dialout $USER
# sudo chmod 666 /dev/ttyUSB0
SERIAL_PORT = '/dev/ttyUSB0'
BAUD_RATE = 115200
OUTPUT_FILE = '/home/sahithbodla/Documents/esp32_logs.txt'

def log_serial_output():
  try:
    with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as ser, open(OUTPUT_FILE, 'a') as log_file:
      print(f"Logging ESP32 output from {SERIAL_PORT}...")
      while True:
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        if line:
          log_file.write(f"{time.strftime('%Y-%m-%d %H:%M:%S')} - {line}\n")
          log_file.flush()
          print(line)
  except Exception as e:
    print(f"Error: {e}")

if __name__ == "__main__":
  log_serial_output()
