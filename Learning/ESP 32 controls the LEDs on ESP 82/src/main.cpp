#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Aayush";
const char* password = "12345678";
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

String validCmds[] = {"on1", "off1", "on2", "off2"};
String lastCommand = "";   // Stores the last valid command
bool resendPending = false;

bool isValidCommand(String cmd) {
  for (String valid : validCmds) {
    if (cmd == valid) return true;
  }
  return false;
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Confirmation: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Reconnecting to MQTT...");
    if (client.connect("ESP32_Client")) {
      Serial.println("connected!");
      client.subscribe("/esp/led/confirm");

      // If we had a command before disconnect, re-send it
      if (resendPending && lastCommand != "") {
        client.publish("/esp/led", lastCommand.c_str());
        Serial.print("🔁 Re-sending last command after reconnect: ");
        Serial.println(lastCommand);
        resendPending = false;  // Reset after sending
      }
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

  Serial.println("Ready! Type commands: on1, off1, on2, off2");
}

void loop() {
  if (!client.connected()) {
    resendPending = true;   // Mark that we should re-send after reconnect
    reconnect();
  }
  client.loop();

  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (isValidCommand(command)) {
      client.publish("/esp/led", command.c_str());
      lastCommand = command;     // Save it
      resendPending = true;      // If disconnect happens later, resend it
      Serial.print("Command sent: ");
      Serial.println(command);
    } else {
      Serial.println("⚠️ Invalid command! Use only: on1, off1, on2, off2");
    }
  }
}
