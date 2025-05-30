#ifndef __MIDI_H
#define __MIDI_H

#include <WiFiS3.h>

#include "ledmatrix.h"
#include "udp_lib.h"

uint16_t last_cc = 0;
uint16_t last_dist = 0;
int last_note = 0;
bool cc_inrange = false;

void send_cc(int cc, int value, int channel = 1) {
    if (debug_midi)
        Serial.println("Send cc: " + String(cc) + " value: " + String(value));

    byte message[4];
    message[0] = 2;
    message[1] = channel;
    message[2] = cc;
    message[3] = value;
    udp_write(message, sizeof(message) * 4);
}

void send_note(int note, int vel = 127, int channel = 1) {
    if (debug_midi)
        Serial.println("Send note: " + String(note));

    byte message[4];
    message[0] = 1;
    message[1] = channel;
    message[2] = note;
    message[3] = vel;
    udp_write(message, sizeof(message) * 4);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(5);
    digitalWrite(LED_BUILTIN, LOW);
}

// TODO: implement filtering here
void send_note_from_dist(uint16_t dist) {
    if (debug_raw)
        Serial.println("dist: " + String(dist));

    Note near = note_near.get();
    Note far = note_far.get();

    if (dist > cc_smin && dist < cc_smax) {
        cc_inrange = true;
        // map the cc
        uint16_t cc = map(dist, cc_smin, cc_smax, cc_tmin, cc_tmax);

        // test last cc
        if (udp_connected == true && cc != last_cc) {
            if (debug_cc)
                Serial.println("cc: " + String(cc));
            // check notes
            for (int i = 0; i < NB_NOTES; i++) {
                Note note = notes[i].get();
                if (note.note > 0 && (dist >= note.min && dist <= note.max)) {
                    if (last_note != note.note) {
                        send_note(note.note, 127, midi_channel);
                        last_note = note.note;
                        break;
                    }
                }
            }

            send_cc(1, cc, midi_channel);
            last_cc = cc;
        }
    } else {
        // send cc max once
        if (cc_inrange) {
            send_cc(1, cc_tmax, midi_channel);
            cc_inrange = false;
        }

        // TODO: checks for far and near note
    }

    last_dist = dist;
}

#endif
