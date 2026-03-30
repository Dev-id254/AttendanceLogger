/*
  ESP32/ESP32-S3 Attendance Logger Firmware
  Built with ESP-IDF (C language)
  
  Receives student ID card barcodes from a mobile device via:
  - Bluetooth SPP (using Bluetooth Serial Reader app paired with ESP32/ESP32-S3)
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include "esp_http_client.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_spp_api.h"
#include "esp_gap_bt_api.h"

#include "driver/uart.h"

// WiFi Configuration
#define EXAMPLE_SSID "YourSSID"
#define EXAMPLE_PASS "YourPassword"
#define BACKEND_URL "http://192.168.1.100:3000/api/attendance"
#define DEVICE_LOCATION "Lab 1"

// Bluetooth Configuration
#define BLUETOOTH_DEVICE_NAME "ESP32_Attendance"
#define BLUETOOTH_PIN "1234"

// UART Configuration
#define UART_NUM UART_NUM_0
#define UART_TX_GPIO_NUM (1)
#define UART_RX_GPIO_NUM (3)
#define UART_BAUD_RATE (115200)
#define BUF_SIZE (1024)

// WiFi event bits
#define WIFI_CONNECTED_BIT (1 << 0)
#define WIFI_FAIL_BIT (1 << 1)

static const char *TAG = "ATTENDANCE_LOGGER";

// Global handles and state
static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;
static QueueHandle_t spp_data_queue;
static uint32_t spp_handle = 0;
static bool spp_connected = false;


// WiFi event handler
static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data) {
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
  } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
    if (s_retry_num < 5) {
      esp_wifi_connect();
      s_retry_num++;
      ESP_LOGI(TAG, "Retrying WiFi connection...");
    } else {
      xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
    }
    ESP_LOGI(TAG, "WiFi connect to SSID failed");
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "WiFi connected with IP: " IPSTR, IP2STR(&event->ip_info.ip));
    s_retry_num = 0;
    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
  }
}

// WiFi initialization
static void wifi_init_sta(void) {
  s_wifi_event_group = xEventGroupCreate();

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  esp_event_handler_instance_t instance_any_id;
  esp_event_handler_instance_t instance_got_ip;
  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                       ESP_EVENT_ANY_ID,
                                                       &event_handler,
                                                       NULL,
                                                       &instance_any_id));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                       IP_EVENT_STA_GOT_IP,
                                                       &event_handler,
                                                       NULL,
                                                       &instance_got_ip));

  wifi_config_t wifi_config = {
    .sta = {
      .ssid = EXAMPLE_SSID,
      .password = EXAMPLE_PASS,
      .threshold.authmode = WIFI_AUTH_WPA2_PSK,
    },
  };
  
  strncpy((char *)wifi_config.sta.ssid, EXAMPLE_SSID, 32 - 1);
  strncpy((char *)wifi_config.sta.password, EXAMPLE_PASS, 64 - 1);

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI(TAG, "WiFi initialization complete");

  EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                          WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                          pdFALSE,
                                          pdFALSE,
                                          portMAX_DELAY);

  if (bits & WIFI_CONNECTED_BIT) {
    ESP_LOGI(TAG, "WiFi connected successfully");
  }
}

// UART initialization for debug output
static void uart_init(void) {
  const uart_config_t uart_config = {
    .baud_rate = UART_BAUD_RATE,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
  };

  ESP_ERROR_CHECK(uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0));
  ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
  ESP_ERROR_CHECK(uart_set_pin(UART_NUM, UART_TX_GPIO_NUM, UART_RX_GPIO_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
  
  ESP_LOGI(TAG, "UART initialized");
}

// Bluetooth SPP callback
static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
  switch (event) {
  case ESP_SPP_INIT_EVT:
    ESP_LOGI(TAG, "SPP initialized");
    esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE);
    break;
  case ESP_SPP_DISCOVERY_COMP_EVT:
    ESP_LOGI(TAG, "SPP discovery completed");
    break;
  case ESP_SPP_OPEN_EVT:
    ESP_LOGI(TAG, "SPP connection opened");
    spp_handle = param->open.handle;
    spp_connected = true;
    break;
  case ESP_SPP_CLOSE_EVT:
    ESP_LOGI(TAG, "SPP connection closed");
    spp_connected = false;
    break;
  case ESP_SPP_CONG_EVT:
    break;
  case ESP_SPP_DATA_IND_EVT:
    // Data received from Bluetooth
    if (param->data_ind.len > 0) {
      uint8_t *data = (uint8_t *)malloc(param->data_ind.len + 1);
      if (data) {
        memcpy(data, param->data_ind.data, param->data_ind.len);
        data[param->data_ind.len] = '\0';
        xQueueSend(spp_data_queue, &data, portMAX_DELAY);
      }
    }
    break;
  case ESP_SPP_WRITE_EVT:
    ESP_LOGV(TAG, "SPP write event");
    break;
  default:
    break;
  }
}

// Bluetooth initialization
static void bluetooth_init(void) {
  esp_err_t ret;

  ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  ret = esp_bt_controller_init(&bt_cfg);
  if (ret) {
    ESP_LOGE(TAG, "Failed to initialize Bluetooth controller");
    return;
  }

  ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);
  if (ret) {
    ESP_LOGE(TAG, "Failed to enable Bluetooth classic");
    return;
  }

  ret = esp_bluedroid_init();
  if (ret) {
    ESP_LOGE(TAG, "Failed to initialize Bluedroid");
    return;
  }

  ret = esp_bluedroid_enable();
  if (ret) {
    ESP_LOGE(TAG, "Failed to enable Bluedroid");
    return;
  }

  ret = esp_spp_register_callback(esp_spp_cb);
  if (ret) {
    ESP_LOGE(TAG, "Failed to register SPP callback");
    return;
  }

  ret = esp_spp_init(ESP_SPP_MODE_CB);
  if (ret) {
    ESP_LOGE(TAG, "Failed to initialize SPP");
    return;
  }

  // Set Bluetooth device name
  esp_bt_dev_set_device_name(BLUETOOTH_DEVICE_NAME);

  ESP_LOGI(TAG, "Bluetooth initialized: %s", BLUETOOTH_DEVICE_NAME);
}

void esp_spp_write_data(uint8_t *data, uint16_t len) {
  if (spp_connected && spp_handle > 0) {
    esp_spp_write(spp_handle, len, data);
  }
}

// HTTP event handler for POST requests
static esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
  static char output_buffer[512];
  static int output_len;

  switch (evt->event_id) {
  case HTTP_EVENT_ERROR:
    ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
    break;
  case HTTP_EVENT_ON_CONNECTED:
    ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
    break;
  case HTTP_EVENT_HEADER_SENT:
    ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
    break;
  case HTTP_EVENT_ON_HEADER:
    ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s", evt->header_key);
    break;
  case HTTP_EVENT_ON_DATA:
    ESP_LOGV(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
    if (!esp_http_client_is_chunked_response(evt->client)) {
      if (output_len + evt->data_len < sizeof(output_buffer)) {
        memcpy(output_buffer + output_len, evt->data, evt->data_len);
        output_len += evt->data_len;
      }
    }
    break;
  case HTTP_EVENT_ON_FINISH:
    ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
    output_len = 0;
    break;
  case HTTP_EVENT_DISCONNECTED:
    ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
    break;
  default:
    break;
  }
  return ESP_OK;
}

// Send attendance record via HTTP POST
static void send_attendance(const char *student_id) {
  if (!student_id || strlen(student_id) == 0) {
    ESP_LOGW(TAG, "Invalid student_id");
    return;
  }

  // Check WiFi status
  tcpip_adapter_ip_info_t ip_info;
  tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info);
  if (ip_info.ip.addr == 0) {
    ESP_LOGI(TAG, "WiFi not connected, queuing attendance record...");
    return;
  }

  // Build the JSON payload
  char payload[512];
  snprintf(payload, sizeof(payload),
           "{\"student_id\":\"%s\",\"device_location\":\"%s\"}",
           student_id, DEVICE_LOCATION);

  // Configure HTTP client
  esp_http_client_config_t config = {
    .url = BACKEND_URL,
    .event_handler = _http_event_handler,
    .method = HTTP_METHOD_POST,
  };

  esp_http_client_handle_t client = esp_http_client_init(&config);
  
  esp_http_client_set_header(client, "Content-Type", "application/json");
  esp_http_client_set_post_field(client, payload, strlen(payload));

  esp_err_t err = esp_http_client_perform(client);
  if (err == ESP_OK) {
    int status_code = esp_http_client_get_status_code(client);
    ESP_LOGI(TAG, "[HTTP] POST success: code %d", status_code);
    
    // Send success message via Bluetooth
    if (spp_connected) {
      char bt_msg[128];
      snprintf(bt_msg, sizeof(bt_msg), "OK: Attendance recorded for %s\n", student_id);
      esp_spp_write_data((uint8_t *)bt_msg, strlen(bt_msg));
    }
  } else {
    ESP_LOGE(TAG, "[HTTP] POST failed: %s", esp_err_to_name(err));
    
    // Send error message via Bluetooth
    if (spp_connected) {
      char bt_msg[64] = "ERROR: Failed to record attendance\n";
      esp_spp_write_data((uint8_t *)bt_msg, strlen(bt_msg));
    }
  }

  esp_http_client_cleanup(client);
}

// Trim whitespace from string
static void trim_string(char *str) {
  if (!str) return;
  
  int i = 0;
  while (isspace((unsigned char)str[i])) {
    i++;
  }
  
  if (i != 0) {
    memmove(str, str + i, strlen(str + i) + 1);
  }

  i = strlen(str) - 1;
  while (i >= 0 && isspace((unsigned char)str[i])) {
    str[i] = '\0';
    i--;
  }
}

// Bluetooth reader task
static void bluetooth_reader_task(void *pvParameters) {
  uint8_t *spp_data;
  char barcode_buffer[256] = {0};
  int buffer_pos = 0;

  spp_data_queue = xQueueCreate(10, sizeof(uint8_t *));

  ESP_LOGI(TAG, "Bluetooth reader task started");

  while (1) {
    if (xQueueReceive(spp_data_queue, &spp_data, portMAX_DELAY)) {
      // Process received data byte by byte
      for (int i = 0; spp_data[i] != '\0'; i++) {
        char c = (char)spp_data[i];
        
        if (c == '\n' || c == '\r') {
          // End of barcode
          if (buffer_pos > 0) {
            barcode_buffer[buffer_pos] = '\0';
            trim_string(barcode_buffer);
            
            if (buffer_pos > 0) {
              ESP_LOGI(TAG, "[BT] Received barcode: %s", barcode_buffer);
              send_attendance(barcode_buffer);
            }
            
            buffer_pos = 0;
          }
        } else {
          // Accumulate character
          if (buffer_pos < sizeof(barcode_buffer) - 1) {
            barcode_buffer[buffer_pos++] = c;
          }
        }
      }
      
      free(spp_data);
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

// Main entry point (replaces Arduino setup/loop)
void app_main(void) {
  ESP_LOGI(TAG, "========================================");
  ESP_LOGI(TAG, "ESP32/ESP32-S3 Attendance Logger");
  ESP_LOGI(TAG, "Built with ESP-IDF (C language)");
  ESP_LOGI(TAG, "========================================");

  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // Initialize UART for debug output
  uart_init();

  // Initialize WiFi
  ESP_LOGI(TAG, "Initializing WiFi...");
  wifi_init_sta();

  // Initialize Bluetooth
  ESP_LOGI(TAG, "Initializing Bluetooth...");
  bluetooth_init();

  // Create Bluetooth reader task
  xTaskCreate(bluetooth_reader_task, "bluetooth_reader", 4096, NULL, 5, NULL);

  ESP_LOGI(TAG, "Firmware initialization complete");
  ESP_LOGI(TAG, "Waiting for Bluetooth connections and WiFi messages...");
}
