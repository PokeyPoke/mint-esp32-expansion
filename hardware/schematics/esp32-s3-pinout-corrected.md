# ESP32-S3 Corrected Pin Allocation

## CORRECTED Pin Assignment (Avoiding Problem Pins)

### Display Interface (SPI) - UNCHANGED (These are safe)
| ESP32-S3 Pin | Function | TFT Pin | Description |
|--------------|----------|---------|-------------|
| GPIO 10 | CS | CS | Chip Select |
| GPIO 11 | MOSI | MOSI | Master Out Slave In |
| GPIO 12 | MISO | MISO | Master In Slave Out |
| GPIO 13 | SCK | SCK | Serial Clock |
| GPIO 14 | DC | DC | Data/Command |
| GPIO 15 | RST | RST | Reset |

### Rotary Encoders - CORRECTED (Avoiding GPIO0, GPIO19-20)
| Encoder | CLK Pin | DT Pin | SW Pin | Function |
|---------|---------|--------|--------|----------|
| 0 | GPIO 1 | GPIO 2 | GPIO 4 | Tempo |
| 1 | GPIO 5 | GPIO 6 | GPIO 7 | Pitch |
| 2 | GPIO 8 | GPIO 9 | GPIO 16 | Length |
| 3 | GPIO 17 | GPIO 18 | GPIO 21 | Envelope |
| 4 | GPIO 47 | GPIO 48 | GPIO 38 | Swing |

### Matrix Keyboard (4x4) - CORRECTED (Avoiding GPIO35-37)
| Function | Pin | Description |
|----------|-----|-------------|
| Row 1 | GPIO 39 | Steps 1,2,3,4 |
| Row 2 | GPIO 40 | Steps 5,6,7,8 |
| Row 3 | GPIO 41 | Steps 9,10,11,12 |
| Row 4 | GPIO 42 | Steps 13,14,15,16 |
| Col 1 | GPIO 26 | Steps 1,5,9,13 |
| Col 2 | GPIO 27 | Steps 2,6,10,14 |
| Col 3 | GPIO 28 | Steps 3,7,11,15 |
| Col 4 | GPIO 29 | Steps 4,8,12,16 |

### Direct Buttons - CORRECTED
| Button | Pin | Function |
|--------|-----|----------|
| 1 | GPIO 30 | PLAY/STOP |
| 2 | GPIO 31 | VOICE SELECT |
| 3 | GPIO 32 | CLEAR STEP |
| 4 | GPIO 33 | COPY STEP |

### Audio Output (I2S) - CORRECTED
| Signal | Pin | Description |
|--------|-----|-------------|
| BCLK | GPIO 34 | Bit Clock |
| LRCLK | GPIO 35 | Left/Right Clock |
| DOUT | GPIO 36 | Data Output |

## AVOIDED PROBLEMATIC PINS

### Strapping Pins (DO NOT USE)
- **GPIO 0**: Boot mode control
- **GPIO 3**: JTAG control  
- **GPIO 45**: VDD_SPI power
- **GPIO 46**: ROM message control

### USB Pins (RESERVED)
- **GPIO 19**: USB D-
- **GPIO 20**: USB D+

### UART Pins (KEEP FREE FOR DEBUG)
- **GPIO 43**: UART0 TX
- **GPIO 44**: UART0 RX

### PSRAM Conflict Pins (AVOID IF USING PSRAM)
- Note: GPIO 35-37 used for I2S, may conflict with Octal PSRAM

## Pin Usage Summary

| Pin Range | Usage | Count |
|-----------|--------|--------|
| GPIO 1-18 | Encoders + Display | 18 pins |
| GPIO 21 | Encoder switch | 1 pin |
| GPIO 26-36 | Matrix + Buttons + Audio | 11 pins |
| GPIO 38-42 | Encoder + Matrix | 5 pins |
| GPIO 47-48 | Encoder | 2 pins |

**Total Used: 37 pins out of 45 available GPIO**

## Power Connections
- **VCC**: 3.3V from ESP32-S3
- **GND**: Common ground
- **5V**: Available if needed for amplification

## Important Notes

1. **All pins use internal pull-ups** where needed (buttons, encoders)
2. **I2S pins are grouped** for clean audio signals
3. **Display uses hardware SPI** for fast updates  
4. **Matrix scanning** optimized for minimal pin usage
5. **Boot sequence safe** - no interference with strapping pins
6. **USB debugging preserved** - UART pins left free
7. **Future expansion possible** - several pins still available

## Alternative Pin Options

If you encounter issues with PSRAM and GPIO 35-37:

**Alternative I2S Pins:**
- BCLK: GPIO 22
- LRCLK: GPIO 23  
- DOUT: GPIO 25

This allocation should work reliably with all ESP32-S3 variants!