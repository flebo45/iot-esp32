#include <Arduino.h>
#include <Wire.h>

// PIN
constexpr int PIN_SDA = 19;
constexpr int PIN_SCL = 20;

void setup() {
    Serial.begin(115200);
    delay(2000);
    // Initialize I2C communication
    Wire.begin(PIN_SDA, PIN_SCL);
}

void loop() {
    int trovati = 0;

    for (uint8_t addr = 1; addr < 127; ++addr) {
        Wire.beginTransmission(addr);        
        uint8_t esito = Wire.endTransmission();
        if (esito == 0) {
            Serial.print("Device found at address 0x");
            if (addr < 16) {
                Serial.print("0");
            }
            Serial.println(addr, HEX);
            trovati++;
        }
    }

    Serial.print("Total devices found: ");
    Serial.println(trovati);
    delay(1000);
}