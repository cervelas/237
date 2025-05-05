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

size_t readN(uint8_t *buf, size_t len);

bool recdData(uint8_t *buf);

uint8_t Cache[23] = {0};    //Cache

void sensor_setup()
{
  Serial1.begin(57600); //Soft serial port
  //Serial1.begin(256000);
}

void sensor_loop()
{
  recdData(Cache);
}

size_t readN(uint8_t *buf, size_t len)
{
  size_t offset = 0, left = len;
  int16_t Tineout = 1500;
  uint8_t  *buffer = buf;
  long curr = millis();
  while (left) {
    if (Serial1.available()) {
      // buffer[offset] = Serial1.read();
      buffer[offset] = Serial1.read();
      offset++;
      left--;
    }
    if (millis() - curr > Tineout) {
      break;
    }
  }
  return offset;
}

bool recdData(uint8_t *buf)
{
  int16_t Tineout = 50000;
  long curr = millis();
  uint8_t ch;
  bool ret = false;
  const char *P;
  while (!ret) {
    if (millis() - curr > Tineout) {
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
                      if (readN(&buf[4], 19) == 19) {
//                        printdf(buf, 23); //Print raw data
                        uint16_t Adistance = buf[10] << 8 | buf[9];
                        uint16_t Sdistance = buf[13] << 8 | buf[12];
                        uint16_t Distance = buf[16] << 8 | buf[15];
                        // switch (buf[8]) {
                        //   case 0x00 : Serial.println("Detected status: nobody"); break;
                        //   case 0x01 : Serial.println("Detected status: moving"); break;
                        //   case 0x02 : Serial.println("Detected status: stationary"); break;
                        //   case 0x03 : Serial.println("Detected status: moving & stationary object"); break;
                        // }
                      //  Serial.print("Energy value of moving object:");
                      //  Serial.println(buf[11]);
                      //  Serial.print("Energy value of stationary object:");
                      //  Serial.println(buf[14]);
                      //  Serial.print("Distance to the moving object in CM:");
                      //  Serial.println(Adistance);
                      //  Serial.print("Distance to the stationary object in CM:");
                      //  Serial.println(Sdistance);
                      //   Serial.print("Detection distance CM:");
                      //   Serial.println(Distance);
                        send_note_from_dist(Distance);
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
  return ret;
}

#endif