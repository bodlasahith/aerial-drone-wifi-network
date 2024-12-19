#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <esp_now.h>

#define WIFI_SSID "Sahiths iphone" // modify this to the SSID of the base station Wi-Fi network
#define WIFI_PASS "123456sb"       // modify this to the password of the base station Wi-Fi network
#define SERVER_IP "172.20.10.8"    // modify this to the IP address of the server
#define PORT_NUM 1234
#define BAUD_RATE 115200
#define FILE_PATH "/src/10mb_file"
#define CHUNK_SIZE 250

// #define DEVICE_ROLE_SERVER_IP
// #define DEVICE_ROLE_CLIENT_IP
// #define DEVICE_ROLE_SERVER_MAC
// #define DEVICE_ROLE_CLIENT_MAC

#ifdef DEVICE_ROLE_SERVER_IP

WiFiServer server(PORT_NUM);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(BAUD_RATE);
  pinMode(LED_BUILTIN, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  server.begin();
  Serial.println("Server started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
  // put your main code here, to run repeatedly:
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Client connected");
    while (client.connected()) {
      if (client.available()) {
        String request = client.readStringUntil('\n');
        Serial.print("Received: ");
        Serial.println(request);
        client.print("Pong: ");
      }
    }
    client.stop();
    Serial.println("Client disconnected");
  }
}

#endif

#ifdef DEVICE_ROLE_CLIENT_IP

WiFiClient client;

void setup() {
  Serial.begin(BAUD_RATE);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Connect to the server
  if (client.connect(SERVER_IP, PORT_NUM)) {
    Serial.println("Connected to server");
  } else {
    Serial.println("Connection to server failed");
    ESP.restart();
  }
}

void loop() {
  if (client.connected()) {
    client.println("Ping"); // Send Ping
    Serial.println("Sent: Ping");

    // Wait for response
    if (client.available()) {
      String response = client.readStringUntil('\n');
      Serial.print("Received: ");
      Serial.println(response);
    }
  } else {
    Serial.println("Disconnected from server");
    delay(1000);
    ESP.restart(); // Reconnect
  }
  delay(1000); // Ping every second
}

#endif

#ifdef DEVICE_ROLE_SERVER_MAC

uint8_t peer_mac_addr[] = {0x58, 0xCF, 0x79, 0x04, 0x40, 0x10};

// Callback when data is received
void onReceive(const uint8_t *mac_addr, const uint8_t *data, int len) {
  static File file;
  static bool receivingFile = false;

  if (strncmp((char *)data, "START", len) == 0) {
    // Start receiving file
    file = SPIFFS.open(FILE_PATH, FILE_WRITE);
    receivingFile = true;
    Serial.println("Receiving file...");
  } else if (strncmp((char *)data, "END", len) == 0) {
    // End of file
    file.close();
    receivingFile = false;
    Serial.println("File received.");
  } else if (receivingFile)
  {
    // Write chunk to file
    file.write(data, len);

    // Print received chunk as hexadecimal string
    Serial.print("Received chunk: ");
    for (int i = 0; i < len; i++) {
      Serial.printf("%02X ", data[i]);
    }
    Serial.println();
  }
}

void sendFile(const uint8_t *mac_addr) {
  File file = SPIFFS.open(FILE_PATH, FILE_READ);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  // Send start signal
  const char *startSignal = "START";
  esp_now_send(mac_addr, (uint8_t *)startSignal, strlen(startSignal));

  // Send file in chunks
  uint8_t buffer[CHUNK_SIZE];
  while (file.available()) {
    int bytesRead = file.read(buffer, CHUNK_SIZE);
    esp_now_send(mac_addr, buffer, bytesRead);
    delay(10); // Small delay to ensure reliable transmission
  }

  // Send end signal
  const char *endSignal = "END";
  esp_now_send(mac_addr, (uint8_t *)endSignal, strlen(endSignal));

  file.close();
  Serial.println("File sent from server.");
}

void setup() {
  Serial.begin(BAUD_RATE);

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Initialize WiFi in STA mode
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  Serial.println("ESP-NOW Initialized");
  // Register receive callback
  esp_now_register_recv_cb(onReceive);

  // Add peer (replace with actual MAC address)
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, peer_mac_addr, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Send file to peer
  sendFile(peerInfo.peer_addr);
}

void loop() {
  // put your main code here, to run repeatedly:
  sendFile(peer_mac_addr);
  Serial.println("10s Delaying...");
  delay(10000); // 10 seconds delay
}

#endif

#ifdef DEVICE_ROLE_CLIENT_MAC

uint8_t peer_mac_addr[] = {0x58, 0xCF, 0x79, 0x04, 0x3E, 0x9C};

// Callback when data is received
void onReceive(const uint8_t *mac_addr, const uint8_t *data, int len) {
  static File file;
  static bool receivingFile = false;

  if (strncmp((char *)data, "START", len) == 0) {
    // Start receiving file
    file = SPIFFS.open(FILE_PATH, FILE_WRITE);
    receivingFile = true;
    Serial.println("Receiving file...");
  } else if (strncmp((char *)data, "END", len) == 0) {
    // End of file
    file.close();
    receivingFile = false;
    Serial.println("File received.");
  } else if (receivingFile) {
    // Write chunk to file
    file.write(data, len);
    Serial.print("Received chunk: ");
    Serial.write(data, len);
    Serial.println();
  }
}

void sendFile(const uint8_t *mac_addr) {
  File file = SPIFFS.open(FILE_PATH, FILE_READ);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  // Send start signal
  const char *startSignal = "START";
  esp_now_send(mac_addr, (uint8_t *)startSignal, strlen(startSignal));

  // Send file in chunks
  uint8_t buffer[CHUNK_SIZE];
  while (file.available()) {
    int bytesRead = file.read(buffer, CHUNK_SIZE);
    esp_now_send(mac_addr, buffer, bytesRead);
    delay(10); // Small delay to ensure reliable transmission
  }

  // Send end signal
  const char *endSignal = "END";
  esp_now_send(mac_addr, (uint8_t *)endSignal, strlen(endSignal));

  file.close();
  Serial.println("File sent from client.");
}

void setup() {
  Serial.begin(BAUD_RATE);

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Initialize WiFi in STA mode
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  Serial.println("ESP-NOW Initialized");

  // Register receive callback
  esp_now_register_recv_cb(onReceive);

  // Add peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, peer_mac_addr, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Send file to peer
  sendFile(peerInfo.peer_addr);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Send file to peer repeatedly every 10 seconds
  sendFile(peer_mac_addr);
  Serial.println("10s Delaying...");
  delay(10000); // 10 seconds delay
}

#endif
