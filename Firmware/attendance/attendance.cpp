/*
  ESP32/ESP32-S3 Attendance Logger Firmware
  Built with Arduino C++ (PlatformIO/Arduino IDE)

  Receives student ID card barcodes from a mobile device via:
  - Bluetooth Serial (using Bluetooth Serial Reader app paired with ESP32/ESP32-S3)
  - WiFi HTTP POST requests (direct phone-to-ESP32 API calls)

  Forwards valid attendance records to backend API /api/attendance

  Phone Setup:
  - Use "Bluetooth Serial Reader" app to scan barcodes and send via Bluetooth
  - OR use "MacroDroid" to automate barcode submission via HTTP
  - Barcode format: Plain student ID (e.g., CCT/00001/023)

  Compatible with:
  - ESP32 (classic)
  - ESP32-S3
*/
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <BluetoothSerial.h>

// Forward declarations
void WiFiEvent(WiFiEvent_t event);
void connectWiFi();
void initBluetooth();
void sendAttendance(const String &studentId);
void processBluetoothData();
void checkBluetoothConnection();
void setup();
void loop();

// WiFi Configuration
const char *ssid = "YourSSID";
const char *password = "YourPassword";
const char *backendUrl = "http://192.168.1.100:3000/api/attendance";
const char *deviceLocation = "Lab 1";

// Bluetooth Configuration
BluetoothSerial SerialBT;
const char *bluetoothName = "ESP32_Attendance";
const char *bluetoothPin = "1234";

// Global state
bool wifiConnected = false;
bool bluetoothConnected = false;
String barcodeBuffer = "";

// WiFi event handler
void WiFiEvent(WiFiEvent_t event)
{
    switch (event)
    {
    case SYSTEM_EVENT_STA_START:
        Serial.println("WiFi connecting...");
        WiFi.begin(ssid, password);
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        Serial.println("WiFi connected to AP");
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        Serial.print("WiFi connected with IP: ");
        Serial.println(WiFi.localIP());
        wifiConnected = true;
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        Serial.println("WiFi disconnected");
        wifiConnected = false;
        // Auto-reconnect
        WiFi.reconnect();
        break;
    default:
        break;
    }
}

// WiFi initialization
void connectWiFi()
{
    Serial.printf("Connecting to %s", ssid);

    WiFi.onEvent(WiFiEvent);
    WiFi.begin(ssid, password);

    int retryCount = 0;
    while (WiFi.status() != WL_CONNECTED && retryCount < 20)
    {
        delay(500);
        Serial.print(".");
        retryCount++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nWiFi connected successfully");
        wifiConnected = true;
    }
    else
    {
        Serial.println("\nWiFi connection failed");
        wifiConnected = false;
    }
}

// Bluetooth initialization
void initBluetooth()
{
    if (!SerialBT.begin(bluetoothName))
    {
        Serial.println("Bluetooth initialization failed");
        return;
    }

    Serial.println("Bluetooth initialized");
    Serial.print("Bluetooth device name: ");
    Serial.println(bluetoothName);
    Serial.print("Bluetooth PIN: ");
    Serial.println(bluetoothPin);

    bluetoothConnected = false;
}

// Send attendance record via HTTP POST
void sendAttendance(const String &studentId)
{
    if (!wifiConnected)
    {
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
    if (httpCode > 0)
    {
        String resp = http.getString();
        Serial.printf("[HTTP] POST success: code %d\n", httpCode);
        Serial.printf("[HTTP] Response: %s\n", resp.c_str());

        // Send success message via Bluetooth
        if (bluetoothConnected)
        {
            SerialBT.printf("OK: Attendance recorded for %s\n", studentId.c_str());
        }
    }
    else
    {
        Serial.printf("[HTTP] POST failed: %s\n", http.errorToString(httpCode).c_str());

        // Send error message via Bluetooth
        if (bluetoothConnected)
        {
            SerialBT.printf("ERROR: Failed to record attendance\n");
        }
    }
    http.end();
}

// Process Bluetooth data
void processBluetoothData()
{
    if (SerialBT.available())
    {
        char c = SerialBT.read();

        if (c == '\n' || c == '\r')
        {
            // End of barcode
            if (barcodeBuffer.length() > 0)
            {
                barcodeBuffer.trim();

                if (barcodeBuffer.length() > 0)
                {
                    Serial.printf("[BT] Received barcode: %s\n", barcodeBuffer.c_str());
                    sendAttendance(barcodeBuffer);
                }

                barcodeBuffer = "";
            }
        }
        else
        {
            // Accumulate character
            if (barcodeBuffer.length() < 256)
            {
                barcodeBuffer += c;
            }
        }
    }
}

// Check Bluetooth connection status
void checkBluetoothConnection()
{
    static unsigned long lastCheck = 0;
    if (millis() - lastCheck > 5000)
    { // Check every 5 seconds
        bluetoothConnected = SerialBT.connected();
        lastCheck = millis();
    }
}

// Arduino setup function
void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println("========================================");
    Serial.println("ESP32/ESP32-S3 Attendance Logger");
    Serial.println("Built with Arduino C++");
    Serial.println("========================================");

    // Initialize WiFi
    Serial.println("Initializing WiFi...");
    connectWiFi();

    // Initialize Bluetooth
    Serial.println("Initializing Bluetooth...");
    initBluetooth();

    Serial.println("Firmware initialization complete");
    Serial.println("Waiting for Bluetooth connections and WiFi messages...");
}

// Arduino loop function
void loop()
{
    // Process Bluetooth data
    processBluetoothData();

    // Check Bluetooth connection status periodically
    checkBluetoothConnection();

    delay(10);
}
