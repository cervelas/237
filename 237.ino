#include <EEPROM-Storage.h>

#include "config.h"
#include "utils.h"

bool sensor_enabled = true;

// WiFi related variables
char client_ssid[] = WIFI_SSID;
char client_pass[] = WIFI_PASS;
bool wifi_connected = false;
uint16_t offsetMs = ((_UID - 3) * SLOT_MS) / NUM_DEVICES;

// Storage for UI and midi
EEPROMStorage<uint8_t> UID(0, _UID);

EEPROMStorage<bool> debug_cc(UID.nextAddress(), true);
EEPROMStorage<bool> debug_sensor(debug_cc.nextAddress(), false);
EEPROMStorage<bool> debug_raw(debug_sensor.nextAddress(), false);
EEPROMStorage<bool> debug_midi(debug_raw.nextAddress(), false);

EEPROMStorage<uint8_t> cc_avg(debug_midi.nextAddress(), 3);
EEPROMStorage<uint16_t> cc_smin(cc_avg.nextAddress(), 30);
EEPROMStorage<uint16_t> cc_smax(cc_smin.nextAddress(), 300);
EEPROMStorage<uint16_t> cc_tmin(cc_smax.nextAddress(), 0);
EEPROMStorage<uint16_t> cc_tmax(cc_tmin.nextAddress(), 127);

typedef struct {
    uint16_t min;
    uint16_t max;
    uint8_t note;
} Note, *pnotes;

#define NB_NOTES 10

EEPROMStorage<Note> note1(cc_tmax.nextAddress(), {0, 0, 0});
EEPROMStorage<Note> note2(note1.nextAddress(), {0, 0, 0});
EEPROMStorage<Note> note3(note2.nextAddress(), {0, 0, 0});
EEPROMStorage<Note> note4(note3.nextAddress(), {0, 0, 0});
EEPROMStorage<Note> note5(note4.nextAddress(), {0, 0, 0});
EEPROMStorage<Note> note6(note5.nextAddress(), {0, 0, 0});
EEPROMStorage<Note> note7(note6.nextAddress(), {0, 0, 0});
EEPROMStorage<Note> note8(note7.nextAddress(), {0, 0, 0});
EEPROMStorage<Note> note9(note8.nextAddress(), {0, 0, 0});
EEPROMStorage<Note> note10(note9.nextAddress(), {0, 0, 0});

EEPROMStorage<Note> notes[NB_NOTES] = {note1, note2, note3, note4, note5,
                                       note6, note7, note8, note9, note10};

// Midi global var
EEPROMStorage<bool> udp_connected(note7.nextAddress(), false);
EEPROMStorage<uint8_t> midi_remote_id(udp_connected.nextAddress(), 33);
EEPROMStorage<uint8_t> midi_channel(midi_remote_id.nextAddress(), _UID);
EEPROMStorage<bool> midi_enable(midi_channel.nextAddress(), true);

EEPROMStorage<Note> note_far(midi_enable.nextAddress(), {0, 0, 0});
EEPROMStorage<Note> note_near(note_far.nextAddress(), {0, 0, 0});

String status = "";
String _log = {};

void log(auto text) {
    Serial.print(text);
    _log = String(text);
}

void logln(auto text) {
    Serial.println(text);
    _log = String(text);
}

#include "ledmatrix.h"
#include "midi.h"
#include "network.h"
#include "sensor9960.h"
#include "sensorHC-SR04.h"
#include "sensorHMMD.h"
#include "sensorLD2410.h"
#include "sensorVL53L1X.h"
#include "udp_lib.h"

// ui in last because some interface are defined in sensor headers
#include "ui.h"

void setup() {
    debug_cc = false;
    debug_sensor = false;
    debug_raw = false;
    debug_midi = false;

    Serial.begin(115200);
    log("Hello. i'm ");
    logln(UID);
    wifi_setup_client();
    udp_setup();
    ui_setup();
    printWiFiStatus();
    leds_setup();
    sensor_setup();
}

void loop() {
    check_time_sync();

    leds_show();
    wifi_client_loop();
    if (udp_connected.get() == true && midi_enable.get() == true)
        sensor_loop();
    ui_loop();
}
