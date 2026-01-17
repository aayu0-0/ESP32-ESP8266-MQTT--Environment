#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Aayush";
const char* password = "12345678";
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// Node topics
const char* node1_topic = "esp/node1";
const char* node1_confirm = "esp/node1/confirm";

const char* node2_topic = "esp/node2";
const char* node2_confirm = "esp/node2/confirm";

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  message.trim();

  Serial.print("Confirmation received from ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(message);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32_Broker_Main")) { // Unique ID
      Serial.println("connected!");
      // Subscribe to confirmation topics
      client.subscribe(node1_confirm);
      client.subscribe(node2_confirm);
      Serial.println("Subscribed to all confirmation topics");
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

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
  client.setCallback(callback);

  reconnect();

  Serial.println("Ready! Type commands:");
  Serial.println("node1:on / node1:off / node1:hi");
  Serial.println("node2:on / node2:off / node2:hi");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.startsWith("node1:")) {
      String payload = command.substring(6); // Remove "node1:"
      client.publish(node1_topic, payload.c_str());
      Serial.println("Command sent to Node1: " + payload);
    } 
    else if (command.startsWith("node2:")) {
      String payload = command.substring(6); // Remove "node2:"
      client.publish(node2_topic, payload.c_str());
      Serial.println("Command sent to Node2: " + payload);
    } 
    else {
      Serial.println("Invalid command. Use node1:<on/off/hi> or node2:<on/off/hi>");
    }
  }

  delay(50); // Small delay to avoid flooding
}
