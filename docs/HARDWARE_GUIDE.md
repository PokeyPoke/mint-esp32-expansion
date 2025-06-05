# Hardware Assembly Guide

## ESP32-S3 Pin Connections

### 2.8" TFT Display (ILI9341) - VERIFIED FOR YOUR BOARD
```
ESP32-S3          →    2.8" TFT Display
GPIO 10 (CS)      →    CS (Chip Select)
GPIO 11 (MOSI)    →    MOSI (Data)
GPIO 13 (SCK)     →    SCK (Clock)
GPIO 9 (DC)       →    DC (Data/Command)
GPIO 14 (RST)     →    RST (Reset)
3.3V              →    VCC
GND               →    GND
```

### 5 Rotary Encoders - VERIFIED FOR YOUR BOARD
```
Encoder 0 (Tempo):     GPIO 4,5,6    (CLK,DT,SW)
Encoder 1 (Pitch):     GPIO 7,15,16  (CLK,DT,SW)
Encoder 2 (Length):    GPIO 17,18,8  (CLK,DT,SW)
Encoder 3 (Envelope):  GPIO 1,2,42   (CLK,DT,SW)
Encoder 4 (Swing):     GPIO 41,40,39 (CLK,DT,SW)
```

### 4x4 Matrix Keyboard - VERIFIED FOR YOUR BOARD
```
Matrix Rows:    GPIO 38,37,36,35 → Rows 1-4
Matrix Columns: GPIO 48,47,21,46 → Cols 1-4
```

### Direct Buttons - OPTIMIZED (3 buttons)
```
GPIO 45 → PLAY/STOP
GPIO 3  → VOICE SELECT  
GPIO 12 → CLEAR/SHIFT (combined function)
```

### Audio Output (I2S) - VERIFIED FOR YOUR BOARD
```
GPIO 19 → I2S_BCLK (Bit Clock)
GPIO 20 → I2S_LRCLK (Left/Right Clock)
GPIO 0  → I2S_DOUT (Data Out) ⚠️ Boot sensitive pin
```

## Assembly Instructions

1. **Mount ESP32-S3 on breadboard**
2. **Connect TFT display using short wires**
3. **Wire rotary encoders with pull-up resistors**
4. **Connect matrix keyboard**
5. **Add I2S DAC module**
6. **Test each section before final assembly**

## YOUR ESP32-S3-WROOM-1 BOARD PINS VERIFIED

✅ **YOUR BOARD'S AVAILABLE PINS (34 total):**
```
4,5,6,7,15,16,17,18,8,3,46,9,10,11,12,13,14,1,2,42,41,40,39,38,37,36,35,0,45,48,47,21,20,19
```

⚠️ **BOOT-SENSITIVE PINS WE'RE USING:**
- **GPIO 0**: Used for I2S audio output - may need to disconnect during boot
- **GPIO 3**: Used as button input (safe with pull-up)
- **GPIO 45**: Used as button input (safe with pull-up)
- **GPIO 46**: Used for matrix keyboard (safe as output)

⚠️ **USB PINS WE'RE USING:**
- **GPIO 19**: I2S BCLK - disables USB debugging
- **GPIO 20**: I2S LRCLK - disables USB debugging

✅ **PIN ALLOCATION SUMMARY:**
- Display: 5 pins (9,10,11,13,14)
- Encoders: 15 pins (1,2,4,5,6,7,8,15,16,17,18,39,40,41,42)
- Matrix: 8 pins (21,35,36,37,38,46,47,48)
- Buttons: 3 pins (3,12,45)
- Audio: 3 pins (0,19,20)
- **TOTAL: 34 pins used (all available pins utilized)**

🔧 **BOOT WORKAROUND:** If GPIO 0 causes boot issues, temporarily disconnect I2S DOUT during power-up.

## Power Requirements

- **5V Input**: 2A capacity recommended
- **3.3V Logic**: Provided by ESP32 onboard regulator
- **Current Draw**: ~500mA typical operation