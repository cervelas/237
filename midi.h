

#ifndef __MIDI_H
  #define __MIDI_H


//#define ONE_PARTICIPANT
//#define NO_SESSION_NAME

//#define APPLEMIDI_INITIATOR

#include <WiFiS3.h>
#define USE_EXT_CALLBACKS
#include <AppleMIDI_Debug.h>
#include <AppleMIDI.h>
#include "ledmatrix.h"


uint16_t last_cc = 0;
int last_note = 0;
int last_last_note = 0;
movingAvg moving_avg(3);

String name = "A" + String(_UID);

APPLEMIDI_CREATE_INSTANCE(WiFiUDP, MIDI, "237", 5004);

void OnAppleMidiException(const APPLEMIDI_NAMESPACE::ssrc_t&, const APPLEMIDI_NAMESPACE::Exception&, const int32_t);


unsigned long t1 = millis();

bool connecting = false;
unsigned long last_sysex = 0;

void midi_end(){
  Serial.println("End session.");
  AppleMIDI.sendEndSession();
}

void midi_start(){
  connecting = true;
  IPAddress remote = WiFi.localIP();
  remote[3] = midi_remote_id.get();
  
  Serial.print("trying to connect MIDI Device ");
  Serial.println(remote);
  MIDI.begin();
  //bool invited = AppleMIDI.sendInvite(remote, DEFAULT_CONTROL_PORT);
  //Serial.print("invite sent");
  //Serial.println(invited);
}

void midi_setup()
{

  moving_avg.begin();

  AppleMIDI.setHandleException(OnAppleMidiException);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.print("MIdi Setup... ");
  midi_connected = false;

  IPAddress remote = WiFi.localIP();
  remote[3] = midi_remote_id.get();
  //AppleMIDI.directory.push_back(remote);
  //AppleMIDI.whoCanConnectToMe = APPLEMIDI_NAMESPACE::Anyone;

  //Udp.begin(5004)

  // Serial.println("Select and then press the Connect button");
  // Serial.println("Then open a MIDI listener and monitor incoming notes");


  // Stay informed on connection status
  AppleMIDI
  .setHandleConnected([](const APPLEMIDI_NAMESPACE::ssrc_t & ssrc, const char* name) {
    midi_connected = true;
    connecting = false;
    Serial.println("Connected to session");
    Serial.println(ssrc);
    Serial.println(name);
    
    digitalWrite(LED_BUILTIN, HIGH);
    set_matrix_text("");
  })
  .setHandleDisconnected([](const APPLEMIDI_NAMESPACE::ssrc_t & ssrc) {
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
  
  //midi_start();
}


byte sysex[1] = { 0 };

void midi_loop()
{
  if(wifi_connected){
    MIDI.read();
    if(midi_connected == false && connecting == false){
      delay(100);
      if(midi_connected == false && connecting == false){
        midi_end();
        midi_start(); 
        delay(100);
      }
    }
    if(millis() - last_sysex > 1000L){
      //MIDI.sendSysEx(1, sysex);
      last_sysex = millis();
    }
  }

  // Listen to incoming notes
  //MIDI.read();
  //delay(50);

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

void send_note(int note, int vel = 127, int channel = 1, int duration = 5){
  if(debug_midi) Serial.println("Send " + String(note));
  MIDI.sendNoteOn(note, vel, channel);
  //delay(duration);
  //MIDI.sendNoteOff(note, vel, channel);
}


void send_note_from_dist(uint16_t dist){
  if(debug_raw) Serial.println("dist: " + String(dist));
  
  // Note near = note_near.get();
  // Note far = note_far.get();

  // if(dist <= near.max){
  //   if(last_note != near.note){
  //     send_note(near.note, 127, midi_channel);
  //     last_note = near.note;
  //   }
  // }else

  // if(dist >= far.min){
  //   if(last_note != far.note){
  //     send_note(far.note, 127, midi_channel);
  //     last_note = far.note;
  //   }
  // }else
  
  if(dist > cc_smin && dist < cc_smax){
    uint16_t avg = moving_avg.reading(dist);
    if(debug_sensor) Serial.println("avg: " + String(avg));

    // map the cc
    uint16_t cc = map(avg, cc_smin, cc_smax, cc_tmin, cc_tmax);

    // test last cc
    if(midi_connected == true && cc != last_cc){
      if(debug_cc) Serial.println("cc: " + String(cc));
      bool hit = false;
      // check notes
      for(int i = 0; i < NB_NOTES; i++){
        Note note = notes[i].get();
        if(note.note > 0 && (cc > note.min && cc < note.max)){
          hit = true;
          if(last_note != note.note){
            send_note(note.note, 127, midi_channel);
            last_note = note.note;
            break;
          }
        }
      }
      if(hit == false)
        last_note = 0;

      MIDI.sendControlChange(1, cc, midi_channel);
      last_cc = cc;
    }
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OnAppleMidiException(const APPLEMIDI_NAMESPACE::ssrc_t& ssrc, const APPLEMIDI_NAMESPACE::Exception& e, const int32_t value ) {
  switch (e)
  {
    case APPLEMIDI_NAMESPACE::Exception::BufferFullException:
      Serial.println(F("*** BufferFullException"));
      break;
    case APPLEMIDI_NAMESPACE::Exception::ParseException:
      Serial.println(F("*** ParseException"));
      break;
    case APPLEMIDI_NAMESPACE::Exception::TooManyParticipantsException:
      Serial.println(F("*** TooManyParticipantsException"));
      break;
    case APPLEMIDI_NAMESPACE::Exception::UnexpectedInviteException:
      Serial.println(F("*** UnexpectedInviteException"));
      break;
    case APPLEMIDI_NAMESPACE::Exception::ParticipantNotFoundException:
      Serial.println(F("*** ParticipantNotFoundException"));
      break;
    case APPLEMIDI_NAMESPACE::Exception::ComputerNotInDirectory:
      Serial.println(F("*** ComputerNotInDirectory"));
      break;
    case APPLEMIDI_NAMESPACE::Exception::NotAcceptingAnyone:
      Serial.println(F("*** NotAcceptingAnyone"));
      break;
    case APPLEMIDI_NAMESPACE::Exception::ListenerTimeOutException:
      Serial.println(F("*** ListenerTimeOutException"));
      break;
    case APPLEMIDI_NAMESPACE::Exception::MaxAttemptsException:
      Serial.println(F("*** MaxAttemptsException"));
      break;
    case APPLEMIDI_NAMESPACE::Exception::NoResponseFromConnectionRequestException:
      Serial.println(F("***:yyy did't respond to the connection request. Check the address and port, and any firewall or router settings. (time)"));
      break;
    case APPLEMIDI_NAMESPACE::Exception::SendPacketsDropped:
      Serial.println(F("*** SendPacketsDropped"));
      break;
    case APPLEMIDI_NAMESPACE::Exception::ReceivedPacketsDropped:
      Serial.println(F("*** ReceivedPacketsDropped"));
      break;
    case APPLEMIDI_NAMESPACE::Exception::UdpBeginPacketFailed:
      Serial.println(F("*** UdpBeginPacketFailed"));
      break;
  }
}

#endif