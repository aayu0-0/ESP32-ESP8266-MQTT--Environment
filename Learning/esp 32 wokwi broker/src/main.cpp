#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";
WiFiServer server(1883); // MQTT broker port

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP32_BROKER", "12345678");

  IPAddress ip = WiFi.softAPIP();
  Serial.print("ESP32 MQTT Broker running at: ");
  Serial.println(ip);

  // Broker setup message (for simulation)
  Serial.println("MQTT Broker Ready...");
}

void loop() {
  // Simulation: send commands every few seconds
  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 5000) {
    lastMsg = millis();
    Serial.println("[CMD] FAN1 ON");
    delay(500);
    Serial.println("[CMD] FAN2 OFF");
  }
}
