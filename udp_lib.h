#ifndef UDP_LIB
#define UDP_LIB

#include <WiFiS3.h>
#include <WiFiUdp.h>

#include "ledmatrix.h"

void connect_wifi();
bool begin_udp();
void write(char* message);
bool read(char* buffer, uint8_t size);
void print_wifi_status();

WiFiUDP Udp;
IPAddress remote;

volatile int32_t timeOffset = 0;

void udp_setup() {
    remote = WiFi.localIP();
    remote[3] = midi_remote_id.get();

    // if (Udp.begin(5000 + UID)) {
    if (Udp.begin(5000)) {
        Serial.println("Will send packets to: " + remote.toString());
        udp_connected = true;
    }
}

void udp_write(byte* message, size_t size) {
    // if (debug_midi) {
    //     for (int i = 0; i < 4; i++)
    //         Serial.print((uint8_t)message[i] + " ");
    //     Serial.println();
    // }
    if (Udp.beginPacket(remote, 5000)) {
        Udp.write(message, size);
        Udp.endPacket();
    } else {
        set_matrix_text("FAIL");
    }
}

bool udp_read(char* buffer, uint8_t size) {
    int packet_size = Udp.parsePacket();

    if (packet_size) {
        int len = Udp.read(buffer, size);
        if (len)
            buffer[len] = '\0';
        return true;
    }

    return false;
}

void check_time_sync() {
    int packetSize = Udp.parsePacket();
    if (packetSize == 4) {
        byte buffer[4];
        Udp.read(buffer, 4);
        uint32_t remoteTime = ((uint32_t)buffer[0] << 24) |
                              ((uint32_t)buffer[1] << 16) |
                              ((uint32_t)buffer[2] << 8) | (uint32_t)buffer[3];
        uint32_t receiveTime = millis();
        timeOffset = (int32_t)remoteTime - (int32_t)receiveTime;

        Serial.println("Recieved time sync: " + format_time(remoteTime));
    }
}

uint32_t syncedTime() { return millis() + timeOffset; }

#endif
