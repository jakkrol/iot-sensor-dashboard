#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DHT.h>

#define RX 10 // Arduino pin connected to LV2 (ESP TX)
#define TX 11 // Arduino pin connected to LV1 (ESP RX)

#define DHT11PIN 7
#define DHTTYPE DHT11
DHT dht(DHT11PIN, DHTTYPE);


#define MoveSensor 4
#define DiodePin 6

SoftwareSerial espSerial(RX, TX); // RX, TX

void setup() {
  Serial.begin(115200);       // Serial monitor
  espSerial.begin(9600);  // ESP default baud rate
  dht.begin();

  pinMode(MoveSensor, INPUT);
  pinMode(DiodePin, OUTPUT);
  Serial.println("Arduino ready!");
}

unsigned long lastTempRequest = 0;
const unsigned long interval = 5000; 

void loop() {
  
  int state = digitalRead(MoveSensor);
  //Serial.print("Sensor state: ");
  //Serial.println(state); // Zobaczysz 0 lub 1
  if (state == HIGH) {
    digitalWrite(DiodePin, HIGH); // Turn on the diode
    Serial.println("Movement detected!");
  }else{
    digitalWrite(DiodePin, LOW); // Turn off the diode
    Serial.println("No movement.");
  }
  // Read message from ESP
  // if (espSerial.available()) {
  //   String msg = espSerial.readStringUntil('\n');
  //   Serial.println("From ESP: " + msg);
  // }

  if (millis() - lastTempRequest >= interval) {
    float t = dht.readTemperature(); //
    float h = dht.readHumidity(); //
    
    String payload = "T: " + String(t) + " H: " + String(h);
    espSerial.println(payload); // Wysyłka do ESP
    
    lastTempRequest = millis(); 
  }
  // Serial.print("Temperature: ");
  // Serial.print(t);
  // Serial.print(" Humidity: ");
  // Serial.println(h);

  

 
  
  //delay(2000);
}
