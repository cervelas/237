#ifndef UDP_LIB
#define UDP_LIB

#include <WiFiS3.h>
#include <WiFiUdp.h>

#include "ledmatrix.h"

void connect_wifi();
bool begin_udp();
void write(char *message);
bool read(char *buffer, uint8_t size);
void print_wifi_status();

WiFiUDP Udp;
IPAddress remote;

void udp_setup() {
  remote = WiFi.localIP();
  remote[3] = midi_remote_id.get();

  if (Udp.begin(5000 + UID)) {
    Serial.println("Will send packets to: " + remote.toString());
    midi_connected = true;
  }
}

void udp_write(byte *message, size_t size) {
  if (debug_midi) {
    for (int i = 0; i < 4; i++)
      Serial.print((uint8_t)message[i] + " ");
    Serial.println();
  }
  if (Udp.beginPacket(remote, 5000)) {
    Udp.write(message, size);
    Udp.endPacket();
  } else {
    set_matrix_text("FAIL");
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
