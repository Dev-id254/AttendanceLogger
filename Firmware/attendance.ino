/*
  MVP Firmware sketch for ESP32 (Arduino framework)
  - connects to Wi-Fi
  - reads barcode strings from serial (simulated scanner)
  - sends HTTP POST to backend API /api/attendance
  - demonstrates MQTT publish as an alternative

  For actual hardware, barcode scanner should be connected via USB Host or UART.
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h> // optional for MQTT

const char* ssid = "YourSSID";
const char* password = "YourPassword";
const char* backendUrl = "http://192.168.1.100:3000/api/attendance"; // adjust to PC IP

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void setup() {
  Serial.begin(115200);
  delay(1000);
  connectWiFi();

  // Example: connect to MQTT broker (optional)
  mqttClient.setServer("broker.hivemq.com", 1883);
}

void loop() {
  // read from serial (could be barcode scanner output)
  if (Serial.available()) {
    String code = Serial.readStringUntil('\n');
    code.trim();
    if (code.length() > 0) {
      Serial.print("Read barcode: ");
      Serial.println(code);
      sendAttendance(code);
    }
  }
  // handle mqtt loop if used
  if (mqttClient.connected()) {
    mqttClient.loop();
  } else {
    // try reconnecting occasionally
    if (millis() % 5000 < 50) {
      mqttReconnect();
    }
  }
  delay(10);
}

void connectWiFi() {
  Serial.printf("Connecting to %s", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void sendAttendance(const String& studentId) {
  if (WiFi.status() != WL_CONNECTED) return;
  HTTPClient http;
  http.begin(backendUrl);
  http.addHeader("Content-Type", "application/json");
  String payload = String("{\"student_id\":\"") + studentId + "\"}";
  int httpCode = http.POST(payload);
  if (httpCode > 0) {
    String resp = http.getString();
    Serial.printf("Attendance posted, code %d resp %s\n", httpCode, resp.c_str());
  } else {
    Serial.printf("HTTP POST failed: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void mqttReconnect() {
  if (mqttClient.connect("esp32-client")) {
    Serial.println("MQTT connected");
  }
}
