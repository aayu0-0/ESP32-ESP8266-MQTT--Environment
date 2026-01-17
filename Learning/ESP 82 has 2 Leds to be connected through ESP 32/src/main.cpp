#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Aayush";
const char* password = "12345678";
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

const int led1 = D0;
const int led2 = D1;

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  message.trim();
  Serial.print("Received: ");
  Serial.println(message);

  if (message == "on1") {
    digitalWrite(led1, HIGH);
    client.publish("/esp/led/confirm", "LED1 ON");
  } 
  else if (message == "off1") {
    digitalWrite(led1, LOW);
    client.publish("/esp/led/confirm", "LED1 OFF");
  } 
  else if (message == "on2") {
    digitalWrite(led2, HIGH);
    client.publish("/esp/led/confirm", "LED2 ON");
  } 
  else if (message == "off2") {
    digitalWrite(led2, LOW);
    client.publish("/esp/led/confirm", "LED2 OFF");
  } 
  else {
    // Send error feedback for unknown command
    String err = "INVALID CMD: " + message;
    client.publish("/esp/led/confirm", err.c_str());
    Serial.println("⚠️ Invalid command received!");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Reconnecting to MQTT...");
    if (client.connect("ESP8266_Client")) {
      Serial.println("connected!");
      client.subscribe("/esp/led");
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  reconnect();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
