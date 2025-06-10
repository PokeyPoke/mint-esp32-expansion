# VaporSynth ESP32-S3 - Hardware Guide (Dual Matrix Edition)

## Overview
VaporSynth is a professional 4-voice wavetable synthesizer built on ESP32-S3 with **dual 4×4 matrix keyboards** via multiplexers, providing 32 programmable buttons and direct-connected encoders for optimal performance.

### Key Advantages of Dual Matrix Design
- **32 dedicated buttons** - 16 for steps, 16 for functions
- **Direct encoder connection** - No multiplexer delay for real-time control
- **Dedicated I2S audio pins** - Clean audio path with no GPIO conflicts
- **Professional workflow** - Dedicated buttons for every function
- **11+ spare GPIO pins** - Room for future expansion

## Core Hardware Requirements

### ESP32-S3 Development Board
- **ESP32-S3-WROOM-1** or compatible
- **Minimum 8MB Flash**, 2MB PSRAM recommended
- **USB-C connector** for programming/debugging

### Multiplexer System
- **2× CD74HC4067** 16-channel analog multiplexers
- **SOIC-24 or DIP-24** packages
- **3.3V operation** (compatible with ESP32-S3 logic levels)

### Input Devices
- **2× 4×4 Matrix Keyboards** (32 tactile switches total)
  - Matrix 1: Step sequencer (steps 1-16)
  - Matrix 2: Function keys (voices, modes, patterns, performance)
- **5× Rotary Encoders** with push buttons (EC11 or compatible)
  - Direct GPIO connection for real-time response

### Display
- **2.8" ILI9341 TFT** 320×240 pixel color display
- **SPI interface** (dedicated pins)
- **Touch capability** optional

### Audio Output
- **PCM5102 I2S DAC** module
- **3.5mm stereo jack** for audio output
- **Line-level output** suitable for headphones/speakers

## Pin Assignment Summary

### GPIO Usage Overview
```
Total Available: 34 GPIO pins
Used for Multiplexers: 8 pins
Used for Encoders: 15 pins
Used for I2S/Display: 8 pins
Total Used: 31 pins
Available for Expansion: 3+ pins (varies by board)
```

### I2S Audio Output (3 pins - Dedicated)
```
ESP32-S3          →    PCM5102 DAC
GPIO 12 (BCLK)    →    BCK (Bit Clock)
GPIO 45 (LRCLK)   →    LRCK (Left/Right Clock)
GPIO 0 (DOUT)     →    DIN (Data Input)
3.3V              →    VDD
GND               →    GND
```

### TFT Display SPI (5 pins - Dedicated)
```
ESP32-S3          →    2.8" TFT Display
GPIO 10 (CS)      →    CS (Chip Select)
GPIO 11 (MOSI)    →    MOSI (Data)
GPIO 13 (CLK)     →    SCK (Clock)
GPIO 9 (DC)       →    DC (Data/Command)
GPIO 14 (RST)     →    RST (Reset)
3.3V              →    VCC
GND               →    GND
```

### Multiplexer Control System (8 pins)
```
ESP32-S3          →    Both CD74HC4067
GPIO 4 (A0)       →    S0 (Address bit 0)
GPIO 5 (A1)       →    S1 (Address bit 1)
GPIO 6 (A2)       →    S2 (Address bit 2)
GPIO 7 (A3)       →    S3 (Address bit 3)
GPIO 8 (SIG_A)    →    SIG (Multiplexer A - Matrix)
GPIO 16 (EN_A)    →    E (Enable A - Matrix)
GPIO 15 (SIG_B)   →    SIG (Multiplexer B - Encoders)
GPIO 17 (EN_B)    →    E (Enable B - Encoders)
3.3V              →    VCC (Both multiplexers)
GND               →    GND (Both multiplexers)
```

### Direct GPIO Encoders (15 pins - Real-time Control)
```
Encoder 1 (Tempo):    GPIO 1, 2, 3     (CLK, DT, SW)
Encoder 2 (Pitch):    GPIO 18, 19, 20  (CLK, DT, SW)
Encoder 3 (Length):   GPIO 21, 35, 36  (CLK, DT, SW)
Encoder 4 (Envelope): GPIO 37, 38, 39  (CLK, DT, SW)
Encoder 5 (Swing):    GPIO 40, 41, 42  (CLK, DT, SW)
```

## Multiplexer Channel Assignments

### Multiplexer A: Matrix 1 - Step Sequencer (4×4)
Connect 16 tactile switches between multiplexer channels and GND:

```
Channel    Matrix Position    Function
   0       Row 1, Col 1      Step 1
   1       Row 1, Col 2      Step 2
   2       Row 1, Col 3      Step 3
   3       Row 1, Col 4      Step 4
   4       Row 2, Col 1      Step 5
   5       Row 2, Col 2      Step 6
   6       Row 2, Col 3      Step 7
   7       Row 2, Col 4      Step 8
   8       Row 3, Col 1      Step 9
   9       Row 3, Col 2      Step 10
  10       Row 3, Col 3      Step 11
  11       Row 3, Col 4      Step 12
  12       Row 4, Col 1      Step 13
  13       Row 4, Col 2      Step 14
  14       Row 4, Col 3      Step 15
  15       Row 4, Col 4      Step 16
```

### Multiplexer B: Matrix 2 - Function Keys (4×4)
Connect 16 tactile switches between multiplexer channels and GND:

```
Channel    Matrix Position    Function          Label
   0       Row 1, Col 1      Voice: BASS       [BASS]
   1       Row 1, Col 2      Voice: LEAD       [LEAD]
   2       Row 1, Col 3      Voice: PAD        [PAD]
   3       Row 1, Col 4      Voice: PERC       [PERC]
   4       Row 2, Col 1      Mode: LIVE        [LIVE]
   5       Row 2, Col 2      Mode: PROGRAM     [PROG]
   6       Row 2, Col 3      Mode: MIXER       [MIX]
   7       Row 2, Col 4      Mode: SCALE       [SCAL]
   8       Row 3, Col 1      Pattern SAVE      [SAVE]
   9       Row 3, Col 2      Pattern LOAD      [LOAD]
  10       Row 3, Col 3      Pattern COPY      [COPY]
  11       Row 3, Col 4      Pattern PASTE     [PASTE]
  12       Row 4, Col 1      PLAY/STOP         [PLAY]
  13       Row 4, Col 2      MUTE ALL          [MUTE]
  14       Row 4, Col 3      FILL Pattern      [FILL]
  15       Row 4, Col 4      RANDOM Pattern    [RND]
```

## Physical Layout Suggestions

### Recommended Control Panel Layout
```
┌───────────────────────────────────────────────────┐
│                  2.8" TFT DISPLAY                 │
│                    (320×240)                      │
├───────────────────────────────────────────────────┤
│   [ENC1]   [ENC2]   [ENC3]   [ENC4]   [ENC5]     │
│   TEMPO    PITCH   LENGTH    ENV     SWING       │
├───────────────────────────────────────────────────┤
│       MATRIX 1 - STEPS      │  MATRIX 2 - FUNCS   │
│  [ 1][ 2][ 3][ 4]          │ [BASS][LEAD][PAD][PERC]│
│  [ 5][ 6][ 7][ 8]          │ [LIVE][PROG][MIX][SCAL]│
│  [ 9][10][11][12]          │ [SAVE][LOAD][COPY][PST]│
│  [13][14][15][16]          │ [PLAY][MUTE][FILL][RND]│
└───────────────────────────────────────────────────┤
```

### Enclosure Considerations
- **Minimum size**: 250mm × 180mm × 50mm (larger for dual matrices)
- **Front panel**: Aluminum or acrylic for durability
- **Button spacing**: 
  - 15mm centers within each 4×4 matrix
  - 20mm gap between the two matrices
- **Encoder spacing**: 30mm centers for comfortable operation
- **Display cutout**: 65mm × 50mm for 2.8" TFT
- **Matrix separation**: Clear visual/physical separation between step and function matrices

## Assembly Instructions

### Step 1: ESP32-S3 Setup
1. Mount ESP32-S3 on breadboard or custom PCB
2. Connect power supply (5V, 2A recommended)
3. Test basic functionality with simple blink sketch

### Step 2: Display Connection
1. Connect TFT display using short, shielded wires
2. Test display with graphics library examples
3. Verify colors and touch response (if applicable)

### Step 3: Audio System
1. Connect PCM5102 DAC module to I2S pins
2. Add 3.5mm stereo jack to DAC analog outputs
3. Test with simple sine wave generation

### Step 4: Multiplexer Installation
1. Mount both CD74HC4067 multiplexers on breadboard/PCB
2. Connect shared address bus (A0-A3) to ESP32-S3
3. Connect enable and signal pins
4. Add bypass capacitors (0.1µF) near each multiplexer

### Step 5: Input Devices
1. Wire matrix keyboard to Multiplexer A channels
2. Connect 5 rotary encoders to Multiplexer B channels
3. Wire 3 direct buttons to dedicated GPIO pins
4. Add pull-up resistors if not using internal pull-ups

### Step 6: Testing & Calibration
1. Upload VaporSynth firmware
2. Test each input systematically
3. Calibrate encoder sensitivity if needed
4. Verify audio output quality

## Bill of Materials (BOM)

### Core Components
| Component | Quantity | Description | Estimated Cost |
|-----------|----------|-------------|----------------|
| ESP32-S3-WROOM-1 | 1 | Development board | $8-12 |
| CD74HC4067 | 2 | 16-channel multiplexers | $1.50 each |
| ILI9341 TFT | 1 | 2.8" color display | $8-15 |
| PCM5102 | 1 | I2S DAC module | $3-5 |
| Rotary Encoders | 5 | EC11 or similar | $1 each |
| Tactile Switches | 32 | 6mm momentary | $0.10 each |
| 4×4 Button Matrix | 2 | Pre-made or DIY | $5-10 each |
| 3.5mm Jack | 1 | Stereo audio connector | $0.50 |

### Passive Components
| Component | Quantity | Description | Estimated Cost |
|-----------|----------|-------------|----------------|
| 0.1µF Capacitors | 10 | Ceramic bypass caps | $0.05 each |
| 10kΩ Resistors | 22 | Pull-up resistors | $0.02 each |
| Breadboard/PCB | 1 | Prototyping platform | $5-20 |

### **Total Estimated Cost: $55-85** (includes dual matrices)

## Available Expansion GPIO

With the dual matrix design, these GPIO pins remain available:
```
GPIO 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32
(11 additional pins, depending on ESP32-S3 variant)
```

Note: Some ESP32-S3 boards may have restrictions on higher GPIO numbers

### Expansion Possibilities
- **Additional multiplexers** for analog controls (potentiometers)
- **MIDI I/O** interface
- **SD card storage** for samples/patterns
- **LED matrices** for visual feedback
- **Additional displays** (OLED, 7-segment)
- **Sensors** (accelerometer, light, temperature)
- **Connectivity** (WiFi antenna, Bluetooth)

## Power Requirements

### Current Consumption
- **ESP32-S3**: ~150mA active, ~5mA sleep
- **TFT Display**: ~40mA (backlight on)
- **PCM5102 DAC**: ~10mA
- **Multiplexers**: ~1mA each
- **Total System**: ~200-250mA typical

### Power Supply Recommendations
- **Input**: 5V DC, 2A capacity (provides headroom)
- **USB-C**: Sufficient for development/testing
- **External**: Recommended for final installation
- **Battery**: 18650 Li-ion possible for portable use

## Troubleshooting

### Common Issues
1. **No audio output**: Check I2S wiring, DAC power
2. **Display not working**: Verify SPI connections, power
3. **Inputs not responding**: Check multiplexer enable pins
4. **Erratic encoder behavior**: Add debouncing capacitors
5. **Boot issues**: Ensure GPIO 0 isn't pulled low

### Debug Tools
- **Serial Monitor**: Check input scanning debug output  
- **Logic Analyzer**: Verify multiplexer address timing
- **Oscilloscope**: Check I2S clock signals
- **Multimeter**: Verify power distribution

## Performance Specifications

### Audio Quality
- **Sample Rate**: 20kHz (CD-quality possible)
- **Bit Depth**: 16-bit stereo
- **Latency**: <10ms input to audio output
- **THD+N**: <0.01% (limited by PCM5102)

### Control Responsiveness
- **Matrix Scan Rate**: 1kHz (1ms response)
- **Encoder Scan Rate**: 500Hz (2ms response)  
- **Direct Button**: Interrupt-driven (<1ms)
- **Display Update**: 20Hz (50ms refresh)

This dual multiplexer architecture provides a professional-grade foundation for the VaporSynth synthesizer while maintaining excellent expandability for future enhancements.