#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "ESP32_BROKER";
const char* password = "12345678";
const char* mqtt_server = "192.168.4.1";  // ESP32's AP IP (broker)

WiFiClient espClient;
PubSubClient client(espClient);

#define RELAY_PIN D1

void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) msg += (char)payload[i];
  Serial.printf("Fan1 received: %s\n", msg.c_str());

  if (msg == "ON") digitalWrite(RELAY_PIN, HIGH);
  else if (msg == "OFF") digitalWrite(RELAY_PIN, LOW);
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  while (!client.connected()) {
    client.connect("Fan1");
  }

  client.subscribe("home/fan1");
}

void loop() {
  client.loop();
}
