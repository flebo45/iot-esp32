#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>

// PIN
constexpr int PIN_SDA = 19;
constexpr int PIN_SCL = 20;

Adafruit_BME280 bme;

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

    // Print the readings to the Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C, Humidity: ");
    Serial.print(humidity);
    Serial.print(" %, Pressure: ");
    Serial.print(pressure);
    Serial.println(" hPa");

    delay(5000); // Wait for 5 seconds before the next reading
}