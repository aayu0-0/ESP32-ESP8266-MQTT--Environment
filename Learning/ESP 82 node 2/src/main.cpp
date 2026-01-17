#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Aayush";
const char* password = "12345678";
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// LED pin (built-in LED)
const int ledPin = LED_BUILTIN;

WiFiClient espClient;
PubSubClient client(espClient);

// Topics
const char* node2_topic = "esp/node2";
const char* node2_confirm = "esp/node2/confirm";

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  message.trim();

  Serial.print("Received command: ");
  Serial.println(message);

  if (message == "on") {
    digitalWrite(ledPin, LOW); // On for ESP8266 built-in LED
    client.publish(node2_confirm, "LED ON");
    Serial.println("LED turned ON");
  } else if (message == "off") {
    digitalWrite(ledPin, HIGH); // Off
    client.publish(node2_confirm, "LED OFF");
    Serial.println("LED turned OFF");
  } else if (message == "hi") {
    client.publish(node2_confirm, "Hi received");
    Serial.println("Hi received, confirmation sent");
  } else {
    String err = "INVALID CMD: " + message;
    client.publish(node2_confirm, err.c_str());
    Serial.println("⚠️ Invalid command received");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP8266_Node2")) { // Unique ID
      Serial.println("connected!");
      client.subscribe(node2_topic);
      Serial.println("Subscribed to topic: esp/node2");
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH); // Turn LED off initially

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
  delay(50); // Avoid flooding
}
