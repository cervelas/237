#if SENSOR == SENSOR_HCSR04

const int trig_pin = 12;
const int echo_pin = 9;

uint16_t readDistance() {
    // delay between readings
    delay(60);

    digitalWrite(trig_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(trig_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig_pin, LOW);

    unsigned long duration = pulseIn(echo_pin, HIGH, 30000);

    // Error checking for timeouts or invalid readings
    if (duration == 0) {
        if (debug_raw)
            Serial.println("HC-SR04: Timeout or invalid reading");
        return 0;
    }

    float distance_cm = duration * 0.0343 / 2.0;
    // HC-SR04 range: ~2–400 cm
    distance_cm = constrain(distance_cm, 2.0, 400.0);

    return static_cast<uint16_t>(distance_cm);
}

void sensor_setup() {
    pinMode(trig_pin, OUTPUT);

    pinMode(echo_pin, INPUT_PULLUP);

    Serial.println(F("Setup Done."));
}

void sensor_loop() {
    uint16_t dist = readDistance();
    dist = filter_dist(dist);

    if (debug_raw)
        Serial.println("dist: " + String(dist));

    send_note_from_dist(dist);
}

#endif
