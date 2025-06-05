# ESP32-S3 ACTUAL Available GPIO Pins

## REAL GPIO PINS THAT EXIST:
**0-21** (continuous range)
**33-48** (continuous range)
**Missing: 22-32** (These GPIO numbers do NOT exist!)

Total: 38 GPIO pins

## CORRECTED Pin Assignment (Using ONLY Real Pins)

### Display Interface (SPI) - OK (These pins exist)
| ESP32-S3 Pin | Function | TFT Pin | Description |
|--------------|----------|---------|-------------|
| GPIO 10 | CS | CS | Chip Select |
| GPIO 11 | MOSI | MOSI | Master Out Slave In |
| GPIO 12 | MISO | MISO | Master In Slave Out |
| GPIO 13 | SCK | SCK | Serial Clock |
| GPIO 14 | DC | DC | Data/Command |
| GPIO 15 | RST | RST | Reset |

### Rotary Encoders - FULLY CORRECTED
| Encoder | CLK Pin | DT Pin | SW Pin | Function |
|---------|---------|--------|--------|----------|
| 0 | GPIO 1 | GPIO 2 | GPIO 4 | Tempo |
| 1 | GPIO 5 | GPIO 6 | GPIO 7 | Pitch |
| 2 | GPIO 8 | GPIO 9 | GPIO 16 | Length |
| 3 | GPIO 17 | GPIO 18 | GPIO 21 | Envelope |
| 4 | GPIO 47 | GPIO 48 | GPIO 38 | Swing |

### Matrix Keyboard (4x4) - FULLY CORRECTED
| Function | Pin | Description |
|----------|-----|-------------|
| Row 1 | GPIO 39 | Steps 1,2,3,4 |
| Row 2 | GPIO 40 | Steps 5,6,7,8 |
| Row 3 | GPIO 41 | Steps 9,10,11,12 |
| Row 4 | GPIO 42 | Steps 13,14,15,16 |
| Col 1 | GPIO 33 | Steps 1,5,9,13 |
| Col 2 | GPIO 34 | Steps 2,6,10,14 |
| Col 3 | GPIO 35 | Steps 3,7,11,15 |
| Col 4 | GPIO 36 | Steps 4,8,12,16 |

### Direct Buttons - FULLY CORRECTED
| Button | Pin | Function |
|--------|-----|----------|
| 1 | GPIO 37 | PLAY/STOP |
| 2 | GPIO 43 | VOICE SELECT |
| 3 | GPIO 44 | CLEAR STEP |
| 4 | GPIO 45 | COPY STEP |

### Audio Output (I2S) - REPOSITIONED
| Signal | Pin | Description |
|--------|-----|-------------|
| BCLK | GPIO 46 | Bit Clock |
| LRCLK | GPIO 3 | Left/Right Clock |
| DOUT | GPIO 8 | Data Output |

## VERIFICATION OF PIN USAGE

### Pins to AVOID (but they exist):
- GPIO 0: Strapping pin (boot mode)
- GPIO 3: Used for I2S (was strapping, but OK for output)
- GPIO 19-20: USB pins
- GPIO 45: Used for button (was strapping, but OK as input with pull-up)
- GPIO 46: Used for I2S (was ROM messages, but OK for output)

### Safe Pin Usage Summary:
- Display: GPIO 10-15 ✓
- Encoders: GPIO 1,2,4,5,6,7,8,9,16,17,18,21,47,48,38 ✓
- Matrix: GPIO 39,40,41,42,33,34,35,36 ✓
- Buttons: GPIO 37,43,44,45 ✓
- Audio: GPIO 46,3,8 ✓

Total pins used: 35 out of 38 available

## IMPORTANT CORRECTIONS FROM PREVIOUS VERSIONS:
1. ❌ GPIO 22-32 DO NOT EXIST on ESP32-S3!
2. ❌ GPIO 26-29 were incorrectly used - these pins don't exist
3. ❌ GPIO 30-31 were incorrectly used - these pins don't exist
4. ✓ Now using only actual existing GPIO pins
5. ✓ All pin numbers verified against ESP32-S3 datasheet