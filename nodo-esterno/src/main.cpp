#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include "measure.h"
#include "secrets.h"
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

// PIN
constexpr int PIN_SDA = 19;
constexpr int PIN_SCL = 20;

// Global for measurement data
uint32_t measure_counter = 0; // Counter for the number of measurements taken
// Written by the send callback (Wi-Fi task), read by loop(): volatile so the compiler re-reads them every time
volatile bool send_done = false; // true when the send callback for the last packet has arrived
volatile esp_now_send_status_t last_send_status = ESP_NOW_SEND_FAIL; // MAC-level result of the last packet

Adafruit_BME280 bme;

void onSent(const esp_now_send_info_t *info, esp_now_send_status_t status) {
    // Runs in the Wi-Fi task: only copy the result and raise the flag, no Serial here
    last_send_status = status;
    send_done = true; // written last, so when loop() sees it the status is already set
}

// Stops the program if an ESP-IDF call failed, printing which call and why
void check(esp_err_t err, const char *what) {
    if (err != ESP_OK) {
        Serial.printf("%s failed: %s\n", what, esp_err_to_name(err));
        while (true) {
            delay(1000); // Infinite loop to halt execution
        }
    }
}


void setup() {
    Serial.begin(115200);
    delay(2000);
    // Initialize I2C communication
    Wire.begin(PIN_SDA, PIN_SCL);

    // Check if the BME280 sensor is connected
    if (!bme.begin(0x76, &Wire)) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (true) {
            delay(1000); // Infinite loop to halt execution
        }
    }

    bme.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1,
                    Adafruit_BME280::SAMPLING_X1,
                    Adafruit_BME280::SAMPLING_X1,
                    Adafruit_BME280::FILTER_OFF,
                    Adafruit_BME280::STANDBY_MS_0_5);

    Serial.print("BME280 sensor ID: ");
    Serial.println(bme.sensorID(), HEX);

    WiFi.mode(WIFI_STA);
    // Every esp_* call returns an esp_err_t: if one fails, check() says which one and stops
    check(esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE), "esp_wifi_set_channel");
    check(esp_now_init(), "esp_now_init");
    check(esp_now_register_send_cb(onSent), "esp_now_register_send_cb");

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, DASHBOARD_ADDRESS, 6);
    peerInfo.channel = ESPNOW_CHANNEL;
    peerInfo.encrypt = false;
    peerInfo.ifidx = WIFI_IF_STA;
    check(esp_now_add_peer(&peerInfo), "esp_now_add_peer");

    Serial.println("ESP-NOW ready");
}

void loop() {

    if (!bme.takeForcedMeasurement()) {
        Serial.println("Failed to take measurement!");
        delay(1000); // Wait for a second before retrying
        return;
    }

    // Read temperature, humidity, and pressure from the BME280 sensor
    float temperature = bme.readTemperature();
    float humidity = bme.readHumidity();
    float pressure = bme.readPressure() / 100.0F; // Convert to hPa

    // Prepare the Measure struct
    Measure measure;
    measure.version = MEASURE_VERSION;
    measure.counter = measure_counter++; // Increment the counter for each measurement

    // Fill the Measure struct with the sensor data
    measure.temperature = temperature;
    measure.humidity = humidity;
    measure.pressure = pressure;

    // Print the readings to the Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C, Humidity: ");
    Serial.print(humidity);
    Serial.print(" %, Pressure: ");
    Serial.print(pressure);
    Serial.println(" hPa");

    // 1. Lower the flag BEFORE sending: from now on, send_done == true can only mean "this packet"
    send_done = false;

    // 2. Return value of esp_now_send(): ESP_OK only means "queued", not "delivered"
    esp_err_t err = esp_now_send(DASHBOARD_ADDRESS, (uint8_t *)&measure, sizeof(Measure));
    if (err != ESP_OK) {
        Serial.printf("#%lu NOT QUEUED: %s\n", (unsigned long)measure.counter, esp_err_to_name(err));
        delay(5000);
        return; // no callback will arrive for a packet that was never queued
    }

    // 3. Wait for the send callback, at most 100 ms
    unsigned long start_time = millis();
    while (!send_done && (millis() - start_time < 100)) {
        delay(1);
    }

    // 4. Only now the status refers to this packet: SUCCESS = the S3 radio sent the MAC ACK
    if (!send_done) {
        Serial.printf("#%lu TIMEOUT\n", (unsigned long)measure.counter);
    } else if (last_send_status == ESP_NOW_SEND_SUCCESS) {
        Serial.printf("#%lu SUCCESS\n", (unsigned long)measure.counter);
    } else {
        Serial.printf("#%lu FAIL\n", (unsigned long)measure.counter);
    }

    delay(5000); // Wait for 5 seconds before the next reading
}