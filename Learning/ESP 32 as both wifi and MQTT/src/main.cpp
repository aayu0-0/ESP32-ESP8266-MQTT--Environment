#include <WiFi.h>
#include <PubSubClient.h>

// ====== AP CONFIGURATION ======
const char* ssid_ap = "ESP32_Hotspot";
const char* password_ap = "12345678";

// ====== STA CONFIGURATION ======
const char* ssid_sta = "Aayush";
const char* password_sta = "12345678";

// ====== MQTT CONFIGURATION ======
const char* mqtt_server = "broker.hivemq.com";   // Public test broker
const int mqtt_port = 1883;
const char* mqtt_topic_pub = "esp32/test/pub";
const char* mqtt_topic_sub = "esp32/test/sub";

WiFiClient espClient;
PubSubClient client(espClient);

// ====== CALLBACK FUNCTION ======
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// ====== RECONNECT FUNCTION ======
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32_Client")) {
      Serial.println("connected!");
      client.subscribe(mqtt_topic_sub);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5s...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // --- WiFi Mode ---
  WiFi.mode(WIFI_AP_STA);

  // --- Start Access Point ---
  WiFi.softAP(ssid_ap, password_ap);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  // --- Connect to external WiFi ---
  WiFi.begin(ssid_sta, password_sta);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to router!");
  Serial.print("Station IP: ");
  Serial.println(WiFi.localIP());

  // --- MQTT Setup ---
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  // Reconnect MQTT if disconnected
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Publish message periodically
  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 5000) {
    lastMsg = millis();
    String message = "Hello from ESP32!";
    client.publish(mqtt_topic_pub, message.c_str());
    Serial.println("Published: " + message);
  }
}
