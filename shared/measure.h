#pragma once
#include <cstdint>
#include <esp_now.h>

constexpr uint8_t MEASURE_VERSION = 1;

struct Measure {
    uint8_t version;   // Version of the measure. Should be equal to MEASURE_VERSION else thhe packet is discarded
    uint32_t counter;   // Counter of the measure. +1 at each sending: a hole in the sequence indicates a lost packet
    float temperature; // Temperature in Celsius
    float humidity;    // Humidity in percentage
    float pressure;    // Pressure in hPa
}__attribute__((packed)); // no padding, to have same disposition of fields on all platforms

static_assert(sizeof(Measure) <= ESP_NOW_MAX_DATA_LEN , "Measure struct size exceeds ESP-NOW maximum data length");