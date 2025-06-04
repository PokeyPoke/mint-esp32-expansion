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

### 5 Rotary Encoders
```
Encoder 0 (Tempo):     GPIO 0,1,2   (CLK,DT,SW)
Encoder 1 (Pitch):     GPIO 3,4,5   (CLK,DT,SW)
Encoder 2 (Length):    GPIO 6,7,8   (CLK,DT,SW)
Encoder 3 (Envelope):  GPIO 9,16,17 (CLK,DT,SW)
Encoder 4 (Swing):     GPIO 18,19,20 (CLK,DT,SW)
```

### 4x4 Matrix Keyboard
```
Matrix Rows:    GPIO 26,27,28,29 → Rows 1-4
Matrix Columns: GPIO 30,31,32,33 → Cols 1-4
```

### Direct Buttons
```
GPIO 34 → PLAY/STOP
GPIO 35 → VOICE SELECT
GPIO 36 → CLEAR STEP
GPIO 37 → COPY STEP
```

### Audio Output (I2S)
```
GPIO 38 → I2S_BCLK (Bit Clock)
GPIO 39 → I2S_LRCLK (Left/Right Clock)
GPIO 40 → I2S_DOUT (Data Out)
```

## Assembly Instructions

1. **Mount ESP32-S3 on breadboard**
2. **Connect TFT display using short wires**
3. **Wire rotary encoders with pull-up resistors**
4. **Connect matrix keyboard**
5. **Add I2S DAC module**
6. **Test each section before final assembly**

## Power Requirements

- **5V Input**: 2A capacity recommended
- **3.3V Logic**: Provided by ESP32 onboard regulator
- **Current Draw**: ~500mA typical operation