# Collegamenti

> Schema dei collegamenti dei due nodi. Si aggiorna a ogni modifica dell'hardware.

## Nodo dashboard: ESP32-S3 + Waveshare e-Paper Driver HAT

Pin riferiti a una ESP32-S3-DevKitC-1 (o clone con la stessa piedinatura).

| HAT | ESP32-S3 | Note |
|---|---|---|
| VCC | 3V3 | il HAT accetta 3,3 o 5 V, ma la logica deve coincidere: con lo S3 usare 3V3 |
| GND | GND | |
| DIN | GPIO 11 | MOSI (FSPI) |
| CLK | GPIO 12 | SCK (FSPI) |
| CS | GPIO 10 | |
| DC | GPIO 6 | |
| RST | GPIO 5 | |
| BUSY | GPIO 4 | |
| PWR | 3V3 | solo HAT Rev 2.3 (connettore a 9 pin): se resta scollegato il pannello non si accende |

Interruttori sul HAT:
- **Display Config: B (0.47R)**, l'impostazione indicata da Waveshare per i 7,5".
- **Interface Config: 0** (SPI a 4 fili).

Cavo FPC del pannello: contatti verso il basso, levetta del connettore chiusa, nessuna piega stretta.

## Nodo esterno: FireBeetle 2 ESP32-C6 + BME280

Da definire alla milestone 2. Pin I2C di default della FireBeetle C6: SDA GPIO 19, SCL GPIO 20.
