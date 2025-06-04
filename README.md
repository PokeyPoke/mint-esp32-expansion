# MintySynth ESP32-S3 Expansion

A professional-grade expansion of the original MintySynth project, ported to ESP32-S3 with enhanced hardware interface and modern features.

## Project Overview

This project transforms the original MintySynth 4-voice wavetable sequencer into a professional synthesizer with:

- **ESP32-S3-WROOM-1** as the main controller
- **2.8" ILI9341 TFT Display** for visual interface
- **5 Rotary Encoders** for precise parameter control
- **4x4 Matrix Keyboard** for step sequencer programming
- **I2S Audio Output** for high-quality sound
- **No more mode switching** - all parameters accessible simultaneously

## Hardware Requirements

### Core Components
- 1x ESP32-S3-WROOM-1 Development Board
- 1x 2.8" ILI9341 SPI TFT Display (240x320)
- 5x Rotary Encoders (with push switches)
- 1x 4x4 Matrix Keyboard + 4 additional buttons
- 1x PCM5102A I2S DAC Module
- Various resistors and connecting wires

### Optional Components
- 16-channel analog multiplexers (for future expansion)
- 3.5mm and 1/4" audio jacks
- Custom 3D printed enclosure

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
├── README.md                 # This file
├── docs/                     # Documentation
│   ├── HARDWARE_GUIDE.md     # Wiring and assembly
│   ├── SOFTWARE_GUIDE.md     # Code structure and APIs
│   └── USER_MANUAL.md        # Operating instructions
├── hardware/                 # Hardware design files
│   ├── schematics/           # Wiring diagrams
│   ├── pcb/                  # PCB designs (future)
│   └── enclosure/            # 3D printable case files
├── software/                 # ESP32 Arduino code
│   ├── src/                  # Main source code
│   ├── lib/                  # Custom libraries
│   └── examples/             # Example sketches
├── original/                 # Original MintySynth reference
└── tools/                    # Development utilities
```

## Getting Started

1. **Hardware Assembly**: Follow the [Hardware Guide](docs/HARDWARE_GUIDE.md)
2. **Software Setup**: Install Arduino IDE with ESP32 support
3. **Upload Code**: Flash the main sketch to your ESP32-S3
4. **Calibration**: Run initial setup and calibration
5. **Play Music**: Start creating sequences!

## Development Status

- [x] Hardware architecture design
- [x] Pin allocation planning
- [x] Core software architecture
- [ ] Display interface implementation
- [ ] Encoder input handling
- [ ] Matrix keyboard scanning
- [ ] Audio synthesis engine port
- [ ] I2S audio output
- [ ] User interface development
- [ ] Preset management system
- [ ] Hardware testing and validation

## Contributing

This project builds upon the excellent original MintySynth by Andrew Mowry. 

Original MintySynth: http://mintysynth.com
Original License: GPL v3

## License

This expanded version maintains the GPL v3 license of the original project.