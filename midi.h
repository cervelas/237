#ifndef __MIDI_H
#define __MIDI_H

#include <WiFiS3.h>

#include "ledmatrix.h"
#include "udp_lib.h"

// Values for Median Filtering & Consistency Checking
#define HISTORY_SIZE 3
uint16_t readings[HISTORY_SIZE];
// The index indicates where should we input the current reading that we are
// doing
int index = 0;
// This init value indicates that at least HISTORY_SIZE readings have been done
bool filter_init = false;

uint16_t last_cc[NB_CCS] = {0, 0, 0, 0};
uint16_t last_dist = 0;
int last_note = 0;
bool cc_inrange[NB_CCS] = {false, false, false, false};

// Function to send cc over UDP
void send_cc(int cc, int value, int channel = 1) {
    // if (debug_midi)
    //     Serial.println("Send cc: " + String(cc) + " value: " +
    //     String(value));

    // Construct message byte array as settled in the protocol spec
    // BYTE 0 -> Type of the message (2 for cc)
    // BYTE 1 -> The channel
    // BYTE 2 -> The cc we are affecting
    // BYTE 3 -> The actual value
    byte message[4];
    message[0] = 2;
    message[1] = channel;
    message[2] = cc;
    message[3] = value;
    udp_write(message, sizeof(message));
}

// Function to send note over UDP
void send_note(int note, int vel = 127, int channel = 1) {
    if (debug_midi)
        Serial.println("Send note: " + String(note));

    // Construct message byte array as settled in the protocol spec
    // BYTE 0 -> Type of the message (1 for note)
    // BYTE 1 -> The channel
    // BYTE 2 -> The note
    // BYTE 3 -> The velocity
    byte message[4];
    message[0] = 1;
    message[1] = channel;
    message[2] = note;
    message[3] = vel;
    udp_write(message, sizeof(message));
    // Light builtin led for dev purposes
    digitalWrite(LED_BUILTIN, HIGH);
    delay(5);
    digitalWrite(LED_BUILTIN, LOW);
}

uint16_t filter_dist(uint16_t dist) {
    // Fallback for invalid readings
    if (dist == std::numeric_limits<uint16_t>::max())
        // Map eventual errors or max distances to the max allowed dist
        dist = cc_smax;

    // Insert the current reading in the array, and increment index
    readings[index] = dist;
    index = (index + 1) % HISTORY_SIZE;

    // If filter was not initialized and we reach 0 (so we made the first
    // HISTORY_SIZE readings) the filter is initialized
    if (!filter_init && index == 0)
        filter_init = true;

    // If filter is not initialized, just return the raw distance
    if (!filter_init)
        return dist;

    // Allocate memory to copy the readings array and sort it
    uint16_t sorted[HISTORY_SIZE];
    memcpy(sorted, readings, sizeof(readings));

    for (int i = 0; i < HISTORY_SIZE - 1; i++) {
        for (int j = 0; j < HISTORY_SIZE - i - 1; j++) {
            if (sorted[j] > sorted[j + 1]) {
                long temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }

    // Compute the median
    uint16_t median = sorted[HISTORY_SIZE / 2];

    // If the distance between the new reading and the median is greater than an
    // arbitrary threshold, return the median for smooth response
    if (abs(dist - median) > 10)
        return median;

    // If not return the raw distance
    return dist;
}

void treat_cc(CC cc, uint16_t dist) {
    int id = cc.cc - 1;

    if (!cc.enabled)
        return;

    if (dist > cc.smin && dist < cc.smax) {
        cc_inrange[id] = true;
        uint16_t cc_val = map(dist, cc.smin, cc.smax, cc.tmin, cc.tmax);

        if (udp_connected && cc_val != last_cc[id]) {
            if (debug_cc)
                Serial.println("cc" + String(id) + ": " + String(cc_val));

            send_cc(cc.cc, cc_val, midi_channel);
            last_cc[id] = cc_val;
        }
    } else {
        if (cc_inrange[id]) {
            if (debug_cc)
                Serial.println("cc" + String(id) + ": " + String(cc.tmax));

            send_cc(cc.cc, cc.tmax, midi_channel);
            cc_inrange[id] = false;
        }
    }
}

// Unification function for distance reading, filtering and UDP sending
void send_note_from_dist(uint16_t dist) {
    // Fetch values for near and far note
    // Note near = note_near.get();
    // Note far = note_far.get();
    // CC cc1 = ccs[0].get();
    //
    // if (dist > cc1.smin && dist < cc1.smax) {
    //     cc_inrange = true;
    //     // map the cc
    //     uint16_t cc = map(dist, cc1.smin, cc1.smax, cc1.tmin, cc1.tmax);
    //
    //     // test last cc
    //     if (udp_connected == true && cc != last_cc) {
    //         if (debug_cc)
    //             Serial.println("cc: " + String(cc));
    //         // check notes
    //         for (int i = 0; i < NB_NOTES; i++) {
    //             Note note = notes[i].get();
    //             if (note.note > 0 && (dist >= note.min && dist <= note.max))
    //             {
    //                 if (last_note != note.note) {
    //                     send_note(note.note, 127, midi_channel);
    //                     last_note = note.note;
    //                     break;
    //                 }
    //             }
    //         }
    //
    //         send_cc(cc1.cc, cc, midi_channel);
    //         last_cc = cc;
    //     }
    // } else {
    //     // send cc max once
    //     if (cc_inrange) {
    //         if (debug_cc)
    //             Serial.println("cc: " + String(cc1.tmax));
    //
    //         send_cc(cc1.cc, cc1.tmax, midi_channel);
    //         cc_inrange = false;
    //     }
    //
    //     // Checks for far and near note
    //     if (dist >= far.min && last_note != far.note) {
    //         send_note(far.note, 127, midi_channel);
    //         last_note = far.note;
    //     }
    //     if (dist <= near.max && last_note != near.note) {
    //         send_note(near.note, 127, midi_channel);
    //         last_note = near.note;
    //     }
    // }

    treat_cc(ccs[0].get(), dist);
    treat_cc(ccs[1].get(), dist);
    treat_cc(ccs[2].get(), dist);
    treat_cc(ccs[3].get(), dist);

    last_dist = dist;
}

#endif
