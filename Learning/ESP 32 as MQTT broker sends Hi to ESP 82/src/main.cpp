#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Aayush";
const char* password = "12345678";
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* topic = "esp/chat";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, mqtt_port);

  // Connect to MQTT broker
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32_Client_1")) {
      Serial.println("connected!");
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }

  delay(3000); // Give receiver time to subscribe
}

void loop() {
  if (!client.connected()) {
    while (!client.connected()) {
      Serial.print("Reconnecting to MQTT...");
      if (client.connect("ESP32_Client_1")) {
        Serial.println("connected!");
      } else {
        Serial.print("failed, rc=");
        Serial.println(client.state());
        delay(2000);
      }
    }
  }

  client.loop();

  // Publish "Hi" every 2 seconds
  client.publish(topic, "Hi",true);
  Serial.println("Message sent: Hi");
  delay(2000);
}
