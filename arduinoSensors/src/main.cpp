#include <Arduino.h>
#include <SoftwareSerial.h>

#define RX 10 // Arduino pin connected to LV2 (ESP TX)
#define TX 11 // Arduino pin connected to LV1 (ESP RX)

SoftwareSerial espSerial(RX, TX); // RX, TX

void setup() {
  Serial.begin(115200);       // Serial monitor
  espSerial.begin(9600);  // ESP default baud rate
  Serial.println("Arduino ready!");
}

void loop() {
  // Send test message to ESP
  espSerial.println("Hello ESP!");
  
  // Read message from ESP
  if (espSerial.available()) {
    String msg = espSerial.readStringUntil('\n');
    Serial.println("From ESP: " + msg);
  }
  
  delay(2000);
}
