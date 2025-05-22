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

// Running average window size
#define LM_SIZE 10

uint16_t last_cc = 0;
uint16_t last_dist = 0;
int last_note = 0;
int last_last_note = 0;
bool cc_inrange = false;

// the following stuff need to be updated via the UI.

int max_idle_far_counter = 40;
int max_idle_min_counter = 40;

// ignore completely what is above this. [not used]
int ultimax_dist = 1000;


String name = "A" + String(_UID);

APPLEMIDI_CREATE_INSTANCE(WiFiUDP, MIDI, name.c_str(), DEFAULT_CONTROL_PORT);

unsigned long t1 = millis();

bool connecting = false;
unsigned long last_sysex = 0;
int idle_counter = 0;

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


//do some smoothing https://playground.arduino.cc/Main/RunningAverage/

// Hola liviu, the following (shitty) code is not working for some reasons.
// please do something better !

uint16_t runningAverage(int M) { 
  static uint16_t LM[LM_SIZE];      // LastMeasurements
  static byte index = 0;
  static uint16_t sum = 0;
  static byte count = 0;

  // keep sum updated to improve speed.
  sum -= LM[index];
  LM[index] = M;
  sum += LM[index];
  index++;
  index = index % LM_SIZE;
  if (count < LM_SIZE) count++;

  return sum / count;
}

void send_note_from_dist(uint16_t dist) {

  if (debug_raw)
    Serial.println("dist: " + String(dist));

  // this is not working at all....
  //uint16_t dist = runningAverage(_dist);

  Note near = note_near.get();
  Note far = note_far.get();

  if (dist > cc_smin && dist < cc_smax) {

    // we also need to reset the idle counter here !
    idle_counter = 0;
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
        if (note.note > 0 && (dist >= note.min && dist <= note.max)) {
          hit = true;
          if (last_note != note.note) {
            // if (last_last_note != note.note) {
            send_note(note.note, 127, midi_channel);
            // last_last_note = note.note;
            last_note = note.note;
            break;
            // }
          }
        }
      }
      // if (hit == false)
      //   last_note = 0;

      send_cc(1, cc, midi_channel);
      last_cc = cc;
    }
  } else {
    // send cc max once
    if (cc_inrange) {
      send_cc(1, cc_tmax, midi_channel);
      cc_inrange = false;
    }

    // check for near note
    if (dist <= near.max && near.note != 0) {
      if (last_note != near.note) {
        if (idle_counter++ > max_idle_min_counter) {
          send_note(near.note, 127, midi_channel);
          last_note = near.note;
          idle_counter = 0;
        }
      }
    }

    // check for far note

    // note for liviu: the idle_counter does not seems to work... 
    if (dist >= far.min && far.note != 0) {
      if (last_note != far.note) {
        if (idle_counter++ > max_idle_far_counter) {
          send_note(far.note, 127, midi_channel);
          last_note = far.note;
          idle_counter = 0;
        }
      }
    }

  }

  last_dist = dist;
}

#endif
