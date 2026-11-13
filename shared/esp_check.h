#pragma once
#include <Arduino.h>
#include <esp_err.h>

// Stops the program if an ESP-IDF call failed, printing which call and why
inline void check(esp_err_t err, const char *what) {
    if (err != ESP_OK) {
        Serial.printf("%s failed: %s\n", what, esp_err_to_name(err));
        while (true) {
            delay(1000); // Infinite loop to halt execution
        }
    }
}