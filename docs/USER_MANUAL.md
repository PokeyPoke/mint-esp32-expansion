# MintySynth ESP32-S3 User Manual

## Getting Started

### First Power-On

1. Connect your ESP32-S3 to power via USB
2. The display should show "MintySynth ESP32-S3" 
3. You'll see the main interface with parameters and step grid

### Basic Operation

The MintySynth expansion eliminates the mode switching of the original. All controls are directly accessible:

## Control Layout

### 5 Rotary Encoders
- **Encoder 0 (Left)**: Tempo (60-200 BPM)
- **Encoder 1**: Pitch/Note (C1-C7)
- **Encoder 2**: Note Length (10-100%)
- **Encoder 3**: Envelope Type (0-4)
- **Encoder 4 (Right)**: Swing (0-50%)

### Encoder Push Functions
- **Tempo Push**: Tap tempo (future feature)
- **Pitch Push**: Jump to next octave
- **Length Push**: Quantize to 25/50/75/100%
- **Envelope Push**: Cycle through envelope types
- **Swing Push**: Reset swing to 0%

### 4x4 Step Matrix
Press any of the 16 buttons to:
- **Select that step** for editing
- **Toggle the step** on/off
- **See visual feedback** on display

Active steps show as green squares, current playing step is red.

### 4 Red Control Buttons
- **Button 1**: PLAY/STOP sequencer
- **Button 2**: Select Voice (1-4)
- **Button 3**: Clear current step
- **Button 4**: Copy step (future feature)

## Programming Sequences

### Basic Step Programming

1. **Select a step**: Press any step button (1-16)
2. **Set the note**: Turn Pitch encoder
3. **Adjust timing**: Turn Length encoder
4. **Shape the sound**: Turn Envelope encoder
5. **Repeat** for other steps

### Multi-Voice Programming

1. Press **Voice Select** button to cycle through voices 1-4
2. Program each voice independently
3. Each voice can have different patterns
4. All voices play simultaneously

### Real-Time Performance

- **Turn encoders** while playing to hear changes instantly
- **Press step buttons** to toggle steps on/off during playback
- **Change voices** to edit different parts while playing
- **Adjust tempo** for dramatic effects

## Display Information

The screen shows:

### Top Section
- Current parameter values (Tempo, Pitch, Length, Envelope, Swing)
- Play status (PLAYING/STOPPED)
- Current voice and step numbers

### Step Grid
- 16 squares representing each step
- **Gray**: Step inactive
- **Green**: Step active  
- **Red**: Currently playing step
- **Numbers**: Step position (1-16)

### Bottom Section
- Current step's note name (C4, F#3, etc.)
- Additional parameter information

## Audio Features

### Synthesis Engine
- Based on original MintySynth wavetable synthesis
- 4 independent voices
- Multiple waveforms and envelopes
- Real-time parameter control

### Audio Output
- High-quality I2S digital audio
- Stereo output via 3.5mm jack
- Low latency for responsive performance

## Advanced Features

### Tempo and Timing
- **Tempo**: 60-200 BPM range
- **Swing**: Adds groove by delaying even steps
- **16th Note Resolution**: Precise timing control

### Sound Shaping
- **5 Envelope Types**: Attack, Decay, Pluck, Long, Reverse
- **Multiple Waveforms**: Sine, Square, Saw, Triangle, Noise, and more
- **Length Control**: 10-100% note duration

### Voice Management
- **4 Independent Voices**: Each with its own sequence
- **Voice Selection**: Switch between voices instantly
- **Polyphonic Playback**: All voices play simultaneously

## Troubleshooting

### No Display
- Check TFT connections
- Verify power supply
- Check TFT_eSPI library configuration

### No Audio
- Verify I2S DAC connections
- Check audio jack wiring
- Ensure DAC is powered

### Unresponsive Controls
- Check encoder wiring
- Verify pull-up resistors
- Test matrix keyboard connections

### Performance Issues
- Reduce display update rate
- Check for loose connections
- Verify adequate power supply

## Tips for Best Performance

1. **Start Simple**: Program a basic 4-step pattern first
2. **Use Different Voices**: Try bass on voice 1, melody on voice 2
3. **Experiment with Swing**: Small amounts (10-20%) add groove
4. **Layer Envelopes**: Mix pluck and long envelopes
5. **Real-time Changes**: Adjust parameters while playing

## Specifications

- **Polyphony**: 4 voices
- **Sequencer**: 16 steps per voice
- **Tempo Range**: 60-200 BPM
- **Audio**: 16-bit/44.1kHz stereo I2S
- **Latency**: <10ms end-to-end
- **Display**: 320x240 color TFT
- **Controls**: 5 encoders + 20 buttons

## Care and Maintenance

- **Power**: Use quality USB cable and power supply
- **Storage**: Keep in dry environment
- **Cleaning**: Use soft cloth for display
- **Connections**: Check wiring periodically

This manual covers the basic operation. The MintySynth ESP32-S3 expansion provides professional-grade sequencing with the simplicity and immediacy that the original lacked.