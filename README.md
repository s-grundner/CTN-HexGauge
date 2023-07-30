# (DE) Automobil Telemetrie Netzwerk

## Master Controller

| Nr  | Relevante Sensoren am Can bus | Instrumentenadresse/ID | Serienmäßig |
| --- | ----------------------------- | ---------------------- | ----------- |
| 1   | Motordrehzahl                 | `0x`                   | ja          |
| 2   | Geschwindigkeit               | `0x`                   | ja          |
| 3   | Reifendrehzahl                | `0x`                   | ja          |
|     |                               |                        |             |
| 4   | Batteriespannung              | `0x`                   | ja          |
|     |                               |                        |             |
| 5   | Öltemperatur                  | `0x`                   | ja          |
| 6   | Kühlfl. Temperatur            | `0x`                   | ja          |
| 13  | Reifentemparatur              | `0x`                   | nein        |
|     |                               |                        |             |
| 7   | Öldruck                       | `0x`                   | ja          |
| 8   | Bremsdruck                    | `0x`                   | ja          |
| 9   | Kraftstoffdruck               | `0x`                   | ja          |
|     |                               |                        |             |
| 10  | Gaspedalposition              | `0x`                   | ja          |
| 11  | Gyroskop                      | `0x`                   | nein        |


### Berechnungen

- CAN Bus Interface mit uC
  - Codes Auslesen
  - Ausgelesene Codes auf Armaturen schreiben



- Bremsschwund Erkennung und Berechnung
  - Vergleicht Bremsleitungsdruck mit Entschleunigung
  - Zeichnet Werte bei Verschiedenen Geschwindigkeiten auf und vergleicht

## Funktionseinheiten

### Reifentemaparatursensor

### Gyroskop

### Drehzahl LED-Leiste

## Modulare Hex Armaturen

- Ein zentrales Master Instrument, wo die Daten eingespeist werden (Drahtlos/Verdrahtet)
- Bus erweiterungen an 2/3/6 seiten des Hexagons
- Instrumentenadresse mit DIP-Schaltern einstellbar (SC18IS602B)
  - ID bestimmt angezeigte Daten
  - ID wird in SPI Chipselect konvertiert
 

## Retro Bargraphs