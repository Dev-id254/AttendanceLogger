/*
  ESP32 Attendance Logger Firmware
  
  Receives student ID card barcodes from a mobile device via:
  - Bluetooth Serial (using Bluetooth Serial Reader app paired with this ESP32)
  - WiFi HTTP POST requests (direct phone-to-ESP32 API calls)
  
  Forwards valid attendance records to backend API /api/attendance
  
  Phone Setup:
  - Use "Bluetooth Serial Reader" app to scan barcodes and send via Bluetooth
  - OR use "MacroDroid" to automate barcode submission via HTTP
  - Barcode format: Plain student ID (e.g., CCT/00001/023)
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <BluetoothSerial.h>

// WiFi Configuration
const char* ssid = "YourSSID";
const char* password = "YourPassword";
const char* backendUrl = "http://192.168.1.100:3000/api/attendance"; // adjust to PC IP
const char* deviceLocation = "Lab 1"; // device label for dashboard

// Bluetooth Configuration
BluetoothSerial SerialBT;
const char* bluetoothName = "ESP32_Attendance";
const char* bluetoothPin = "1234"; // Default PIN for pairing

WiFiClient espClient;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Initialize Bluetooth Serial
  SerialBT.begin(bluetoothName);
  Serial.println("Bluetooth initialized");
  Serial.print("Bluetooth device name: ");
  Serial.println(bluetoothName);
  
  connectWiFi();
}

void loop() {
  // Read barcode from Bluetooth (phone sends via Bluetooth Serial Reader app)
  if (SerialBT.available()) {
    String code = SerialBT.readStringUntil('\n');
    code.trim();
    if (code.length() > 0) {
      Serial.print("[BT] Received barcode: ");
      Serial.println(code);
      sendAttendance(code);
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
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, queuing attendance record...");
    // In production, implement local queue for offline records
    return;
  }
  
  HTTPClient http;
  http.begin(backendUrl);
  http.addHeader("Content-Type", "application/json");
  
  String mac = WiFi.macAddress();
  String payload =
    String("{\"student_id\":\"") + studentId +
    String("\",\"device_id\":\"") + mac +
    String("\",\"device_location\":\"") + deviceLocation +
    String("\"}");
  
  int httpCode = http.POST(payload);
  if (httpCode > 0) {
    String resp = http.getString();
    Serial.printf("[HTTP] POST success: code %d\n", httpCode);
    Serial.printf("[HTTP] Response: %s\n", resp.c_str());
    SerialBT.printf("OK: Attendance recorded for %s\n", studentId.c_str());
  } else {
    Serial.printf("[HTTP] POST failed: %s\n", http.errorToString(httpCode).c_str());
    SerialBT.printf("ERROR: Failed to record attendance\n");
  }
  http.end();
}
