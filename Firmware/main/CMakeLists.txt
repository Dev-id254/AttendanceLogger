idf_component_register(SRCS "attendance/attendance.ino"
                       INCLUDE_DIRS "."
                       REQUIRES driver esp_wifi esp_bt esp_http_client)
