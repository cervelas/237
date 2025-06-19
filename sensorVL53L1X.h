#if SENSOR == SENSOR_VL53L1X

#include <Wire.h>

#include "VL53L1X.h"

VL53L1X sensor;

uint16_t readDistance() {
    int dist = sensor.read() / 10;

    if (sensor.ranging_data.range_status)
        // dist = 0;
        dist = std::numeric_limits<uint16_t>::max();

    return static_cast<uint16_t>(dist);
}

void sensor_setup() {
    Wire.begin();
    Wire.setClock(400000);  // use 400 kHz I2C

    sensor.setTimeout(500);
    if (!sensor.init()) {
        Serial.println("Failed to initialize VL53L1X Distance_Sensor!");
        while (1)
            ;
    }

    // Use long distance mode and allow up to 50000 us (50 ms) for a
    // measurement. You can change these settings to adjust the performance
    // of the sensor, but the minimum timing budget is 33 ms for medium and
    // long distance mode
    sensor.setDistanceMode(VL53L1X::Medium);
    sensor.setMeasurementTimingBudget(50000);

    // Start continuous readings at a rate of one measurement every 50 ms (the
    // inter-measurement period). This period should be at least as long as the
    // timing budget.
    sensor.startContinuous(50);
}

void sensor_loop() {
    uint16_t dist = readDistance();
    // dist = filter_dist(dist);
    if (debug_raw)
        Serial.println("dist: " + String(dist));

    send_note_from_dist(dist);
}

#endif
