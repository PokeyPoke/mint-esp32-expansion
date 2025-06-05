# Hardware Assembly Guide

## ESP32-S3 Pin Connections

### 2.8" TFT Display (ILI9341)
```
ESP32-S3          →    2.8" TFT Display
GPIO 10 (CS)      →    CS (Chip Select)
GPIO 11 (MOSI)    →    MOSI (Data)
GPIO 12 (MISO)    →    MISO (Data)
GPIO 13 (SCK)     →    SCK (Clock)
GPIO 14 (DC)      →    DC (Data/Command)
GPIO 15 (RST)     →    RST (Reset)
3.3V              →    VCC
GND               →    GND
```

### 5 Rotary Encoders - CORRECTED
```
Encoder 0 (Tempo):     GPIO 1,2,4   (CLK,DT,SW)
Encoder 1 (Pitch):     GPIO 5,6,7   (CLK,DT,SW)
Encoder 2 (Length):    GPIO 8,9,16  (CLK,DT,SW)
Encoder 3 (Envelope):  GPIO 17,18,21 (CLK,DT,SW)
Encoder 4 (Swing):     GPIO 47,48,38 (CLK,DT,SW)
```

### 4x4 Matrix Keyboard - CORRECTED
```
Matrix Rows:    GPIO 39,40,41,42 → Rows 1-4
Matrix Columns: GPIO 26,27,28,29 → Cols 1-4
```

### Direct Buttons - CORRECTED
```
GPIO 30 → PLAY/STOP
GPIO 31 → VOICE SELECT
GPIO 32 → CLEAR STEP
GPIO 33 → COPY STEP
```

### Audio Output (I2S) - CORRECTED
```
GPIO 34 → I2S_BCLK (Bit Clock)
GPIO 35 → I2S_LRCLK (Left/Right Clock)
GPIO 36 → I2S_DOUT (Data Out)
```

## Assembly Instructions

1. **Mount ESP32-S3 on breadboard**
2. **Connect TFT display using short wires**
3. **Wire rotary encoders with pull-up resistors**
4. **Connect matrix keyboard**
5. **Add I2S DAC module**
6. **Test each section before final assembly**

## IMPORTANT: Avoided Problematic Pins

⚠️ **These pins are NOT used to avoid conflicts:**
- **GPIO 0**: Boot mode control (strapping pin)
- **GPIO 3**: JTAG control (strapping pin)  
- **GPIO 19-20**: USB data pins
- **GPIO 43-44**: UART debug pins
- **GPIO 45-46**: Power/ROM control pins

✅ **Pin allocation verified safe for:**
- Normal boot sequence
- USB debugging
- PSRAM compatibility (with alternatives provided)
- No strapping pin conflicts

## Power Requirements

- **5V Input**: 2A capacity recommended
- **3.3V Logic**: Provided by ESP32 onboard regulator
- **Current Draw**: ~500mA typical operation