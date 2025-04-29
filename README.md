# 237
237 Project

# Installation
This arduino project need thoses libs so far 

- AppleMIDI by lathoub Version 3.2.0
- ArduinoHttpServer by Sander van Woensel <sander@vanwoensel.me> Version 0.10.0 
- movingAvg by Jack Christensen <jack.christensen@outlook.com> Version 2.3.2  
- EEPROM-Storage by Daniel Porrey Version 2.0.1
- MyLD2410 by Iavor Veltchev Version 1.2.3 
- LED_Matrix Built-In by Arduino Version 1.1.0 

# Architecture

- `237.ino` is the entrypoint, define ROM Storage and global vars.
- `midi.h` setup and loop of MIDI part.
- `network.h` setup and loop of WiFI part.
- `ui.h` setup and loop of web server, along with templates fucntion and post/get handlers.
- `sensor***.h` define specific sensor setup and loop functions.

## Sensor selection

On top of 237.ino you can choose whatever sensor support will be included by defining the SENSOR compilation var.

# Sensors Support
- 9960
- HMMD

# Development Setup PC

- Arduino Uno R4 Wifi
- Supported Sensor
- Wifi Network
- Client PC with rtpMidi software and Midiview Software

## rtpMidi Setup
- Create a Session with a name
- Create a remote Peer with the arduino ip address
- Enable the session and connect the arduino