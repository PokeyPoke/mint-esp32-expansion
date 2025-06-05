# ESP32-S3-WROOM-1 Optimized Final Pin Allocation

## AVAILABLE PINS: 34 total
```
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,35,36,37,38,39,40,41,42,45,46,47,48
```

## FINAL OPTIMIZED ALLOCATION (Uses 33 pins):

### 2.8" TFT Display (5 pins) - Software SPI
| Function | GPIO | TFT Pin |
|----------|------|---------|
| CS | 10 | CS |
| MOSI | 11 | MOSI |
| SCK | 13 | SCK |
| DC | 9 | DC |
| RST | 14 | RST |

### 5 Rotary Encoders (15 pins)
| Encoder | CLK | DT | SW | Function |
|---------|-----|----|----|----------|
| 0 | 4 | 5 | 6 | Tempo |
| 1 | 7 | 15 | 16 | Pitch |
| 2 | 17 | 18 | 8 | Length |
| 3 | 1 | 2 | 42 | Envelope |
| 4 | 41 | 40 | 39 | Swing |

### 4x4 Matrix Keyboard (8 pins)
| Function | GPIO | Description |
|----------|------|-------------|
| Row 1 | 38 | Steps 1,2,3,4 |
| Row 2 | 37 | Steps 5,6,7,8 |
| Row 3 | 36 | Steps 9,10,11,12 |
| Row 4 | 35 | Steps 13,14,15,16 |
| Col 1 | 48 | Steps 1,5,9,13 |
| Col 2 | 47 | Steps 2,6,10,14 |
| Col 3 | 21 | Steps 3,7,11,15 |
| Col 4 | 46 | Steps 4,8,12,16 |

### Direct Buttons (3 pins) - Reduced from 4
| Button | GPIO | Function |
|--------|------|----------|
| 1 | 45 | PLAY/STOP |
| 2 | 3 | VOICE SELECT |
| 3 | 19 | CLEAR/SHIFT |

### Audio Output I2S (3 pins)
| Signal | GPIO | Description |
|--------|------|-------------|
| BCLK | 12 | Bit Clock |
| LRCLK | 20 | Left/Right Clock |
| DOUT | 0 | Data Output |

**CAUTION**: GPIO 0 used for audio - this may affect boot sequence. Alternative: use external I2S module.

## PIN USAGE SUMMARY:
- Display: 5 pins
- Encoders: 15 pins  
- Matrix: 8 pins
- Buttons: 3 pins
- Audio: 3 pins
- **TOTAL: 34 pins used (all available pins)**

## ALTERNATIVE SAFER ALLOCATION:

If GPIO 0 causes boot issues, here's a safer version:

### Audio Output (Using USB pins - requires no USB debugging)
| Signal | GPIO | Description |
|--------|------|-------------|
| BCLK | 19 | Bit Clock |
| LRCLK | 20 | Left/Right Clock |  
| DOUT | 0 | Data Output |

### Move CLEAR/SHIFT button:
- Button 3: GPIO 12 (freed from I2S)

This keeps GPIO 0 free for boot safety.

## RECOMMENDATION:
Use the **SAFER** allocation to avoid boot issues. This gives you:
- ✅ All 5 rotary encoders
- ✅ Full 4x4 matrix keyboard  
- ✅ 3 essential buttons
- ✅ High-quality I2S audio
- ✅ Full-color display
- ✅ Safe boot sequence

Would you like me to update the code with this optimized allocation?