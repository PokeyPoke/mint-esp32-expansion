# ESP32-S3-WROOM-1 Safe Pin Allocation

## Conservative GPIO Assignment for ESP32-S3-WROOM-1

Based on common ESP32-S3-WROOM-1 documentation, using only the most widely confirmed GPIO pins:

### CONFIRMED SAFE GPIO PINS:
These pins are commonly available and safe to use on most ESP32-S3-WROOM-1 variants:
- GPIO 1-11, 12-18, 21, 35-42, 47, 48

### CONSERVATIVE PIN ALLOCATION:

#### Display Interface (SPI) - Standard pins
| ESP32-S3 Pin | Function | TFT Pin |
|--------------|----------|---------|
| GPIO 12 | MISO | MISO |
| GPIO 11 | MOSI | MOSI |
| GPIO 10 | SCK | SCK |
| GPIO 9 | CS | CS |
| GPIO 8 | DC | DC |
| GPIO 18 | RST | RST |

#### Rotary Encoders - Using safe lower GPIO numbers
| Encoder | CLK | DT | SW | Function |
|---------|-----|----|----|----------|
| 0 | GPIO 1 | GPIO 2 | GPIO 3 | Tempo |
| 1 | GPIO 4 | GPIO 5 | GPIO 6 | Pitch |
| 2 | GPIO 7 | GPIO 13 | GPIO 14 | Length |
| 3 | GPIO 15 | GPIO 16 | GPIO 17 | Envelope |
| 4 | GPIO 35 | GPIO 36 | GPIO 37 | Swing |

#### Matrix Keyboard - Using confirmed GPIO numbers
| Function | Pin |
|----------|-----|
| Row 1 | GPIO 38 |
| Row 2 | GPIO 39 |
| Row 3 | GPIO 40 |
| Row 4 | GPIO 41 |
| Col 1 | GPIO 42 |
| Col 2 | GPIO 47 |
| Col 3 | GPIO 48 |
| Col 4 | GPIO 21 |

#### Direct Buttons - Safe pins
| Button | Pin | Function |
|--------|-----|----------|
| 1 | GPIO 1 | PLAY/STOP |
| 2 | GPIO 2 | VOICE SELECT |
| 3 | GPIO 4 | CLEAR STEP |
| 4 | GPIO 5 | COPY STEP |

Wait, I have conflicts in this allocation. Let me revise...

## REVISED CONSERVATIVE ALLOCATION:

#### Display Interface (Uses GPIO 8-12, 18)
- MISO: GPIO 12
- MOSI: GPIO 11  
- SCK: GPIO 10
- CS: GPIO 9
- DC: GPIO 8
- RST: GPIO 18

#### Rotary Encoders (Uses GPIO 1-7, 13-17)
- Encoder 0: GPIO 1,2,3 (Tempo)
- Encoder 1: GPIO 4,5,6 (Pitch)  
- Encoder 2: GPIO 7,13,14 (Length)
- Encoder 3: GPIO 15,16,17 (Envelope)
- Encoder 4: GPIO 35,36,37 (Swing)

#### Matrix Keyboard (Uses GPIO 38-42, 47-48, 21)
- Rows: GPIO 38,39,40,41
- Cols: GPIO 42,47,48,21

#### Direct Buttons (Need 4 more pins)
Could you please verify which GPIO pins are actually available on your ESP32-S3-WROOM-1 module? 

The safest approach would be for you to:
1. Check the physical pinout of your specific board
2. List which GPIO pins are broken out
3. Let me create a pin allocation based on your actual hardware

This will ensure we use only pins that definitely exist on your module.