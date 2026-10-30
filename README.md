# iot-esp32: dashboard e-ink da quadro con sensore esterno

Una dashboard per casa: un display e-ink da 7,5" dentro un quadro, che si aggiorna da sola e mostra le condizioni esterne misurate da un nodo sensori solare, il meteo e il Second Brain.

## Architettura

| Nodo | Hardware | Alimentazione | Ruolo |
|---|---|---|---|
| Esterno | FireBeetle 2 ESP32-C6 + BME280 | LiPo 2000 mAh (+ pannello solare 1 W in un secondo momento) | legge temperatura, umidita' e pressione, invia via ESP-NOW e torna in deep sleep |
| Dashboard | ESP32-S3 + Waveshare 7,5" 800x480 B/N (e-Paper Driver HAT) | cavo USB | riceve i dati, scarica il meteo via Wi-Fi, disegna la pagina |

## Struttura

- `dashboard/`: progetto PlatformIO del nodo dashboard
- `nodo-esterno/`: progetto PlatformIO del nodo esterno (FireBeetle C6 + BME280)
- `docs/collegamenti.md`: schema dei collegamenti

## Stato

- Milestone 1 ✅ display e-ink funzionante ("Hello, Second Brain").
- Milestone 2 ✅ nodo esterno al banco: temperatura, umidità e pressione via seriale, BME280 in modalità forced.
- Prossima: milestone 3, ESP-NOW dal nodo esterno alla dashboard.

## Compilare e caricare

Da VS Code con PlatformIO: apri la cartella `dashboard/`, poi Build e Upload. Da terminale:

```
cd dashboard
pio run -t upload && pio device monitor
```
