// Diagnostica e-Paper Waveshare 7,5" + Driver HAT, senza librerie.
// 1) BUSY e' pilotato o flottante?  2) Il pannello reagisce al reset?
// 3) Il pannello reagisce al comando di accensione (PON, 0x04 sui controller UC8179)?
// Sul 7,5" V2 BUSY e' attivo basso: LOW = occupato, HIGH = libero.

#include <Arduino.h>
#include <SPI.h>

constexpr int PIN_SCK  = 12;
constexpr int PIN_MOSI = 11;
constexpr int PIN_CS   = 10;
constexpr int PIN_DC   = 6;
constexpr int PIN_RST  = 5;
constexpr int PIN_BUSY = 4;

void command(uint8_t c) {
  digitalWrite(PIN_DC, LOW);
  digitalWrite(PIN_CS, LOW);
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  SPI.transfer(c);
  SPI.endTransaction();
  digitalWrite(PIN_CS, HIGH);
}

void data(uint8_t d) {
  digitalWrite(PIN_DC, HIGH);
  digitalWrite(PIN_CS, LOW);
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  SPI.transfer(d);
  SPI.endTransaction();
  digitalWrite(PIN_CS, HIGH);
}

// Campiona BUSY per durata_ms e stampa solo i cambi di stato, con il tempo.
void osservaBusy(const char *fase, uint32_t durata_ms) {
  uint32_t t0 = millis();
  int prec = digitalRead(PIN_BUSY);
  Serial.printf("  [%s] t=0 ms BUSY=%d\n", fase, prec);
  int cambi = 0;
  while (millis() - t0 < durata_ms) {
    int v = digitalRead(PIN_BUSY);
    if (v != prec) {
      Serial.printf("  [%s] t=%lu ms BUSY=%d\n", fase, millis() - t0, v);
      prec = v;
      cambi++;
    }
    delayMicroseconds(200);
  }
  Serial.printf("  [%s] fine: %d cambi, BUSY finale=%d\n", fase, cambi, prec);
}

void diagnostica() {
  Serial.println("\n=== Diagnostica e-Paper ===");

  // 1) Flottante? Con pull-up e pull-down interni: se il valore segue la resistenza,
  //    nessuno sta pilotando la linea (filo staccato o HAT/pannello muti).
  pinMode(PIN_BUSY, INPUT_PULLUP);   delay(5);
  int conPullUp = digitalRead(PIN_BUSY);
  pinMode(PIN_BUSY, INPUT_PULLDOWN); delay(5);
  int conPullDown = digitalRead(PIN_BUSY);
  pinMode(PIN_BUSY, INPUT);
  Serial.printf("1) BUSY con pull-up=%d, con pull-down=%d -> %s\n", conPullUp, conPullDown,
                (conPullUp == 1 && conPullDown == 0) ? "FLOTTANTE (nessuno pilota BUSY)"
                                                     : "pilotato dal HAT/pannello");

  // 2) Reset hardware: un pannello vivo di solito va occupato (BUSY=0) per qualche ms.
  Serial.println("2) Impulso di reset (RST basso 10 ms)");
  digitalWrite(PIN_RST, LOW);  delay(10);
  digitalWrite(PIN_RST, HIGH);
  osservaBusy("reset", 500);

  // 3) Accensione: power setting minimo + PON. Un UC8179 vivo tiene BUSY=0 per decine/centinaia di ms.
  Serial.println("3) Comando di accensione (0x01 power setting, 0x04 PON)");
  command(0x01); data(0x07); data(0x07); data(0x3F); data(0x3F);
  command(0x04);
  osservaBusy("PON", 3000);

  // Spegnimento (POF) e deep sleep, per non lasciare il pannello alimentato.
  command(0x02);
  osservaBusy("POF", 1000);
  command(0x07); data(0xA5);
  Serial.println("Fatto. Ripeto tra 10 s.");
}

void setup() {
  Serial.begin(115200);
  delay(1500);
  pinMode(PIN_CS, OUTPUT);  digitalWrite(PIN_CS, HIGH);
  pinMode(PIN_DC, OUTPUT);  digitalWrite(PIN_DC, HIGH);
  pinMode(PIN_RST, OUTPUT); digitalWrite(PIN_RST, HIGH);
  pinMode(PIN_BUSY, INPUT);
  SPI.begin(PIN_SCK, -1, PIN_MOSI, -1);  // CS gestito a mano
}

void loop() {
  diagnostica();
  delay(10000);
}
