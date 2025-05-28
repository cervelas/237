#if SENSOR == SENSOR_VL53L1X

#include "VL53L1X.h"

VL53L1X sensor;

uint16_t readDistance() {
  int dist = sensor.readSingle() / 10;
  // Serial.println("dist tof: " + String(dist));
  if (dist < 1)
    dist = 0;
  delay(50);
  return static_cast<uint16_t>(dist);
}

void sensor_setup() {
  // VL53L1X::DistanceMode range_mode = VL53L1X::Short;
  VL53L1X::DistanceMode range_mode = VL53L1X::Medium;
  Wire.begin();
  sensor.setTimeout(500);
  sensor.init();
  sensor.setDistanceMode(range_mode);
  sensor.setMeasurementTimingBudget(140000);
}

void sensor_loop() {
  uint16_t dist = readDistance();
  if (debug_raw)
    Serial.println("dist: " + String(dist));

  send_note_from_dist(dist);
  // if (dist > cc_smin && dist < cc_smax) {
  //   uint16_t avg = moving_avg.reading(dist);
  //   if (debug_sensor)
  //     Serial.println("avg: " + String(avg));
  //
  //   // map the cc
  //   uint16_t cc = map(dist, cc_smin, cc_smax, cc_tmin, cc_tmax);
  //   Serial.println("Ultra dist, cc: " + String(dist) + ", " + String(cc));
  //
  //   // test last cc
  //   if (midi_connected == true && cc != last_cc) {
  //     if (debug_cc)
  //       Serial.println("cc: " + String(cc));
  //     bool hit = false;
  //     // check notes
  //     for (int i = 0; i < NB_NOTES; i++) {
  //       Note note = notes[i].get();
  //       if (note.note > 0 && (cc > note.min && cc < note.max)) {
  //         hit = true;
  //         if (last_note != note.note) {
  //           send_note(note.note, 127, midi_channel);
  //           last_note = note.note;
  //           break;
  //         }
  //       }
  //     }
  //     if (hit == false)
  //       last_note = 0;

  // Serial.println(dist);
  // Serial.println(dist);
  // Serial.println(cc);
  // MIDI.sendControlChange(1, cc, midi_channel);
  // MIDI.sendNoteOn(note, 127, 1);
  // delay(50);

  // MIDI.sendNoteOff(note, 127, 1);
  // delay(20);
  // last_cc = cc;
}

#endif
