# ESP32-S3 VaporSynth Hardware Design - Dual Matrix Edition

## Overview
This design uses two 16-channel analog multiplexers (CD74HC4067) to manage dual 4×4 matrix keyboards, providing 32 programmable buttons with clean signal separation. Encoders are connected directly to GPIO for optimal responsiveness.

## Pin Allocation Summary
- **Total GPIO Used:** 23 pins (8 for multiplexers + 15 for encoders)
- **Total Buttons:** 32 (2× 4×4 matrices via multiplexers)
- **Total Encoders:** 5 (direct GPIO connection)
- **Available Expansion:** 11+ spare GPIO pins

## Core System Pins

### I2S Audio Output (3 pins) - Dedicated
| Signal | GPIO | Description |
|--------|------|-------------|
| BCLK   | 12   | Bit Clock |
| LRCLK  | 45   | Left/Right Clock (Word Select) |
| DOUT   | 0    | Digital Audio Data Output |

### TFT Display SPI (5 pins) - Dedicated  
| Signal | GPIO | Description |
|--------|------|-------------|
| CS     | 10   | Chip Select |
| DC     | 9    | Data/Command |
| RST    | 14   | Reset |
| MOSI   | 11   | Data Out |
| CLK    | 13   | Clock |

## Multiplexer Control System

### Shared Address Bus (4 pins)
| Signal | GPIO | Description |
|--------|------|-------------|
| A0     | 4    | Address bit 0 (LSB) |
| A1     | 5    | Address bit 1 |
| A2     | 6    | Address bit 2 |
| A3     | 7    | Address bit 3 (MSB) |

### Multiplexer Signal Pins (4 pins)
| Signal | GPIO | Description |
|--------|------|-------------|
| SIG_A  | 8    | Matrix 1 Data (Step Sequencer) |
| EN_A   | 16   | Matrix 1 Enable |
| SIG_B  | 15   | Matrix 2 Data (Function Keys) |
| EN_B   | 17   | Matrix 2 Enable |

## Direct GPIO Encoder Connections (15 pins)

### Rotary Encoders
| Encoder | Function | CLK | DT | SW | Description |
|---------|----------|-----|----|----| ------------|
| 1 | Tempo    | 1  | 2  | 3  | BPM control (40-400) |
| 2 | Pitch    | 18 | 19 | 20 | Transpose/Scale select |
| 3 | Length   | 21 | 35 | 36 | Note duration |
| 4 | Envelope | 37 | 38 | 39 | ADSR parameters |
| 5 | Swing    | 40 | 41 | 42 | Timing variation |

## Multiplexer Channel Assignments

### Multiplexer A: Matrix 1 - Step Sequencer (4×4)
| Channel | Function | Physical Position |
|---------|----------|-------------------|
| 0       | Step 1   | Row 1, Col 1 |
| 1       | Step 2   | Row 1, Col 2 |
| 2       | Step 3   | Row 1, Col 3 |
| 3       | Step 4   | Row 1, Col 4 |
| 4       | Step 5   | Row 2, Col 1 |
| 5       | Step 6   | Row 2, Col 2 |
| 6       | Step 7   | Row 2, Col 3 |
| 7       | Step 8   | Row 2, Col 4 |
| 8       | Step 9   | Row 3, Col 1 |
| 9       | Step 10  | Row 3, Col 2 |
| 10      | Step 11  | Row 3, Col 3 |
| 11      | Step 12  | Row 3, Col 4 |
| 12      | Step 13  | Row 4, Col 1 |
| 13      | Step 14  | Row 4, Col 2 |
| 14      | Step 15  | Row 4, Col 3 |
| 15      | Step 16  | Row 4, Col 4 |

### Multiplexer B: Matrix 2 - Function Keys (4×4)
| Channel | Function | Description | Physical Position |
|---------|----------|-------------|-------------------|
| 0  | Voice: BASS  | Select bass voice     | Row 1, Col 1 |
| 1  | Voice: LEAD  | Select lead voice     | Row 1, Col 2 |
| 2  | Voice: PAD   | Select pad voice      | Row 1, Col 3 |
| 3  | Voice: PERC  | Select percussion     | Row 1, Col 4 |
| 4  | Mode: LIVE   | Live performance mode | Row 2, Col 1 |
| 5  | Mode: PROG   | Program sequences     | Row 2, Col 2 |
| 6  | Mode: MIXER  | Mix voice volumes     | Row 2, Col 3 |
| 7  | Mode: SCALE  | Select musical scale  | Row 2, Col 4 |
| 8  | Pattern SAVE | Save current pattern  | Row 3, Col 1 |
| 9  | Pattern LOAD | Load saved pattern    | Row 3, Col 2 |
| 10 | Pattern COPY | Copy to clipboard     | Row 3, Col 3 |
| 11 | Pattern PASTE| Paste from clipboard  | Row 3, Col 4 |
| 12 | PLAY/STOP    | Start/stop sequencer  | Row 4, Col 1 |
| 13 | MUTE ALL     | Mute/unmute voices    | Row 4, Col 2 |
| 14 | FILL         | Fill current pattern  | Row 4, Col 3 |
| 15 | RANDOM       | Randomize pattern     | Row 4, Col 4 |

## Hardware Components Required

### Multiplexers
- **2× CD74HC4067** 16-channel analog multiplexers
- **Supply:** 3.3V operation
- **Logic Level:** 3.3V compatible
- **Package:** SOIC-24 or DIP-24

### Input Components
- **Matrix 1:** 4×4 tactile switch matrix (16 buttons for step sequencer)
- **Matrix 2:** 4×4 tactile switch matrix (16 function buttons)
- **Rotary Encoders:** 5× EC11 encoders with push buttons (direct GPIO)
- **Pull-up Resistors:** Internal ESP32-S3 pull-ups used for all inputs

## Advantages of Dual Matrix Design

### Superior Control Layout
- **32 dedicated buttons** - Clear separation between steps and functions
- **Logical grouping** - All voice controls together, all modes together
- **Direct encoder access** - No multiplexer delay for real-time control
- **Professional workflow** - Dedicated buttons for copy/paste, fill, random

### Performance Benefits
- **Clean multiplexer mapping** - Each multiplexer handles exactly one 4×4 matrix
- **Optimized scanning** - Matrix buttons scanned at 1kHz, encoders read directly
- **Dedicated I2S audio** - No GPIO conflicts with control inputs

### Expandability
- **11+ spare GPIO pins** - Room for MIDI, SD card, sensors
- **Scalable design** - Easy to add more multiplexers for analog controls
- **Future-proof** - Professional architecture allows growth

### Reliability
- **Clean signal separation** between audio and control
- **Reduced crosstalk** and interference
- **Better electromagnetic compatibility (EMC)**

### Cost Efficiency
- **~$2 total cost** for both multiplexers
- **Frees up expensive GPIO pins** for other uses
- **Reduces PCB complexity** (fewer direct traces)

## Expansion Possibilities

### Available Spare GPIO Pins (11 total)
GPIO 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32

Note: Some GPIO pins (34+ on certain ESP32-S3 variants) may have restrictions

### Potential Future Additions
- **Analog controls:** Potentiometers, sliders via ADC multiplexer
- **Additional displays:** Secondary OLED, LED matrices
- **Connectivity:** MIDI I/O, USB host, WiFi antenna
- **Sensors:** Accelerometer, ambient light, temperature
- **Storage:** SD card interface

## Implementation Notes

### Scanning Strategy
1. **Matrix (Fast):** 1kHz scan rate for responsive step input
2. **Encoders (Medium):** 500Hz scan rate for smooth rotation
3. **Direct Buttons (Immediate):** Interrupt-driven for critical functions

### Debouncing
- **Hardware:** Optional RC filters on multiplexer inputs
- **Software:** Individual debounce timers per channel
- **Encoder:** Quadrature decoding with direction validation

### Power Considerations
- **Multiplexer current:** ~1mA each (2mA total)
- **Input protection:** Internal ESP32-S3 pull-ups (45kΩ)
- **Signal integrity:** Short traces, proper grounding

## Physical Layout Diagram

```
┌─────────────────────────────────────────────────┐
│               2.8" TFT DISPLAY                  │
│                 (320×240)                       │
├─────────────────────────────────────────────────┤
│  [ENC1]  [ENC2]  [ENC3]  [ENC4]  [ENC5]        │
│  TEMPO   PITCH  LENGTH   ENV    SWING          │
├─────────────────────────────────────────────────┤
│        MATRIX 1 - STEPS        MATRIX 2 - FUNC │
│  [ 1][ 2][ 3][ 4]      [BASS][LEAD][PAD][PERC] │
│  [ 5][ 6][ 7][ 8]      [LIVE][PROG][MIX][SCAL] │
│  [ 9][10][11][12]      [SAVE][LOAD][COPY][PST] │
│  [13][14][15][16]      [PLAY][MUTE][FILL][RND] │
│         ↑                        ↑              │
│   Multiplexer A            Multiplexer B        │
└─────────────────────────────────────────────────┘
```

## Wiring Quick Reference

### Matrix 1 Wiring (Step Sequencer)
- Each button connects multiplexer channel to GND
- Channels 0-15 → Steps 1-16
- Enable: GPIO 16, Signal: GPIO 8

### Matrix 2 Wiring (Function Keys)
- Each button connects multiplexer channel to GND
- Channels 0-15 → Function buttons (see table above)
- Enable: GPIO 17, Signal: GPIO 15

### Encoder Wiring (Direct GPIO)
- All encoder pins use internal pull-ups
- CLK and DT for rotation detection
- SW for push button function
- See encoder table for specific GPIO assignments