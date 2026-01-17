#include <WiFi.h>

// ====== CONFIGURATION ======
const char* ssid_ap = "ESP32_Hotspot";     // Name of the hotspot
const char* password_ap = "12345678";      // Hotspot password (min 8 chars)

const char* ssid_sta = "Aayush";     // WiFi name to connect to
const char* password_sta = "12345678"; // WiFi password

void setup() {
  Serial.begin(115200);
  delay(1000);

  // --- Start Wi-Fi in Dual Mode ---
  WiFi.mode(WIFI_AP_STA);

  // --- Start Access Point ---
  bool ap_started = WiFi.softAP(ssid_ap, password_ap);
  if (ap_started) {
    Serial.println("Access Point started!");
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("Failed to start Access Point!");
  }

  // --- Connect to another Wi-Fi as a client ---
  WiFi.begin(ssid_sta, password_sta);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("Station IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Keep showing connection status
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 5000) {
    lastPrint = millis();
    Serial.print("STA connected: ");
    Serial.println(WiFi.status() == WL_CONNECTED ? "Yes" : "No");
  }
}
