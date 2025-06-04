# ESP32-S3 Pin Allocation

## Complete Pin Assignment

### Display Interface (SPI)
| ESP32-S3 Pin | Function | TFT Pin | Description |
|--------------|----------|---------|-------------|
| GPIO 10 | CS | CS | Chip Select |
| GPIO 11 | MOSI | MOSI | Master Out Slave In |
| GPIO 12 | MISO | MISO | Master In Slave Out |
| GPIO 13 | SCK | SCK | Serial Clock |
| GPIO 14 | DC | DC | Data/Command |
| GPIO 15 | RST | RST | Reset |

### Rotary Encoders
| Encoder | CLK Pin | DT Pin | SW Pin | Function |
|---------|---------|--------|--------|----------|
| 0 | GPIO 0 | GPIO 1 | GPIO 2 | Tempo |
| 1 | GPIO 3 | GPIO 4 | GPIO 5 | Pitch |
| 2 | GPIO 6 | GPIO 7 | GPIO 8 | Length |
| 3 | GPIO 9 | GPIO 16 | GPIO 17 | Envelope |
| 4 | GPIO 18 | GPIO 19 | GPIO 20 | Swing |

### Matrix Keyboard (4x4)
| Function | Pin | Description |
|----------|-----|-------------|
| Row 1 | GPIO 26 | Steps 1,2,3,4 |
| Row 2 | GPIO 27 | Steps 5,6,7,8 |
| Row 3 | GPIO 28 | Steps 9,10,11,12 |
| Row 4 | GPIO 29 | Steps 13,14,15,16 |
| Col 1 | GPIO 30 | Steps 1,5,9,13 |
| Col 2 | GPIO 31 | Steps 2,6,10,14 |
| Col 3 | GPIO 32 | Steps 3,7,11,15 |
| Col 4 | GPIO 33 | Steps 4,8,12,16 |

### Direct Buttons
| Button | Pin | Function |
|--------|-----|----------|
| 1 | GPIO 34 | PLAY/STOP |
| 2 | GPIO 35 | VOICE SELECT |
| 3 | GPIO 36 | CLEAR STEP |
| 4 | GPIO 37 | COPY STEP |

### Audio Output (I2S)
| Signal | Pin | Description |
|--------|-----|-------------|
| BCLK | GPIO 38 | Bit Clock |
| LRCLK | GPIO 39 | Left/Right Clock |
| DOUT | GPIO 40 | Data Output |

### Available for Future Expansion
| Pin Range | Count | Potential Use |
|-----------|-------|---------------|
| GPIO 21-25 | 5 pins | Additional controls |
| GPIO 41-48 | 8 pins | MIDI, CV, extras |

## Power Connections
- **VCC**: 3.3V from ESP32-S3
- **GND**: Common ground
- **5V**: For audio amplification (if needed)

## Notes
- All encoder and button pins use internal pull-up resistors
- I2S uses dedicated audio pins for clean signal
- SPI display uses hardware SPI for fast updates
- Matrix scanning minimizes pin usage while providing 16 inputs