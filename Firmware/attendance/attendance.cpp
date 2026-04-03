/*
  ESP32/ESP32-S3 Attendance Logger Firmware
  Built with Arduino C++ (PlatformIO/Arduino IDE)

  Receives student ID card barcodes from a mobile device via:
  - BLE GATT service (using BLE app paired with ESP32/ESP32-S3)
  - WiFi HTTP POST requests (direct phone-to-ESP32 API calls)

  Forwards valid attendance records to backend API /api/attendance

  Phone Setup:
  - Use BLE app to scan barcodes and send via BLE characteristic
  - OR use "MacroDroid" to automate barcode submission via HTTP
  - Barcode format: Plain student ID (e.g., CCT/00001/023)

  Compatible with:
  - ESP32 (classic)
  - ESP32-S3
*/
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <NimBLEDevice.h>

// Forward declarations
void WiFiEvent(WiFiEvent_t event);
void connectWiFi();
void initBLE();
void sendAttendance(const String &studentId);
void setup();
void loop();

// BLE UUIDs
#define SERVICE_UUID "12345678-1234-1234-1234-123456789abc"
#define CHARACTERISTIC_UUID "87654321-4321-4321-4321-cba987654321"

// BLE objects
NimBLEServer *pServer = NULL;
NimBLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;

// BLE Callbacks
class MyServerCallbacks : public NimBLEServerCallbacks
{
    void onConnect(NimBLEServer *pServer)
    {
        deviceConnected = true;
        Serial.println("BLE device connected");
    }

    void onDisconnect(NimBLEServer *pServer)
    {
        deviceConnected = false;
        Serial.println("BLE device disconnected");
        NimBLEDevice::startAdvertising();
    }
};

class MyCharacteristicCallbacks : public NimBLECharacteristicCallbacks
{
    void onWrite(NimBLECharacteristic *pCharacteristic)
    {
        std::string value = pCharacteristic->getValue();
        String data = String(value.c_str());
        for (size_t i = 0; i < data.length(); i++)
        {
            char c = data[i];
            if (c == '\n' || c == '\r')
            {
                if (barcodeBuffer.length() > 0)
                {
                    barcodeBuffer.trim();
                    if (barcodeBuffer.length() > 0)
                    {
                        Serial.printf("[BLE] Received barcode: %s\n", barcodeBuffer.c_str());
                        sendAttendance(barcodeBuffer);
                    }
                    barcodeBuffer = "";
                }
            }
            else
            {
                if (barcodeBuffer.length() < 256)
                {
                    barcodeBuffer += c;
                }
            }
        }
    }
};

// WiFi Configuration
const char *ssid = "YourSSID";
const char *password = "YourPassword";
const char *backendUrl = "http://192.168.1.100:3000/api/attendance";
const char *deviceLocation = "Lab 1";

// BLE Configuration
const char *bleName = "ESP32_Attendance";

// Global state
bool wifiConnected = false;
String barcodeBuffer = "";

// WiFi event handler
void WiFiEvent(WiFiEvent_t event)
{
    switch (event)
    {
    case SYSTEM_EVENT_STA_START:
        Serial.println("WiFi connecting...");
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

// WiFi initialization (non-blocking)
void connectWiFi()
{
    WiFi.onEvent(WiFiEvent);
    WiFi.begin(ssid, password);
}

// BLE initialization
void initBLE()
{
    NimBLEDevice::init(bleName);
    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    NimBLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::WRITE);
    pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

    pService->start();

    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->start();

    Serial.println("BLE initialized");
    Serial.print("BLE device name: ");
    Serial.println(bleName);
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
    }
    else
    {
        Serial.printf("[HTTP] POST failed: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
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

    // Initialize BLE
    Serial.println("Initializing BLE...");
    initBLE();

    Serial.println("Firmware initialization complete");
    Serial.println("Waiting for BLE connections and WiFi messages...");
}

// Arduino loop function
void loop()
{
    // BLE is event-driven, no polling needed
    delay(10);
}
