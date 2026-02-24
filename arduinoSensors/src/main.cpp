#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DHT.h>

#define RX 10 // Arduino pin connected to LV2 (ESP TX)
#define TX 11 // Arduino pin connected to LV1 (ESP RX)

#define DHT11PIN 7
#define DHTTYPE DHT11
DHT dht(DHT11PIN, DHTTYPE);


SoftwareSerial espSerial(RX, TX); // RX, TX

void setup() {
  Serial.begin(115200);       // Serial monitor
  espSerial.begin(9600);  // ESP default baud rate
  dht.begin();
  Serial.println("Arduino ready!");
}

void loop() {
  
  
  // Read message from ESP
  // if (espSerial.available()) {
  //   String msg = espSerial.readStringUntil('\n');
  //   Serial.println("From ESP: " + msg);
  // }
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" Humidity: ");
  Serial.println(h);

  String payload = "Temperature: " + String(t) + " Humidity: " + String(h);

  // Send test message to ESP
  espSerial.println(payload);
  
  delay(2000);
}
