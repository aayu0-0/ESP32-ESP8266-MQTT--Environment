#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Aayush";
const char* password = "12345678";
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// Topics
const char* node1_topic = "esp/node1";
const char* node1_confirm = "esp/node1/confirm";

// LED pin
const int ledPin = 2; // Built-in LED

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  message.trim();

  Serial.print("Received command: ");
  Serial.println(message);

  if (message == "on") {
    digitalWrite(ledPin, HIGH);
    client.publish(node1_confirm, "LED ON");
    Serial.println("LED turned ON");
  } else if (message == "off") {
    digitalWrite(ledPin, LOW);
    client.publish(node1_confirm, "LED OFF");
    Serial.println("LED turned OFF");
  } else if (message == "hi") {
    client.publish(node1_confirm, "Hi received");
    Serial.println("Hi received, confirmation sent");
  } else {
    String err = "INVALID CMD: " + message;
    client.publish(node1_confirm, err.c_str());
    Serial.println("⚠️ Invalid command received");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32_Node1")) { // Unique ID
      Serial.println("connected!");
      client.subscribe(node1_topic);
      Serial.println("Subscribed to topic: " + String(node1_topic));
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
  delay(50); // Prevent flooding
}
