# 237
237 Project

## Installation
This arduino project need thoses libs so far 

- ArduinoHttpServer by Sander van Woensel <sander@vanwoensel.me> Version 0.10.0 
- EEPROM-Storage by Daniel Porrey Version 2.0.1
- MyLD2410 by Iavor Veltchev Version 1.2.3 
- LED_Matrix Built-In by Arduino Version 1.1.0 

## Architecture

- `237.ino` is the entrypoint, define ROM Storage and global vars.
- `midi.h` setup and loop of MIDI part.
- `network.h` setup and loop of WiFI part.
- `ui.h` setup and loop of web server, along with templates fucntion and post/get handlers.
- `sensor***.h` define specific sensor setup and loop functions.

## Sensor selection

On top of 237.ino you can choose whatever sensor support will be included by defining the SENSOR compilation var.

## Sensors Support
- 9960
- HMMD
- LD2410
- HCSR04
- VL53L1X (Sensor used in the final product)

## Development Setup PC

- Arduino Uno R4 Wifi
- Supported Sensor
- Wifi Network
- Client PC with Python

## Configuration

Copy the `config.h.example` file, select compilation parameters and upload to Arduino.

```bash
cp config.h.example config.h
```

Parameters:

- WiFi SSID and Password
- Sensor selection (Use the corresponding sensor as the `SENSOR` flag)
