# VaporSynth ESP32-S3 - Dual Matrix Edition

A professional wavetable synthesizer built on ESP32-S3 with **dual 4×4 matrix keyboards** for ultimate control and performance. Based on the original MintySynth project, enhanced with modern hardware design and professional workflow.

## Key Features

- **32 Dedicated Buttons** - Dual 4×4 matrices for steps and functions
- **Direct-Connected Encoders** - 5 encoders on GPIO for real-time response
- **Dedicated I2S Audio Pins** - Clean audio path with no conflicts
- **Professional Workflow** - Dedicated buttons for every function
- **4-Voice Wavetable Synthesis** - 15 unique waveforms with full ADSR envelopes
- **16-Step Sequencer** - Per-voice programming with live recording
- **Pattern Management** - Copy, paste, save, load, fill, and randomize

## Hardware Architecture

### Dual Matrix Design
- **ESP32-S3-WROOM-1** microcontroller (8MB Flash recommended)
- **2× CD74HC4067** 16-channel analog multiplexers
- **2× 4×4 Button Matrices** (32 tactile switches total)
- **Matrix 1:** Step sequencer control (steps 1-16)
- **Matrix 2:** Voice, mode, pattern, and performance functions
- **2.8" ILI9341 TFT Display** (320×240 pixels)
- **PCM5102 I2S DAC** for professional audio output
- **5× Rotary Encoders** with push buttons (direct GPIO)

### Advantages of Dual Matrix Design
- **Clear Separation** - Steps vs functions logically organized
- **No Encoder Delay** - Direct GPIO connection for real-time control
- **Professional Layout** - Every function has a dedicated button
- **Scalable Design** - Room for expansion with 11+ spare GPIO pins
- **Performance Optimized** - Dedicated I2S audio pins

## Features

### Enhanced Control Interface
- **Direct Step Access**: 16 buttons for immediate step programming
- **Rotary Encoders**: Precise control for tempo, pitch, length, envelope, and swing
- **Real-time Display**: Visual feedback for all parameters
- **Multi-page Interface**: Organized parameter groups

### Audio Improvements
- **I2S Digital Audio**: 16-bit/44.1kHz stereo output
- **Low Latency**: <10ms end-to-end audio processing
- **Professional Output**: Line level and headphone outputs

### User Experience
- **No Mode Switching**: Access all functions simultaneously
- **Visual Sequencer**: See all 16 steps and their states
- **Preset Management**: Save and recall complete configurations
- **Live Performance Ready**: Optimized for real-time use

## Project Structure

```
mint-esp32-expansion/
├── README.md                          # This file
├── docs/                              # Documentation
│   └── HARDWARE_GUIDE_MULTIPLEXER.md # Complete assembly guide
├── hardware/                          # Hardware design files
│   └── schematics/                    # Multiplexer pinout documentation
│       └── esp32-s3-multiplexer-pinout.md
├── software/                          # ESP32 Arduino code
│   └── arduino-ide/                   # Arduino IDE projects
│       ├── VaporSynth_Multiplexer.ino # Main multiplexer version
│       ├── MintySynth_ESP32_S3_Enhanced.ino # Enhanced direct GPIO version
│       └── USER_GUIDE.md              # Complete user manual
├── original/                          # Original MintySynth reference
└── tools/                             # Development utilities
```

## Getting Started

### Quick Start (5 minutes)
1. **Hardware**: Assemble multiplexer system per [Hardware Guide](docs/HARDWARE_GUIDE_MULTIPLEXER.md)
2. **Software**: Upload [VaporSynth_Multiplexer.ino](software/arduino-ide/VaporSynth_Multiplexer.ino)
3. **Power On**: Experience animated boot sequence with audio sweep
4. **Play**: Press PLAY/STOP button - demo song starts automatically!

### Complete Setup
1. **Pin Configuration**: Follow [Multiplexer Pinout](hardware/schematics/esp32-s3-multiplexer-pinout.md)
2. **Hardware Assembly**: Build dual multiplexer system per documentation
3. **Software Upload**: Flash VaporSynth_Multiplexer.ino to ESP32-S3
4. **User Manual**: Read [USER_GUIDE.md](software/arduino-ide/USER_GUIDE.md) for complete instructions

## Development Status

### ✅ Completed Features
- [x] **Dual multiplexer hardware architecture**
- [x] **Complete GPIO pin allocation (16 pins used, 18+ spare)**
- [x] **Professional I2S audio output implementation**
- [x] **Full 4-voice wavetable synthesis with 15 waveforms**
- [x] **Complete ADSR envelope system**
- [x] **16-step sequencer with per-voice programming**
- [x] **Live recording with real-time note capture**
- [x] **Per-voice swing with non-blocking event scheduler**
- [x] **320×240 TFT display with neon-themed UI**
- [x] **Multiple operating modes (LIVE, PROGRAM, MIXER, SCALE)**
- [x] **Save/Load pattern system**
- [x] **Comprehensive documentation and user manual**

### 🔄 Current Status
- **Hardware**: Fully designed and documented
- **Software**: Complete implementation ready for use
- **Documentation**: Comprehensive guides available
- **Testing**: Ready for real-world deployment

## Contributing

This project builds upon the excellent original MintySynth by Andrew Mowry. 

Original MintySynth: http://mintysynth.com
Original License: GPL v3

## License

This expanded version maintains the GPL v3 license of the original project.