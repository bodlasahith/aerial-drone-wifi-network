SERVER_IP="172.20.10.7"
IPERF_OUTPUT_FILE="/home/sahithbodla/Documents/iperf3_results.txt"
PING_PI_OUTPUT_FILE="/home/sahithbodla/Documents/ping_pi_results.txt"
PING_BASE_OUTPUT_FILE="/home/sahithbodla/Documents/ping_base_results.txt"
DURATION=10 # duration of each test in seconds
INTERVAL=2 # interval between tests in seconds
ESP32_LOGGER="/home/sahithbodla/Documents/esp32_logger.py"
ESP32_LOG_FILE="/home/sahithbodla/Documents/esp32_log.txt"
PING_PI_ADDRESS="172.20.10.7"
PING_BASE_ADDRESS="172.20.10.6"

sudo python3 $ESP32_LOGGER >> $ESP32_LOG_FILE 2>&1 &

while true; do
	date=$(date)
	echo "Running iperf3 test at $date" >> $IPERF_OUTPUT_FILE
	iperf3 -c $SERVER_IP -t $DURATION >> $IPERF_OUTPUT_FILE
	echo "Test complete. Waiting for $INTERVAL seconds..." >> $IPERF_OUTPUT_FILE

	date=$(date)
	echo "Pinging $PING_PI_ADDRESS at $date" >> $PING_PI_OUTPUT_FILE
	ping -c 4 $PING_PI_ADDRESS >> $PING_PI_OUTPUT_FILE
	echo "Ping test complete. Waiting for $INTERVAL seconds..." >> $PING_PI_OUTPUT_FILE

	date=$(date)
	echo "Pinging $PING_BASE_ADDRESS at $date" >> $PING_BASE_OUTPUT_FILE
	ping -c 4 $PING_BASE_ADDRESS >> $PING_BASE_OUTPUT_FILE
	echo "Ping test complete. Waiting for $INTERVAL seconds..." >> $PING_BASE_OUTPUT_FILE

	sleep $INTERVAL
done
