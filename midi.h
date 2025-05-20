#ifndef __MIDI_H
#define __MIDI_H

// #define ONE_PARTICIPANT
// #define NO_SESSION_NAME

#define APPLEMIDI_INITIATOR

#include "ledmatrix.h"
#include "udp_lib.h"
#include <AppleMIDI.h>
#include <AppleMIDI_Debug.h>
#include <WiFiS3.h>

uint16_t last_cc = 0;
int last_note = 0;
int last_last_note = 0;
movingAvg moving_avg(3);

bool cc_inrange = false;

String name = "A" + String(_UID);

APPLEMIDI_CREATE_INSTANCE(WiFiUDP, MIDI, name.c_str(), DEFAULT_CONTROL_PORT);

unsigned long t1 = millis();

bool connecting = false;
unsigned long last_sysex = 0;

void midi_end() {
  Serial.println("End session.");
  AppleMIDI.sendEndSession();
}

void midi_start() {
  connecting = true;
  Serial.print("trying to connect MIDI Device ");
  Serial.println(remote);
  bool invited = AppleMIDI.sendInvite(remote, DEFAULT_CONTROL_PORT);
  Serial.print("invite sent");
  Serial.println(invited);
}

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

void send_note_from_dist(uint16_t dist) {
  if (debug_raw)
    Serial.println("dist: " + String(dist));

  Note near = note_near.get();
  Note far = note_far.get();

  if (dist <= near.max && near.note != 0) {
    cc_inrange = true;
    if (last_note != near.note) {
      send_note(near.note, 127, midi_channel);
      last_note = near.note;
    }
  } else if (dist >= far.min && far.note != 0) {
    cc_inrange = true;
    if (last_note != far.note) {
      send_note(far.note, 127, midi_channel);
      last_note = far.note;
    }
  } else if (dist > cc_smin && dist < cc_smax) {
    cc_inrange = true;
    // map the cc
    uint16_t cc = map(dist, cc_smin, cc_smax, cc_tmin, cc_tmax);

    // test last cc
    if (midi_connected == true && cc != last_cc) {
      if (debug_cc)
        Serial.println("cc: " + String(cc));
      bool hit = false;
      // check notes
      for (int i = 0; i < NB_NOTES; i++) {
        Note note = notes[i].get();
        if (note.note > 0 && (dist > note.min && dist < note.max)) {
          hit = true;
          if (last_note != note.note) {
            if (last_last_note != note.note) {
              send_note(note.note, 127, midi_channel);
              last_last_note = note.note;
              break;
            }
            last_note = note.note;
          }
        }
      }
      if (hit == false)
        last_note = 0;

      send_cc(1, cc, midi_channel);
      last_cc = cc;
    }
  }
  // else {
  //   if (cc_inrange) {
  //     send_cc(1, cc_tmax, midi_channel);
  //     cc_inrange = false;
  //   }
  // }
}

#endif
