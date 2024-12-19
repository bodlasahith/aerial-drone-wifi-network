# Drone-Based Aerial WiFi Network

This project evaluates network communication between Raspberry Pis and ESP32s mounted on drones. The goal is to analyze connectivity metrics such as latency, throughput, signal strength, and packet loss under varying distances and environmental conditions.

## Table of Contents

- [Overview](#overview)
- [Requirements](#requirements)
  - [Hardware](#hardware)
  - [Software](#software)
- [Features](#features)
- [Usage](#usage)
  - [Computer Setup](#computer-setup)
  - [Raspberry Pi Setup](#raspberry-pi-setup)
  - [ESP32 Setup](#esp32-setup)
  - [Drone Setup](#drone-setup)

---

## Overview

This project uses:

1. **Raspberry Pis**: Testing WiFi-based communication using tools like `iperf3`.
2. **ESP32s**: Testing direct MAC-address pings and hotspot communication.
3. **Drones**: Serving as mobile platforms for the devices to simulate real-world distance and environmental variations.

The results will help assess the feasibility of deploying such systems for extended range WiFi networks or mesh networking solutions.

---

## Requirements

### Hardware

- Raspberry Pi 4 (or similar)
- ESP32-C3-DevKitM-1 microcontrollers (or similar)
- Power sources for Raspberry Pis and ESP32s (e.g., LiPo batteries, power bank)
- Drones capable of carrying the above devices
- WiFi hotspot (e.g., a smartphone)

### Software

- Raspberry Pi OS (64-bit w/ Desktop)
- `iperf3` for network testing
- PlatformIO IDE Extension on VS Code or Arduino IDE for ESP32 programming

---

## Features

- Measure latency, throughput, and signal strength between devices.
- Test communication over WiFi or direct MAC addressing.
- Log results to local files for later analysis.
- Evaluate performance under various distances and conditions (e.g., indoor/outdoor, LOS, obstacles).

---

## Usage

### Computer Setup

1. **Hotspot**: Turn on the smartphone hotspot and connect the computer to it
2. **PlatformIO IDE**: Install the PlatformIO IDE to upload and monitor communication between the ESP32s
3. **Change IP Address**: If the computer's IP is in a different network (has different first octet), manually change the IPv4 configuration and subnet mask to be able to connect to Pis
4. **Ping or ssh into Pis**: Test connection with Pis over the network or ssh into the Pis to run code

```
ping @<pi-address>
```

```
ssh USERID@<pi-address>
```

### Raspberry Pi Setup

1. **SD Card OS**: Install the specified OS onto an SD Card using Raspberry Pi Imager and insert the card into the Pi:

- Create username and password for pi
- Ensure ssh is enabled via password
- Add hotspot SSID and password information
- Check location to user country code

2. **Pi Connectivity**: Connect each Pi to a power source and to the hotspot
3. **Assign Static IPs**: Edit the dhcpcd.conf file with `sudo nano /etc/dhcpcd.conf`:
   ```
   interface wlan0
   static ip_address=172.20.10.X
   static routers=172.20.10.X
   static domain_name_servers=8.8.8.8
   ```
4. **Install `iperf3`:**
   ```bash
   sudo apt update
   sudo apt install iperf3
   ```
5. **Run `iperf3` and ESP32s manually**:

- One one pi, start the server:

```
iperf3 -s
```

- On another pi, start the client:

```
iperf3 -c <server_ip> -t
```

- Alternatively, to run the client infinitely until user interrupt and store ping logs locally:

```
./iperf3_loop.sh
```

- To log ESP32 ping data to pi local storage, plug in the ESP32s into each pi's USB ports and enable serial port permissions to run `esp32_logger.py`:

```
sudo usermod -a -G dialout $USER
sudo chmod 666 /dev/ttyUSB0
```

- Copy the `esp32_logger.py` file into the respective directory and run it

```
sudo python3 esp32_logger.py
```

6. **Run `iperf3` and ESP32s automatically on reboot**

- To run the server and client with iperf3 and ESP32s infinitely on pi reboot until user interrupt/shutdown, create a crontab file following the lines in `crontab.txt`.

### ESP32 Setup

1. **Code Enabling**: Choose which type of communication

- With WiFi, enable the IP Address server/client code by uncommenting the define header
- Without WiFi using ESP-NOW, enable the MAC Address server/client code by uncommenting the define header

```
// #define DEVICE_ROLE_SERVER_IP
// #define DEVICE_ROLE_CLIENT_IP
// #define DEVICE_ROLE_SERVER_MAC
// #define DEVICE_ROLE_CLIENT_MAC
```

2. **Plug in ESP32s**: Plug the modules into the respective ports for the ENVs listed in `platformio.ini` for client and server
3. **Upload Code**: Build and upload respective server and client code to each ESP32 by holding down the BOOT button:

```
pio run -t upload -e esp32_server
```

```
pio run -t upload -e esp32_client
```

4. **Monitor Communication**: Open the serial monitor to check pings/pongs between the ESPs and reset the monitor with the RST button:

```
pio device monitor -e esp32_server
```

```
pio device monitor -e esp32_client
```

### Drone Setup

1. **Power bank**: Mount the power supply device to the drone using command strips
2. **Pi case**: Insert the pis into 3D-printed cases.
3. **Secure Pis**: Zip-tie and attach velcro command strips to the chassis of the drone to secure the pis/ESP32s/power banks.

Fly the drones and test various distance between devices, altitudes, and environmental conditions.