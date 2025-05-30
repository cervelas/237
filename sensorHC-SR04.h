#if SENSOR == SENSOR_HCSR04

#define MEAN_MEASURES 3

const int trig_pin = 12;
const int echo_pin = 9;

int curr_measure = 0;
uint16_t measures[MEAN_MEASURES];

void bubble_sort(uint16_t* arr, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                uint16_t temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

uint16_t readDistance() {
    delay(40);
    digitalWrite(trig_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(trig_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig_pin, LOW);

    float duration = pulseIn(echo_pin, HIGH);
    float distanceF = duration * 0.0343 / 2;
    uint16_t distance = static_cast<uint16_t>(distanceF);

    if (distance <= 300)
        measures[curr_measure] = distance;

    if (curr_measure >= MEAN_MEASURES)
        curr_measure = 0;
    else
        curr_measure++;

    bubble_sort(measures, MEAN_MEASURES);
    uint16_t result = measures[MEAN_MEASURES / 2];
    return result;
}

void sensor_setup() {
    pinMode(trig_pin, OUTPUT);

    pinMode(echo_pin, INPUT_PULLUP);

    Serial.println(F("Setup Done."));
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
