

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
  IPAddress remote = WiFi.localIP();
  remote[3] = midi_remote_id.get();
  Serial.print("trying to connect MIDI Device ");
  Serial.println(remote);
  bool invited = AppleMIDI.sendInvite(remote, DEFAULT_CONTROL_PORT);
  Serial.print("invite sent");
  Serial.println(invited);
}

void midi_setup() {

  moving_avg.begin();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.print("MIdi Setup... ");
  midi_connected = false;

  // Udp.begin(5004)

  // Serial.println("Select and then press the Connect button");
  // Serial.println("Then open a MIDI listener and monitor incoming notes");

  // Stay informed on connection status
  AppleMIDI
      .setHandleConnected(
          [](const APPLEMIDI_NAMESPACE::ssrc_t &ssrc, const char *name) {
            midi_connected = true;
            connecting = false;
            Serial.println("Connected to session");
            Serial.println(ssrc);
            Serial.println(name);

            digitalWrite(LED_BUILTIN, HIGH);
            set_matrix_text("");
          })
      .setHandleDisconnected([](const APPLEMIDI_NAMESPACE::ssrc_t &ssrc) {
        midi_connected = false;
        connecting = false;
        Serial.println("Disconnected");
        Serial.println(ssrc);
        digitalWrite(LED_BUILTIN, LOW);
        set_matrix_text(WiFi.localIP().toString());
      });
  /*
  MIDI.setHandleNoteOn([](byte channel, byte note, byte velocity) {
    Serial.print("NoteOn");
    Serial.print(note);
  });
  MIDI.setHandleNoteOff([](byte channel, byte note, byte velocity) {
    Serial.print("NoteOff");
    Serial.print(note);
  });*/
  MIDI.begin();

  Serial.println("Done.");

  // Serial.println("OK, now make sure you an rtpMIDI session that is Enabled");
  Serial.print("Add device named Arduino with Host ");
  Serial.print(WiFi.localIP());
  Serial.print(" Port ");
  Serial.print(AppleMIDI.getPort());
  Serial.print(" Name ");
  Serial.println(AppleMIDI.getName());

  // midi_start();
}

byte sysex[1] = {0};

void midi_loop() {
  if (wifi_connected) {
    MIDI.read();
    if (midi_connected == false && connecting == false) {
      delay(100);
      if (midi_connected == false && connecting == false) {
        midi_end();
        midi_start();
        delay(100);
      }
    }
    if (millis() - last_sysex > 1000L) {
      // MIDI.sendSysEx(1, sysex);
      last_sysex = millis();
    }
  }

  // Listen to incoming notes
  // MIDI.read();
  // delay(50);

  // send a note every second
  // (dont cáll delay(1000) as it will stall the pipeline)
  // if ((isConnected > 0) && (millis() - t1) > 1000)
  // {
  //   t1 = millis();

  //   byte note = random(1, 127);
  //   byte velocity = 127;
  //   byte channel = 1;

  //   MIDI.sendNoteOn(note, velocity, channel);
  //   delay(50);
  //   MIDI.sendNoteOff(note, velocity, channel);
  // }
}

void send_note(int note, int vel = 127, int channel = 1, int duration = 5) {
  if (debug_midi)
    Serial.println("Send " + String(note));

  char message[4];
  message[0] = 1;
  message[1] = channel;
  message[2] = note;
  message[3] = vel;
  udp_write(message);
  // MIDI.sendNoteOn(note, vel, channel);
  // delay(duration);
  // MIDI.sendNoteOff(note, vel, channel);
}

void send_note_from_dist(uint16_t dist) {
  if (debug_raw)
    Serial.println("dist: " + String(dist));

  Note near = note_near.get();
  Note far = note_far.get();

  if (dist <= near.max) {
    if (last_note != near.note) {
      send_note(near.note, 127, midi_channel);
      last_note = near.note;
    }
  } else

      if (dist >= far.min) {
    if (last_note != far.note) {
      send_note(far.note, 127, midi_channel);
      last_note = far.note;
    }
  } else

    if (dist > cc_smin && dist < cc_smax) {
      uint16_t avg = moving_avg.reading(dist);
    if (debug_sensor)
      Serial.println("avg: " + String(avg));

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
        if (note.note > 0 && (cc > note.min && cc < note.max)) {
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

      // Serial.println(dist);
      // Serial.println(dist);
      // Serial.println(cc);
      // delay(20);
      // MIDI.sendControlChange(1, cc, midi_channel);
      // MIDI.sendNoteOn(note, 127, 1);
      // delay(50);

      // MIDI.sendNoteOff(note, 127, 1);
      // delay(20);
      last_cc = cc;
    }
  }
}

#endif
