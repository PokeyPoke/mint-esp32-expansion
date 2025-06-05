# Hardware Assembly Guide

## ESP32-S3 Pin Connections

### 2.8" TFT Display (ILI9341)
```
ESP32-S3          →    2.8" TFT Display
GPIO 10 (CS)      →    CS (Chip Select)
GPIO 11 (MOSI)    →    MOSI (Data)
GPIO 12 (MISO)    →    MISO (Data)
GPIO 13 (SCK)     →    SCK (Clock)
GPIO 9 (DC)       →    DC (Data/Command)
GPIO 46 (RST)     →    RST (Reset)
3.3V              →    VCC
GND               →    GND
```

### 5 Rotary Encoders - FINAL VERSION
```
Encoder 0 (Tempo):     GPIO 4,5,6    (CLK,DT,SW)
Encoder 1 (Pitch):     GPIO 7,15,16  (CLK,DT,SW)
Encoder 2 (Length):    GPIO 17,18,8  (CLK,DT,SW)
Encoder 3 (Envelope):  GPIO 1,2,42   (CLK,DT,SW)
Encoder 4 (Swing):     GPIO 41,40,39 (CLK,DT,SW)
```

### 4x4 Matrix Keyboard - FINAL VERSION
```
Matrix Rows:    GPIO 21,47,48,14 → Rows 1-4
Matrix Columns: GPIO 38,37,36,35 → Cols 1-4
```

### Direct Buttons - FINAL VERSION
```
GPIO 34 → PLAY/STOP
GPIO 33 → VOICE SELECT
GPIO 45 → CLEAR STEP
GPIO 3  → COPY STEP
```

### Audio Output (I2S) - FINAL VERSION
```
GPIO 43 → I2S_BCLK (Bit Clock)
GPIO 44 → I2S_LRCLK (Left/Right Clock)
GPIO 20 → I2S_DOUT (Data Out)
```

## Assembly Instructions

1. **Mount ESP32-S3 on breadboard**
2. **Connect TFT display using short wires**
3. **Wire rotary encoders with pull-up resistors**
4. **Connect matrix keyboard**
5. **Add I2S DAC module**
6. **Test each section before final assembly**

## CRITICAL: ESP32-S3 GPIO Reality Check

⚠️ **GPIO PINS THAT DO NOT EXIST ON ESP32-S3:**
- **GPIO 22-32**: These pin numbers DO NOT EXIST! (11 missing pins)
- Available pins: GPIO 0-21 and GPIO 33-48 only (38 total pins)

⚠️ **Problematic pins we're carefully using:**
- **GPIO 0**: Boot strapping pin (NOT USED - avoided completely)
- **GPIO 3**: Used as button input (safe with pull-up)
- **GPIO 19**: USB D- (NOT USED - preserved for USB)
- **GPIO 20**: USB D+ (used for I2S audio - OK if not using USB)
- **GPIO 43-44**: Default UART (used for I2S - OK if not debugging)
- **GPIO 45-46**: Strapping pins (used carefully - GPIO 45 as input, GPIO 46 as output)

✅ **Pin allocation verified:**
- All pins confirmed to exist on ESP32-S3
- Boot sequence safe (GPIO 0 avoided)
- USB can be used if GPIO 20 is freed (alternative I2S pin available)
- Total pins used: 36 out of 38 available

## Power Requirements

- **5V Input**: 2A capacity recommended
- **3.3V Logic**: Provided by ESP32 onboard regulator
- **Current Draw**: ~500mA typical operation