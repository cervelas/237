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

#if SENSOR == SENSOR_LD2410


// #include <ld2410.h>

// ld2410 radar;

#include "MyLD2410.h"

MyLD2410 sensor(Serial1, true);

uint32_t lastReading = 0;
bool radarConnected = false;

unsigned long nextPrint = 0, printEvery = 1000;  // print every second

uint8_t Cache[23] = {0};    //Cache
uint16_t last_cc = 0;
int last_note = 0;
movingAvg moving_avg(3);


void printValue(const byte &val) {
  Serial.print(' ');
  Serial.print(val);
}

void printData() {
  Serial.print(sensor.statusString());
  if (sensor.presenceDetected()) {
    Serial.print(", distance: ");
    Serial.print(sensor.detectedDistance());
    Serial.print("cm");
  }
  Serial.println();
  if (sensor.movingTargetDetected()) {
    Serial.print(" MOVING    = ");
    Serial.print(sensor.movingTargetSignal());
    Serial.print("@");
    Serial.print(sensor.movingTargetDistance());
    Serial.print("cm ");
    if (sensor.inEnhancedMode()) {
      Serial.print("\n signals->[");
      sensor.getMovingSignals().forEach(printValue);
      Serial.print(" ] thresholds:[");
      sensor.getMovingThresholds().forEach(printValue);
      Serial.print(" ]");
    }
    Serial.println();
  }
  if (sensor.stationaryTargetDetected()) {
    Serial.print(" STATIONARY= ");
    Serial.print(sensor.stationaryTargetSignal());
    Serial.print("@");
    Serial.print(sensor.stationaryTargetDistance());
    Serial.print("cm ");
    if (sensor.inEnhancedMode()) {
      Serial.print("\n signals->[");
      sensor.getStationarySignals().forEach(printValue);
      Serial.print(" ] thresholds:[");
      sensor.getStationaryThresholds().forEach(printValue);
      Serial.print(" ]");
    }
    Serial.println();
  }

  if (sensor.inEnhancedMode() && (sensor.getFirmwareMajor() > 1)) { 
    Serial.print("Light level: ");
    Serial.println(sensor.getLightLevel());
    Serial.print("Output level: ");
    Serial.println((sensor.getOutLevel()) ? "HIGH" : "LOW");
  }

  Serial.println();
}

void sensor_setup() {
  Serial.println(F("------------------------------------"));
  Serial.println(F("    mmWave 24Ghz LD2410"));
  Serial.println(F("------------------------------------"));

  Serial1.begin(256000);

  //moving_avg.begin();e

  // MOdule Report Data Normal Mode

  // NORMAL 
  //String hex_to_send = "FDFCFBFA0800120000006400000004030201";

  //delay(2000);

  while (!sensor.begin()) {
    Serial.println("Failed to communicate with the sensor.");
  }

  // REPORT
  // if(radar.begin(Serial1))
  // {
  //   Serial.println(F("OK"));
  //   Serial.print(F("LD2410 firmware version: "));
  //   Serial.print(radar.firmware_major_version);
  //   Serial.print('.');
  //   Serial.print(radar.firmware_minor_version);
  //   Serial.print('.');
  //   Serial.println(radar.firmware_bugfix_version, HEX);
  //   Serial.println(F("Setup Done."));
  // }
  // else
  // {
  //   Serial.println(F("not connected"));
  // }
  // // Read and print serial data
  // delay(100);
  
  Serial.println("Sensor Ready.");

}

void sensor_loop() {
  printData();
  if ((sensor.check() == MyLD2410::Response::DATA) && (millis() > nextPrint)) {
    nextPrint = millis() + printEvery;
    printData();
  }

  // radar.read();
  // if(radar.isConnected() && millis() - lastReading > 1000)  //Report every 1000ms
  // {
  //   lastReading = millis();
  //   if(radar.presenceDetected())
  //   {
  //     if(radar.stationaryTargetDetected())
  //     {
  //       Serial.print(F("Stationary target: "));
  //       Serial.print(radar.stationaryTargetDistance());
  //       Serial.print(F("cm energy:"));
  //       Serial.print(radar.stationaryTargetEnergy());
  //       Serial.print(' ');
  //     }
  //     if(radar.movingTargetDetected())
  //     {
  //       Serial.print(F("Moving target: "));
  //       Serial.print(radar.movingTargetDistance());
  //       Serial.print(F("cm energy:"));
  //       Serial.print(radar.movingTargetEnergy());
  //     }
  //     Serial.println();
  //   }
  //   else
  //   {
  //     Serial.println(F("No target"));
  //   }
  // }
  // // Read and print serial data
  // uint16_t dist = readDistance(Cache);
  // if(debug_raw) Serial.println("dist: " + String(dist));
  // //int note = dist;
  // if(dist > cc_smin && dist < cc_smax){
  //   uint16_t avg = moving_avg.reading(dist);
  //   if(debug_sensor) Serial.println("avg: " + String(avg));
  //   //int note = map(avg, 0, 600, 0, 127);
  //   uint16_t cc = map(avg, cc_smin, cc_smax, cc_tmin, cc_tmax);

  //   if(midi_connected == true && cc != last_cc){
  //     if(debug_cc) Serial.println("cc: " + String(cc));
      
  //     for(int i = 0; i < NB_NOTES; i++){
  //       Note note = notes[i].get();
  //       if(note.note > 0 && (cc > note.min && cc < note.max)){
  //         if(last_note != note.note){
  //           send_note(note.note);
  //           last_note = note.note;
  //         }
  //       }
  //     }

  //     //Serial.println(dist);
  //     //Serial.println(dist);
  //     //Serial.println(cc);
  //     MIDI.sendControlChange(1, cc, 1);
  //     //MIDI.sendNoteOn(note, 127, 1);
  //     //delay(50);

  //     //MIDI.sendNoteOff(note, 127, 1);
  //     //delay(20);
  //     last_cc = cc;
  //   }
  // }
}


#endif