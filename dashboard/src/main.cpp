// Dashboard IoT, passo 1: "Hello, Second Brain" sul Waveshare 7,5" 800x480 B/N.
// Hardware: ESP32-S3 (DevKitC-1 o compatibile) + e-Paper Driver HAT Waveshare.
// Collegamenti: vedi ../docs/collegamenti.md

#include <Arduino.h>
#include <SPI.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include "measure.h"
#include "secrets.h"

// Pin SPI (FSPI di default dello S3) e di controllo. Evitati i pin di strapping (0, 3, 45, 46),
// USB (19, 20) e quelli della PSRAM ottale (35-37).
constexpr int PIN_SCK  = 12;  // HAT: CLK
constexpr int PIN_MOSI = 11;  // HAT: DIN
constexpr int PIN_CS   = 10;  // HAT: CS
constexpr int PIN_DC   = 6;   // HAT: DC
constexpr int PIN_RST  = 5;   // HAT: RST
constexpr int PIN_BUSY = 4;   // HAT: BUSY
// HAT: PWR va a 3V3 (alimentazione del pannello sempre attiva), VCC a 3V3, GND a GND.

// Piano A: GDEW075T7 (Waveshare 7,5" V2 "classico").
// Piano B: GDEY075T7 (pannelli piu' recenti), attivabile da platformio.ini con -D EPD_GDEY075T7.
#ifdef EPD_GDEY075T7
using Panel = GxEPD2_750_GDEY075T7;
static const char *PANEL_NAME = "GxEPD2_750_GDEY075T7";
#else
using Panel = GxEPD2_750_T7;
static const char *PANEL_NAME = "GxEPD2_750_T7";
#endif

// Buffer intero (800x480 / 8 = 48 KB): sullo S3 la RAM non e' un problema.
GxEPD2_BW<Panel, Panel::HEIGHT> display(Panel(PIN_CS, PIN_DC, PIN_RST, PIN_BUSY));

// Scrive un testo centrato orizzontalmente con la baseline a quota y.
void drawCentered(const char *text, const GFXfont *font, int16_t y) {
  display.setFont(font);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((display.width() - w) / 2 - x1, y);
  display.print(text);
}

void setup() {
  Serial.begin(115200);
  delay(5000);
  Serial.printf("Avvio dashboard, driver: %s\n", PANEL_NAME);

  WiFi.mode(WIFI_STA);  // solo client, non serve AP

  uint8_t mac[6]= {};
  if (esp_wifi_get_mac(WIFI_IF_STA, mac) == ESP_OK){
    Serial.printf("MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  } else {
    Serial.println("Errore lettura MAC address.");
  }
    


  SPI.begin(PIN_SCK, -1, PIN_MOSI, PIN_CS);  // MISO non serve
  // reset_duration = 2 ms: richiesto dalle schede Waveshare con il circuito di reset "clever".
  display.init(115200, true, 2, false);
  display.setRotation(0);  // orizzontale, 800x480
  display.setTextColor(GxEPD_BLACK);
  display.setFullWindow();

  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.drawRect(16, 16, display.width() - 32, display.height() - 32, GxEPD_BLACK);
    drawCentered("Hello, Second Brain", &FreeSansBold24pt7b, display.height() / 2);
    drawCentered("Progetto IoT - passo 1", &FreeSans12pt7b, display.height() / 2 + 48);
    drawCentered(PANEL_NAME, &FreeSans12pt7b, display.height() - 40);
  } while (display.nextPage());

  display.hibernate();  // il pannello non va lasciato alimentato tra un refresh e l'altro
  Serial.println("Fatto: display in hibernate.");
}

void loop() {}
