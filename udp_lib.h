#ifndef UDP_LIB
#define UDP_LIB

#include <WiFiS3.h>
#include <WiFiUdp.h>

#include "config.h"

void connect_wifi();
bool begin_udp();
void write(char *message);
bool read(char *buffer, uint8_t size);
void print_wifi_status();

WiFiUDP Udp;
IPAddress remoteIP;

bool udp_setup() {
  if (!remoteIP.fromString(REMOTE_IP)) {
    Serial.println("Invalid remote IP address format.");
    return false;
  }

  Udp.begin(LOCAL_PORT);
  return true;
}

void udp_write(byte *message, size_t size) {
  if (debug_midi) {
    for (int i = 0; i < 4; i++)
      Serial.print((uint8_t)message[i] + " ");
    Serial.println();
  }
  if (Udp.beginPacket(remoteIP, REMOTE_PORT)) {
    Udp.write(message, size);
    Udp.endPacket();
  } else {
    Serial.println("Failed to begin UDP packet!");
  }
}

bool udp_read(char *buffer, uint8_t size) {
  int packet_size = Udp.parsePacket();

  if (packet_size) {
    int len = Udp.read(buffer, size);
    if (len)
      buffer[len] = '\0';
    return true;
  }

  return false;
}

#endif
