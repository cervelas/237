#include <WiFiS3.h>
#include <WiFiUdp.h>

// WiFi credentials
const char ssid[] = "Je suis trop un bobet";
const char pass[] = "Wifi1012";

// UDP settings
const char* message = "Hello from Arduino R4!";
const char* remoteIpStr = "192.168.0.105";
const unsigned int remotePort = 5005;
const unsigned int localPort = 2390;

WiFiUDP Udp;
IPAddress remoteIP;

void setup() {
  Serial.begin(9600);
  while (!Serial);  // Wait for Serial Monitor

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("WiFi module not found.");
    while (true);
  }

  connectToWiFi();
  printWifiStatus();

  if (!remoteIP.fromString(remoteIpStr)) {
    Serial.println("Invalid remote IP address format.");
    while (true);
  }

  Udp.begin(localPort);
  Serial.println("UDP ready. Sending packets...");
}

void loop() {
  Udp.beginPacket(remoteIP, remotePort);
  Udp.write(message);
  Udp.endPacket();

  Serial.println("UDP packet sent.");

  delay(1000);  // Adjust delay as needed
}

void connectToWiFi() {
  int status = WL_IDLE_STATUS;

  while (status != WL_CONNECTED) {
    Serial.print("Connecting to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(5000);
  }

  Serial.println("Connected to WiFi.");
}

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Signal Strength (RSSI): ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
}
