#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Serial2 pins for Arduino communication
#define RXD2 16
#define TXD2 17

// Wi-Fi credentials
const char* ssid = ""; //wifi ssid
const char* password = ""; //wifi pass

// MQTT broker IP
const char* mqtt_server = ""; //server ip

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);                      // Debug via USB
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); // Arduino comms
 
  Serial2.println("ESP ready!");             // Initial handshake

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // Set MQTT server
  client.setServer(mqtt_server, 1883);
  Serial.println(WiFi.localIP());
}

// Reconnect to MQTT broker if disconnected
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32_Unique_ID_99")) {  // Remove username/password for anonymous
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      Serial.println(WiFi.localIP());
      delay(5000);
    }
  }
}

unsigned long lastMsg = 0;

void loop() {
  // Keep MQTT connection alive
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

if (Serial2.available()) {
    String msg = Serial2.readStringUntil('\n'); 
    msg.trim(); 
    
    Serial.println("From Arduino: " + msg);
    

    // Publish to MQTT
    unsigned long now = millis();
    if (now - lastMsg > 5000) {
      lastMsg = now;
        //String msg = "Hello MQTT";
        if (client.publish("esp/arduino", msg.c_str())) {
          Serial.println("Message sent to MQTT");
        } else {
          Serial.println("Failed to send message");
        }
    }
    // Odpowiedz z nową linią na końcu
    //Serial2.println("Hello Arduino"); 

    delay(2000);
}

}

