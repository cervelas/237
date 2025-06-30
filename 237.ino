#include <EEPROM-Storage.h>

#include "config.h"
#include "utils.h"

#define NB_CCS 4

bool sensor_enabled = true;

// WiFi related variables
char client_ssid[] = WIFI_SSID;
char client_pass[] = WIFI_PASS;
bool wifi_connected = false;

// Storage for UI and midi
EEPROMStorage<uint8_t> midi_channel(0, 3);
EEPROMStorage<uint8_t> UID(midi_channel.nextAddress(), 3);

EEPROMStorage<bool> debug_cc(UID.nextAddress(), true);
EEPROMStorage<bool> debug_sensor(debug_cc.nextAddress(), false);
EEPROMStorage<bool> debug_raw(debug_sensor.nextAddress(), false);
EEPROMStorage<bool> debug_midi(debug_raw.nextAddress(), false);

EEPROMStorage<uint16_t> note_min(debug_midi.nextAddress(), 2);
EEPROMStorage<uint16_t> note_max(note_min.nextAddress(), 150);

typedef struct {
    uint16_t min;
    uint16_t max;
    uint8_t note;
} Note, *pnotes;

typedef struct {
    uint8_t cc;
    uint16_t smin;
    uint16_t smax;
    uint8_t tmin;
    uint8_t tmax;
    bool enabled;
} CC;

#define NB_NOTES 10

EEPROMStorage<Note> note1(note_max.nextAddress(), {0, 0, 0});
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

EEPROMStorage<CC> cc1(note10.nextAddress(), {1, 2, 150, 0, 127, false});
EEPROMStorage<CC> cc2(cc1.nextAddress(), {2, 2, 150, 0, 127, false});
EEPROMStorage<CC> cc3(cc2.nextAddress(), {3, 2, 150, 0, 127, false});
EEPROMStorage<CC> cc4(cc3.nextAddress(), {4, 2, 150, 0, 127, false});

EEPROMStorage<CC> ccs[NB_CCS] = {cc1, cc2, cc3, cc4};

// Midi global var
EEPROMStorage<bool> udp_connected(cc4.nextAddress(), false);
EEPROMStorage<uint8_t> midi_remote_id(udp_connected.nextAddress(), 33);
// EEPROMStorage<uint8_t> midi_channel(midi_remote_id.nextAddress(), _CHANNEL);
EEPROMStorage<bool> midi_enable(midi_remote_id.nextAddress(), true);

EEPROMStorage<Note> note_far(midi_enable.nextAddress(), {0, 0, 0});
EEPROMStorage<Note> note_near(note_far.nextAddress(), {0, 0, 0});

String status = "";  // used in ui.h
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

    pinMode(alim5V, OUTPUT);
    digitalWrite(alim5V, LOW);
    digitalWrite(alim5V, HIGH);

    Serial.begin(115200);
    log("Hello. i'm ");
    logln(UID);

    // Setup LED Matrix
    leds_setup();

    // Wifi Setup Phase
    leds_wifi();
    wifi_setup_client();
    udp_setup();
    ui_setup();
    printWiFiStatus();

    // Sensor Setup Phase
    leds_sensor();
    sensor_setup();

    set_matrix_text("ID" + String(UID) + " CH" + String(midi_channel));
}

void loop() {
    if (millis() < 120000)
        leds_show();
    wifi_client_loop();
    if (udp_connected.get() == true && midi_enable.get() == true)
        sensor_loop();
    ui_loop();
}
