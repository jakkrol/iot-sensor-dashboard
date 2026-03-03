#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

const char index_html[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Panel ESP32</title>
    <style>
        body { font-family: Arial; text-align: center; background-color: #1a1a1a; color: white; padding: 50px; }
        .card { background: #2d2d2d; padding: 20px; border-radius: 15px; box-shadow: 0 4px 8px rgba(0,0,0,0.5); display: inline-block; }
        h1 { color: #00d1ff; }
        .data { font-size: 2rem; font-weight: bold; margin: 20px 0; }
        .btn { background: #ff4c4c; color: white; border: none; padding: 10px 20px; border-radius: 5px; cursor: pointer; }
        .btn:hover { background: #ff1a1a; }
    </style>
</head>
<body>
    <div class="card">
        <h1>Panel kontroli</h1>
        <div class="data">Ruch: <span id="status">OK</span></div>
        <div class="data">Temp: <span id="temp">--</span> &deg;C</div>
        <button class="btn" onclick="alert('Alarm wyłączony!')">RESET ALARMU</button>
    </div>

    <script>
        // Funkcja do pobierania danych z ESP32 bez przeładowania strony
        setInterval(function() {
            fetch('/pobierz-dane').then(res => res.json()).then(data => {
                document.getElementById('temp').innerText = data.temperatura;
                document.getElementById('status').innerText = data.stan;
            });
        }, 2000); // Odświeżaj co 2 sekundy
    </script>
</body>
</html>
)=====";

AsyncWebServer server(80);
DNSServer dnsServer;



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

String temperature = "--";
String humidity = "--";

void setup() {
  Serial.begin(115200);                      // Debug via USB
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); // Arduino comms
 
  Serial2.println("ESP ready!");             // Initial handshake

// --- KONFIGURACJA CAPTIVE PORTAL ---
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
  WiFi.softAP("ESP32", "haslo123");

  // Przekieruj wszystkie zapytania DNS (*) na IP ESP32
  dnsServer.start(53, "*", WiFi.softAPIP());

  // Strona główna
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/html", index_html);
  });

  // DODANO: Specjalna obsługa dla systemów Android/iOS (szukają one stron typu /generate_204)
  server.onNotFound([](AsyncWebServerRequest *request){
      request->send(200, "text/html", index_html);
  });

  // Endpoint danych
  server.on("/pobierz-dane", HTTP_GET, [](AsyncWebServerRequest *request){
      //float t = random(200, 250) / 10.0; 
      String json = "{\"temperatura\":" + temperature + ", \"stan\":\"BRAK\"}";
      request->send(200, "application/json", json);
  });

  server.begin();
  Serial.println("System gotowy!");
//   // Connect to Wi-Fi
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);
//   Serial.print("Connecting to WiFi");
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\nWiFi connected");

//   // Set MQTT server
//   client.setServer(mqtt_server, 1883);
//   Serial.println(WiFi.localIP());
// }

// // Reconnect to MQTT broker if disconnected
// void reconnect() {
//   while (!client.connected()) {
//     Serial.print("Attempting MQTT connection...");
//     if (client.connect("ESP32_Unique_ID_99")) {  // Remove username/password for anonymous
//       Serial.println("connected");
//     } else {
//       Serial.print("failed, rc=");
//       Serial.print(client.state());
//       Serial.println(" retrying in 5 seconds");
//       Serial.println(WiFi.localIP());
//       delay(5000);
//     }
//   }
  }

unsigned long lastMsg = 0;

void loop() {
  dnsServer.processNextRequest(); // Obsługa DNS dla captive portal
  // Keep MQTT connection alive
  // if (!client.connected()) {
  //   reconnect();
  // }
  // client.loop();

if (Serial2.available()) {
    String msg = Serial2.readStringUntil('\n'); 
    msg.trim(); 
    Serial.println("From Arduino: " + msg);
    
    if(msg.startsWith("T:")) {
      int tempStart = msg.indexOf("T:") + 2;
      int tempEnd = msg.indexOf(" H:");
      if (tempStart != -1 && tempEnd != -1) {
        temperature = msg.substring(tempStart, tempEnd);
        Serial.println("Updated temperature: " + temperature);
      }
    }


    // Publish to MQTT
    // unsigned long now = millis();
    // if (now - lastMsg > 5000) {
    //   lastMsg = now;
    //     //String msg = "Hello MQTT";
    //     if (client.publish("esp/arduino", msg.c_str())) {
    //       Serial.println("Message sent to MQTT");
    //     } else {
    //       Serial.println("Failed to send message");
    //     }
    // }
    // Odpowiedz z nową linią na końcu
    //Serial2.println("Hello Arduino"); 

    delay(2000);
}

}

