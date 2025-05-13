#ifndef UDP_LIB
#define UDP_LIB

#include <WiFiS3.h>
#include <WiFiUdp.h>

#include "config.h"

namespace UdpLib {

void connect_wifi();
bool begin_udp();
void write(char* message);
bool read(char* buffer, uint8_t size);
void print_wifi_status();

WiFiUDP Udp;
IPAddress remoteIP;

bool begin_udp() {
    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("WiFi module not found.");
        return false;
    }

    connect_wifi();
#ifdef DEBUG
    print_wifi_status();
#endif

#ifdef WRITE
    if (!remoteIP.fromString(REMOTE_IP)) {
        Serial.println("Invalid remote IP address format.");
        return false;
    }
#endif

    Udp.begin(LOCAL_PORT);
    return true;
}

void write(char* message) {
    if (Udp.beginPacket(remoteIP, REMOTE_PORT)) {
        Udp.write(message);
        Udp.endPacket();
        Serial.println("Packet sent.");
    } else {
        Serial.println("Failed to begin UDP packet!");
    }
}

bool read(char* buffer, uint8_t size) {
    int packet_size = Udp.parsePacket();

    if (packet_size) {
        int len = Udp.read(buffer, size);
        if (len)
            buffer[len] = '\0';
        return true;
    }

    return false;
}

void connect_wifi() {
    int status = WL_IDLE_STATUS;

    while (status != WL_CONNECTED) {
        Serial.print("Connecting to SSID: ");
        Serial.println(WIFI_SSID);
        status = WiFi.begin(WIFI_SSID, WIFI_PASS);
        delay(5000);
    }

    Serial.println("Connected to WiFi.");
}

void print_wifi_status() {
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    Serial.print("Signal Strength (RSSI): ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");

    // #ifdef WRITE
    //     Serial.print("Sending to: ");
    //     Serial.println(REMOTE_IP);
    // #endif
}
}  // namespace UdpLib

#endif
