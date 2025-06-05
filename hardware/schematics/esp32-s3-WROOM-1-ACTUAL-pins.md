# ESP32-S3-WROOM-1 Actual Available Pins

## YOUR BOARD'S AVAILABLE GPIO PINS:
```
4, 5, 6, 7, 15, 16, 17, 18, 8, 3, 46, 9, 10, 11, 12, 13, 14, 1, 2, 42, 41, 40, 39, 38, 37, 36, 35, 0, 45, 48, 47, 21, 20, 19
```

**Total: 34 GPIO pins available**

## PINS TO AVOID (but available on your board):
- **GPIO 0**: Boot strapping pin - avoid using
- **GPIO 3**: Boot strapping pin - use carefully (OK as input with pull-up)
- **GPIO 19, 20**: USB pins - avoid if using USB debugging
- **GPIO 45, 46**: Strapping pins - use carefully

## OPTIMIZED PIN ALLOCATION FOR YOUR BOARD:

### Display Interface (SPI) - 6 pins
| Function | GPIO | TFT Pin |
|----------|------|---------|
| CS | 10 | CS |
| MOSI | 11 | MOSI |
| MISO | 12 | MISO |
| SCK | 13 | SCK |
| DC | 9 | DC |
| RST | 14 | RST |

### Rotary Encoders - 15 pins
| Encoder | CLK | DT | SW | Function |
|---------|-----|----|----|----------|
| 0 | 4 | 5 | 6 | Tempo |
| 1 | 7 | 15 | 16 | Pitch |
| 2 | 17 | 18 | 8 | Length |
| 3 | 1 | 2 | 42 | Envelope |
| 4 | 41 | 40 | 39 | Swing |

### Matrix Keyboard - 8 pins
| Function | GPIO |
|----------|------|
| Row 1 | 38 |
| Row 2 | 37 |
| Row 3 | 36 |
| Row 4 | 35 |
| Col 1 | 48 |
| Col 2 | 47 |
| Col 3 | 21 |
| Col 4 | 46 |

### Direct Buttons - 4 pins
| Button | GPIO | Function |
|--------|------|----------|
| 1 | 45 | PLAY/STOP |
| 2 | 3 | VOICE SELECT |
| 3 | 19 | CLEAR STEP |
| 4 | 20 | COPY STEP |

### Audio Output (I2S) - 1 pin remaining
We have a problem - only 1 pin left but need 3 for I2S.

## SOLUTION: Optimize pin usage

Let me redistribute to free up pins for I2S:

### REVISED ALLOCATION:

#### Use software SPI for display (frees up hardware SPI pins):
- CS: 10, MOSI: 11, SCK: 13, DC: 9, RST: 14 (5 pins)
- Remove MISO (not needed for display-only) - saves 1 pin

#### Reduce encoder count or use I2C encoder expander:
Option 1: Use 4 encoders instead of 5
Option 2: Use I2C GPIO expander for some controls

#### Final I2S allocation with freed pins:
- BCLK: 12 (freed from display MISO)
- LRCLK: 46 (moved from matrix)
- DOUT: 21 (moved from matrix)

#### Adjusted matrix (using 6 pins instead of 8):
- Use 3x2 matrix scanning or direct button reading

Would you prefer:
1. **4 encoders** instead of 5 (saves 3 pins)
2. **I2C GPIO expander** for matrix keyboard 
3. **Different control layout** to fit available pins

Which approach would you like me to implement?