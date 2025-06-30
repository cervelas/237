#ifndef __NET_H
#define __NET_H
#include "WiFiS3.h"
#include "ledmatrix.h"

char ap_ssid[] = "ARDUINO";  // your network SSID (name)
char ap_pass[] =
    "12345678";  // your network password (use for WPA, or use as key for WEP)

arduino::IPAddress ap_ip = IPAddress(10, 10, 1, UID);

bool dhcp = false;

int net_status = WL_IDLE_STATUS;

void wifi_setup_client() {
    set_matrix_text("WIFI");

    IPAddress client_ip = IPAddress(192, 168, 2, UID.get());

    Serial.println("Client ip: " + client_ip.toString());

    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
        // don't continue
        leds_error();
        while (true)
            ;
    }

    String fv = WiFi.firmwareVersion();

    if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
        Serial.println("Please upgrade the firmware");
        leds_error();
        while (true)
            ;
    }

    // attempt to connect to WiFi network:
    if (!dhcp)
        WiFi.config(client_ip);

    uint8_t no_tries = 0;
    while (net_status != WL_CONNECTED && no_tries < 5) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(client_ssid);
        Serial.println("Attempt: " + String(no_tries + 1));

        net_status = WiFi.begin(client_ssid, client_pass);

        delay(2000);
        no_tries++;
    }

    wifi_connected = net_status == WL_CONNECTED;

    if (net_status != WL_CONNECTED) {
        Serial.println("Could not connect to WIFI");
        leds_error();
        while (true)
            ;
    }

    Serial.println("Started WiFi client");
}

void wifi_setup_ap() {
    set_matrix_text("WIFI");

    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
        // don't continue
        leds_error();
        while (true)
            ;
    }

    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
        Serial.println("Please upgrade the firmware");
        leds_error();
        while (true)
            ;
    }

    WiFi.config(ap_ip);

    Serial.print("Creating access point named: ");
    Serial.println(ap_ssid);

    net_status = WiFi.beginAP(ap_ssid, ap_pass);

    if (net_status != WL_AP_LISTENING) {
        Serial.println("Creating access point failed");
        // don't continue
        leds_error();
        while (true)
            ;
    }

    Serial.println("Started WiFi AP");

    set_matrix_text(WiFi.localIP().toString());
}

void wifi_client_loop() {
    // compare the previous status to the current status
    if (net_status != WiFi.status()) {
        // it has changed update the variable
        net_status = WiFi.status();
        wifi_connected = net_status == WL_CONNECTED;

        Serial.print("Wifi status has changed (see WiFiTypes.h) : ");
        Serial.println(net_status);
        // Serial.println(wl_status_t);
    }
}

void wifi_ap_loop() {
    // compare the previous status to the current status
    if (net_status != WiFi.status()) {
        // it has changed update the variable
        net_status = WiFi.status();
        wifi_connected = net_status == WL_AP_CONNECTED;

        Serial.print("Wifi status has changed : ");
        Serial.println(net_status);
        // Serial.println(wl_status_t);
    }
}

void printWiFiStatus() {
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your WiFi shield's IP address:
    String ip = WiFi.localIP().toString();
    Serial.print("IP Address: ");
    Serial.println(ip);
}

#endif
