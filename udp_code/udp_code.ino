#include "udp_lib.h"

#define SIZE 255
char message[] = "Hello from Arduino R4!";

void setup() {
  Serial.begin(9600);
  while (!Serial);  // Wait for Serial Monitor

  if (!UdpLib::begin_udp()) {
    while (true);
  }

  Serial.println("UDP ready. Sending packets...");
}

void loop() {
#ifdef WRITE
    write();
#else
    read();
#endif
}

void read() {
    char buffer[SIZE];

    if(UdpLib::read(buffer, SIZE)) {
        Serial.println("UDP packet recieved");
        Serial.println(buffer);
    }

    delay(1);
}

void write() {
  UdpLib::write(message);

  delay(1000);
}
