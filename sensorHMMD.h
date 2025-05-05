/*

HMMD Sensor
- WaveShare

Wiki : https://www.waveshare.com/wiki/HMMD_mmWave_Sensor#Introduction

Protocol is shit and undocumented, only infos is :

  Read Firmware Version Command 
  FD FC FB FA 	02 00 	00 00 	04 03 02 01 

  Read Serial Number Command
  FD FC FB FA 	02 00 	11 00 	04 03 02 01 

  MOdule report Data 
  Normal : FD FC FB FA 	08 00 	12 00 	00 00 	64 00 00 00 	04 03 02 01 
  Debug:   FD FC FB FA 	08 00 	12 00 	00 00 	00 00 00 00 	04 03 02 01 

*/

/*
#include "s3km1110.h"

s3km1110 radar;
uint32_t lastReading = 0;

void sensor_setup(void) {
    Serial1.begin(115200);

    bool isRadarEnabled = radar.begin(Serial1, Serial);
    Serial.print("Radar status: ");
    Serial.println(isRadarEnabled ? "Ok" : "Failed");

    if (radar.readAllRadarConfigs()) {
        auto config = radar.radarConfiguration;
        Serial.println("Super");
        Serial.println(*config->detectionGatesMin);
        Serial.println(*config->detectionGatesMax);
        Serial.println(*config->activeFrameNum);
        Serial.println(*config->delay);
        //Serial.println("[Info] Radar config:\n |- Gates  | Min: %u\t| Max: %u\n |- Frames | Detect: %u\t| Disappear: %u\n |- Disappearance delay: %u\n",
        //            config->detectionGatesMin, config->detectionGatesMax, config->activeFrameNum, config->inactiveFrameNum, config->delay);
    }
}

void sensor_loop() {
    if (radar.isConnected()) {
        lastReading = millis();
        while (millis() - lastReading < 2000) {
            if (radar.read()) {
                // Get radar info
                bool isDetected = radar.isTargetDetected;
                int16_t targetDistance = radar.distanceToTarget;
            }
        }
    }
}
*/

#if SENSOR == SENSOR_HMMD


uint8_t Cache[23] = {0};    //Cache


void printdf(uint8_t *buf, int len)
{
  for (int i = 0; i < len; i++) {
    if (buf[i] < 0x10) {
      Serial.print("0");
    }

    Serial.print(buf[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void sendHexData(String hexString) {
    // Convert hex string to bytes
    int hexStringLength = hexString.length();
    byte hexBytes[hexStringLength / 2];
    for (int i = 0; i < hexStringLength; i += 2) {
      hexBytes[i / 2] = strtoul(hexString.substring(i, i + 2).c_str(), NULL, 16);
    }
    Serial.println(hexString);
    // Send bytes through software serial
    Serial1.write(hexBytes, sizeof(hexBytes));
  }

size_t readN(uint8_t *buf, size_t len)
{
  size_t offset = 0, left = len;
  int16_t timeout = 1500;
  uint8_t  *buffer = buf;
  long curr = millis();
  while (left) {
    if (Serial1.available()) {
      buffer[offset] = Serial1.read();
      offset++;
      left--;
    }
    if (millis() - curr > timeout) {
      break;
    }
  }
  return offset;
}

uint16_t readDistance(uint8_t *buf)
{
  int16_t timeout = 500;
  long curr = millis();
  uint8_t ch;
  bool ret = false;
  uint16_t distance = 0;
  const char *P;
  while (!ret) {
    if (millis() - curr > timeout) {
      break;
    }
    if (readN(&ch, 1) == 1) {
      if (ch == 0xF4) {
        buf[0] = ch;
        if (readN(&ch, 1) == 1) {
          if (ch == 0xF3) {
            buf[1] = ch;
            if (readN(&ch, 1) == 1) {
              if (ch == 0xF2) {
                buf[2] = ch;
                if (readN(&ch, 1) == 1) {
                  if (ch == 0xF1) {
                    buf[3] = ch;
                      if (readN(&buf[4], 35) == 35) {
                        uint16_t length = buf[4] + (buf[5] << 8);
                        // Serial.write(27);       // ESC command
                        // Serial.print("[2J");    // clear screen command
                        // Serial.write(27);
                        // Serial.print("[H");     // cursor to home command
                        // printdf(buf, 10 + buf[4]); //Print raw data
                        // switch (buf[6]) {
                        //     case 0x00 : Serial.println("Detected status: nobody"); break;
                        //     case 0x01 : Serial.println("Detected status: present"); break;
                        //   }
                        distance = buf[7] + (buf[8] << 8);
                        // for (uint8_t idx = 0; idx < 32; idx+=2) {
                        //     uint16_t energy = buf[9 + idx] + (buf[9 + idx] << 8);
                        //     Serial.print("energy ");
                        //     Serial.print(idx);
                        //     Serial.print(" : ");
                        //     Serial.println(energy);

                        // }
                        
//                        Serial.print("Energy value of moving object:");
//                        Serial.println(buf[11]);
//                        Serial.print("Energy value of stationary object:");
//                        Serial.println(buf[14]);
//                        Serial.print("Distance to the moving object in CM:");
//                        Serial.println(Adistance);
//                        Serial.print("Distance to the stationary object in CM:");
//                        Serial.println(Sdistance);
                        // Serial.print("length: ");
                        // Serial.println(length);
                        // Serial.print("distance: ");
                        // Serial.println(distance);
                        break;
                      }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return distance;
}


void sensor_setup() {
    
  Serial.println(F("------------------------------------"));
  Serial.println(F("Human Micro-Motion Detection mmWave "));
  Serial.println(F("------------------------------------"));

  Serial1.begin(115200);


  // MOdule Report Data Normal Mode

  // NORMAL 
  //String hex_to_send = "FDFCFBFA0800120000006400000004030201";

  // REPORT
  String hex_to_send = "FDFCFBFA0800120000000400000004030201";
  sendHexData(hex_to_send);
  // Read and print serial data
  delay(100);

  Serial.println(F("Setup Done."));
}

void sensor_loop() {
  uint16_t dist = readDistance(Cache);
  if(debug_raw) Serial.println("dist: " + String(dist));

  send_note_from_dist(dist);
  //return;
  
  // if(dist <= note_near.get().max){
  //   send_note(note_near.get().note, 127, midi_channel);
  //   return;
  // }

  // if(dist >= note_far.get().min){
  //   send_note(note_far.get().note, 127, midi_channel);
  //   return;
  // }

  // if(dist > cc_smin && dist < cc_smax){
  //   uint16_t avg = moving_avg.reading(dist);
  //   if(debug_sensor) Serial.println("avg: " + String(avg));

  //   // map the cc
  //   uint16_t cc = map(avg, cc_smin, cc_smax, cc_tmin, cc_tmax);

  //   // test last cc
  //   if(midi_connected == true && cc != last_cc){
  //     if(debug_cc) Serial.println("cc: " + String(cc));
  //     bool hit = false;
  //     // check notes
  //     for(int i = 0; i < NB_NOTES; i++){
  //       Note note = notes[i].get();
  //       if(note.note > 0 && (cc > note.min && cc < note.max)){
  //         hit = true;
  //         if(last_note != note.note){
  //           if(last_last_note != note.note){
  //           send_note(note.note, 127, midi_channel);
  //           last_last_note = note.note;
  //           return;
  //           }
  //           last_note = note.note;
  //         }
  //       }
  //     }
  //     if(hit == false)
  //       last_note = 0;

  //     //Serial.println(dist);
  //     //Serial.println(dist);
  //     //Serial.println(cc);
  //     delay(20);
  //     MIDI.sendControlChange(1, cc, midi_channel);
  //     //MIDI.sendNoteOn(note, 127, 1);
  //     //delay(50);

  //     //MIDI.sendNoteOff(note, 127, 1);
  //     //delay(20);
  //     last_cc = cc;
  //   }
  // }
}


#endif