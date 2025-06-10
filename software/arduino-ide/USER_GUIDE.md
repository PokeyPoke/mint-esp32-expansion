# VaporSynth ESP32-S3 - User Guide (Multiplexer Edition)

## ⚡ Hardware Overview - Dual Matrix Edition

VaporSynth uses **dual 4×4 matrix keyboards** for ultimate control:
- **32 programmable buttons** via 2× CD74HC4067 multiplexers
- **Matrix 1:** 16-step sequencer control
- **Matrix 2:** Voice selection, modes, patterns, and performance
- **5 direct-connected encoders** - no multiplexer delay
- **Dedicated I2S audio pins** - clean signal separation

### Required Hardware
- **ESP32-S3-WROOM-1** development board (8MB Flash recommended)
- **2× CD74HC4067** 16-channel analog multiplexers (SOIC-24 or DIP-24)
- **2× 4×4 button matrices** (32 tactile switches total)
- **2.8" ILI9341 TFT display** (320×240 pixels) 
- **PCM5102 I2S DAC module** for professional audio output
- **5× rotary encoders** with push buttons (EC11 compatible)

---

## 🚀 QUICK HITS - Start Making Music in 30 Seconds!

### Power On & Play
1. **Power up** → VaporSynth boot screen with animated sun
2. **Press [PLAY]** (Matrix 2, Row 4, Col 1) → Demo song starts automatically
3. **Turn TEMPO** (Encoder 1) → Speed up/slow down
4. **Press [BASS]/[LEAD]/[PAD]/[PERC]** (Matrix 2, Row 1) → Select voice
5. **Turn SWING** (Encoder 5) → Add groove

### Record Your First Beat
1. **Press SWING encoder button** → Enable REC mode (red indicator)
2. **Press [PLAY]** → Start sequencer
3. **Hit Matrix 1 keys (steps 1-16)** → Record notes in real-time!
4. **Press SWING encoder again** → Disable recording

### Change Sounds
1. **Press [PROG]** (Matrix 2, Row 2, Col 2) → Enter programming mode
2. **Turn ENV encoder** → Cycle through 15 waveforms
3. **Press ENV encoder button** → Cycle ADSR parameters (ATK/DEC/SUS/REL)
4. **Turn encoder** → Adjust attack, decay, sustain, or release

### Quick Access Functions
- **Matrix 2 Row 1:** Voice selection ([BASS], [LEAD], [PAD], [PERC])
- **Matrix 2 Row 2:** Mode switching ([LIVE], [PROG], [MIX], [SCAL])
- **Matrix 2 Row 3:** Pattern management ([SAVE], [LOAD], [COPY], [PASTE])
- **Matrix 2 Row 4:** Performance controls ([PLAY], [MUTE], [FILL], [RND])
- **Encoder buttons:** Direct mode shortcuts and parameter cycling

---

## 🎹 Full User Manual

### Hardware Layout - Dual Matrix Edition

#### Physical Control Layout
```
┌─────────────────────────────────────────────────┐
│               2.8" TFT DISPLAY                  │
├─────────────────────────────────────────────────┤
│  [ENC1]  [ENC2]  [ENC3]  [ENC4]  [ENC5]        │
│  TEMPO   PITCH  LENGTH   ENV    SWING          │
├─────────────────────────────────────────────────┤
│      MATRIX 1 - STEPS      │  MATRIX 2 - FUNCS │
│  [ 1][ 2][ 3][ 4]         │ [BASS][LEAD][PAD][PERC]│
│  [ 5][ 6][ 7][ 8]         │ [LIVE][PROG][MIX][SCAL]│
│  [ 9][10][11][12]         │ [SAVE][LOAD][COPY][PST]│
│  [13][14][15][16]         │ [PLAY][MUTE][FILL][RND]│
└─────────────────────────────────────────────────┘
```

#### Core System Connections

**I2S Audio Output (Dedicated - No Conflicts!)**
```
ESP32-S3 Pin → PCM5102 DAC
GPIO 12      → BCK (Bit Clock)
GPIO 45      → LRCK (Left/Right Clock)  
GPIO 0       → DIN (Data Input)
```

**TFT Display SPI (Dedicated)**
```
ESP32-S3 Pin → 2.8" ILI9341 Display
GPIO 10      → CS (Chip Select)
GPIO 11      → MOSI (Data)
GPIO 13      → SCK (Clock)
GPIO 9       → DC (Data/Command)
GPIO 14      → RST (Reset)
```

**Multiplexer Control System**
```
ESP32-S3 Pin → Both CD74HC4067 Multiplexers
GPIO 4       → S0 (Address bit 0, shared)
GPIO 5       → S1 (Address bit 1, shared)
GPIO 6       → S2 (Address bit 2, shared)
GPIO 7       → S3 (Address bit 3, shared)
GPIO 8       → SIG (Multiplexer A - Matrix data)
GPIO 16      → E (Enable A - Matrix enable)
GPIO 15      → SIG (Multiplexer B - Encoder data)
GPIO 17      → E (Enable B - Control enable)
```

**Direct Buttons (Critical Functions)**
```
ESP32-S3 Pin → Function
GPIO 1       → PLAY/STOP Button
GPIO 2       → VOICE SELECT Button
GPIO 3       → CLEAR/SHIFT Button
```

#### Input Device Layout

**Rotary Encoders (5 total) - Direct GPIO Connection**
1. **TEMPO** (GPIO 1,2,3) - Controls playback speed (40-400 BPM)
2. **PITCH** (GPIO 18,19,20) - Controls pitch/transpose/scales
3. **LENGTH** (GPIO 21,35,36) - Controls note duration and ADSR
4. **ENV** (GPIO 37,38,39) - Controls ADSR envelope parameters
5. **SWING** (GPIO 40,41,42) - Controls timing swing (0-100%) and recording

**Matrix 1: Step Sequencer (4×4) - Via Multiplexer A**
- **Steps 1-16**: Step programming and live recording
- **Layout**: 4 rows × 4 columns for easy navigation
- **Functions**: Toggle steps, trigger preview notes

**Matrix 2: Function Keys (4×4) - Via Multiplexer B**
- **Row 1**: Voice selection (BASS, LEAD, PAD, PERC)
- **Row 2**: Mode switching (LIVE, PROG, MIX, SCAL)
- **Row 3**: Pattern management (SAVE, LOAD, COPY, PASTE)
- **Row 4**: Performance controls (PLAY, MUTE, FILL, RND)

#### Expansion Possibilities
With **11+ spare GPIO pins** available:
- MIDI I/O interface for external connection
- SD card storage for samples/patterns/songs
- LED matrices for enhanced visual feedback
- Additional displays (OLED status, 7-segment BPM)
- Sensors (accelerometer for gesture control)
- Connectivity modules (WiFi for MIDI over network)
- Additional multiplexers for analog faders/potentiometers

#### Display Features
- **Real-time waveform visualization**
- **Step sequencer grid with current position**
- **ADSR parameter display**
- **Voice indicators with volume levels**
- **Mode and status information**

## 🎮 Dual Matrix Button Guide

### Matrix 1: Step Sequencer (Left Side)
```
[ 1][ 2][ 3][ 4]    Steps 1-4
[ 5][ 6][ 7][ 8]    Steps 5-8  
[ 9][10][11][12]    Steps 9-12
[13][14][15][16]    Steps 13-16
```
**Functions:**
- **LIVE Mode:** Trigger preview notes, live recording when REC active
- **PROGRAM Mode:** Toggle sequence steps on/off for current voice
- **MIXER Mode:** Adjust individual voice parameters
- **SONG Mode:** Save patterns to slots 1-16

### Matrix 2: Function Keys (Right Side)
```
[BASS][LEAD][PAD][PERC]    Voice Selection (Row 1)
[LIVE][PROG][MIX][SCAL]    Mode Selection (Row 2)
[SAVE][LOAD][COPY][PST]    Pattern Management (Row 3)
[PLAY][MUTE][FILL][RND]    Performance Controls (Row 4)
```

#### Row 1 - Voice Selection
- **[BASS]:** Select bass voice (low-frequency sounds)
- **[LEAD]:** Select lead voice (melodic lines)  
- **[PAD]:** Select pad voice (sustained textures)
- **[PERC]:** Select percussion voice (drums, hits)

#### Row 2 - Mode Selection
- **[LIVE]:** Live performance mode - real-time playing/recording
- **[PROG]:** Program mode - edit sequences for current voice
- **[MIX]:** Mixer mode - adjust volumes and voice parameters
- **[SCAL]:** Scale mode - select musical scales

#### Row 3 - Pattern Management
- **[SAVE]:** Save current pattern to memory slot
- **[LOAD]:** Load pattern from memory slot
- **[COPY]:** Copy current pattern to clipboard
- **[PASTE]:** Paste pattern from clipboard

#### Row 4 - Performance Controls  
- **[PLAY]:** Start/stop the sequencer
- **[MUTE]:** Toggle mute all voices
- **[FILL]:** Fill current voice with all steps active
- **[RND]:** Randomize current voice pattern

## 🎵 Operating Modes

### LIVE Mode (Default)
**Quick Access**: Press TEMPO encoder button
- **Matrix Keys**: Play preview notes, live record when REC active
- **TEMPO**: Global playback speed
- **PITCH**: Global transpose (-24 to +24 semitones)  
- **SWING**: Global swing amount
- **VOICE SELECT**: Switch active voice
- **Live Recording**: Enable with SWING encoder button

### PROGRAM Mode (Per Voice)
**Quick Access**: Press CLEAR/SHIFT button
- **Matrix Keys**: Toggle sequence steps on/off for current voice
- **PITCH**: Transpose current voice
- **LENGTH**: Note duration for current voice
- **ENV + Button**: Cycle through ADSR parameters (ATK→DEC→SUS→REL)
- **ENV**: Adjust current ADSR parameter
- **VOICE SELECT**: Switch between voice program modes

### MIXER Mode
**Quick Access**: Press LENGTH encoder button
- **Keys 1-4**: Toggle voice mute/unmute
- **Keys 9-12**: Toggle per-voice swing for voices 1-4
- **TEMPO**: Still controls global tempo
- **Display**: Shows voice volumes and swing states

### SCALE Mode
**Quick Access**: Press PITCH encoder button
- **PITCH**: Select from 8 musical scales
- **Scales**: Chromatic, Major, Minor, Dorian, Phrygian, Lydian, Mixolydian, Pentatonic
- **All notes**: Automatically follow selected scale

## 🔥 Advanced Features

### ADSR Envelope Control
- **Press ENV encoder button** → Cycle: ATK → DEC → SUS → REL
- **Turn ENV encoder** → Adjust current parameter
- **Attack**: Note fade-in time (1-2000ms)
- **Decay**: Fade from peak to sustain (10-2000ms)
- **Sustain**: Held level (0-127)
- **Release**: Fade-out time when note ends (10-4000ms)

### Live Recording System
1. **Enable**: Press SWING encoder button (REC indicator appears)
2. **Record**: Play matrix keys while sequencer runs
3. **Real-time**: Notes record to current step automatically
4. **Toggle**: Press key on active step to erase
5. **Feedback**: Immediate audio when recording

### Per-Voice Swing
- **Global Swing**: SWING encoder affects all voices
- **Per-Voice**: In MIXER mode, keys 9-12 add individual swing
- **Musical Result**: Hi-hats can swing differently from kick drums
- **Non-blocking**: Uses advanced event scheduler for perfect timing

### Waveform Collection (15 Total)
1. **SINE** - Pure fundamental
2. **TRI** - Triangle wave
3. **SAW** - Sawtooth wave  
4. **SQR** - Square wave
5. **NOIS** - White noise
6. **RAMP** - Inverted sawtooth
7. **WAV-A** - 25% pulse wave
8. **WAV-B** - 12.5% pulse wave
9. **WAV-C** - Two-step square
10. **WAV-D** - Four-step staircase
11. **WAV-E** - Exponential decay
12. **WAV-F** - Half-rectified sine
13. **WAV-G** - Absolute sine
14. **WAV-H** - Harmonic series (odd harmonics)
15. **WAV-I** - FM synthesis style

## 🎼 Demo Song & Boot Sequence

### VaporSynth Boot Experience
1. **Visual**: Animated sun with rays, gradient background
2. **Audio**: Rising frequency sweep with reverb tail
3. **Text**: "VaporSynth (Based on MintySynth)"
4. **Duration**: 3-second immersive experience

### Auto-Demo Song
- **Starts**: Automatically after boot sequence
- **Pattern**: 4-voice demonstration with bass, lead, pad, percussion
- **Features**: Shows off ADSR envelopes, different waveforms, swing
- **Control**: Press any button to take control

## 🎯 Pro Tips

### Performance Workflow
1. **Start with demo** → Let it play to hear capabilities
2. **Take control** → Press PLAY/STOP to start/stop
3. **Switch voices** → VOICE SELECT to hear each part
4. **Live record** → Enable REC mode, play keys in time
5. **Tweak sounds** → Program mode for detailed editing

### Sound Design Secrets
- **Layer voices**: Different waveforms create rich textures
- **ADSR magic**: Long attack for pads, quick decay for percussion
- **Scale tricks**: Minor scales for dark moods, Pentatonic for simple melodies
- **Swing grooves**: Try 15-25% swing for shuffle feels
- **Per-voice swing**: Hi-hats swung, kick drums straight

### Pattern Building
1. **Foundation**: Start with kick on steps 1, 5, 9, 13
2. **Backbeat**: Add snare on steps 5, 13  
3. **Texture**: Hi-hats on off-beats (2, 4, 6, 8, 10, 12, 14, 16)
4. **Melody**: Use LEAD voice for melodic sequences
5. **Harmony**: Transpose voices to create chords

### Recording Techniques
- **Count-in**: Enable REC, wait for step 1, start playing
- **Overdub**: Record different voices on different passes
- **Erase**: Press keys on active steps to remove notes
- **Timing**: Don't worry about perfect timing - swing helps!

## 🛠️ Troubleshooting - Multiplexer Edition

### No Audio
- **I2S Connections**: Check BCK=GPIO12, LRCK=GPIO45, DATA=GPIO0
- **PCM5102 DAC**: Verify wiring and 3.3V power supply
- **Voice Levels**: Ensure voices aren't muted (check MIXER mode)
- **Test Audio**: Press CLEAR/SHIFT button for test tone

### Controls Not Working

**Encoders Not Responding**
- **Multiplexer B**: Check enable pin GPIO17 and signal pin GPIO15
- **Address Bus**: Verify GPIO4-7 connections to both multiplexers
- **Encoder Wiring**: Each encoder needs 3 channels (CLK, DT, SW)
- **Power**: Ensure multiplexers have 3.3V VCC and GND

**Matrix Keys Not Working**
- **Multiplexer A**: Check enable pin GPIO16 and signal pin GPIO8
- **Channel Assignment**: Verify tactile switches connect channels to GND
- **Address Selection**: Problem may be shared address bus (GPIO4-7)

**Direct Buttons Unresponsive**
- **GPIO Pins**: Check GPIO1 (PLAY), GPIO2 (VOICE), GPIO3 (CLEAR)
- **Pull-ups**: Buttons should connect pins to GND (internal pull-ups used)
- **Wiring**: Verify solid connections to button pins

### Multiplexer System Issues

**Partial Input Response**
- **Address Bus**: Check all 4 address lines (GPIO4-7) to both multiplexers
- **Enable Timing**: Software controls enable pins - hardware issue unlikely
- **Channel Crosstalk**: Use debouncing capacitors if needed

**Intermittent Operation**
- **Power Quality**: Add 0.1µF bypass capacitors near each multiplexer
- **Signal Integrity**: Keep multiplexer wires short and away from power traces
- **Grounding**: Ensure solid ground connections to all devices

### Display Issues
- **No Display**: Check SPI connections (GPIO9-11, GPIO13-14)
- **Wrong Colors**: Try display.invertDisplay(true/false)
- **Flickering**: Normal during audio processing - not a hardware issue

### Advanced Diagnostics
- **Serial Monitor**: Check for input scanning debug messages
- **Logic Analyzer**: Verify multiplexer address timing on GPIO4-7
- **Oscilloscope**: Check I2S clock signals (GPIO12, GPIO45)
- **Multimeter**: Verify 3.3V power distribution to all components

## 🎶 Musical Scales Reference

1. **CHROMATIC** - All 12 notes (no restrictions)
2. **MAJOR** - Happy, bright (C-D-E-F-G-A-B)
3. **MINOR** - Sad, dark (A-B-C-D-E-F-G)
4. **DORIAN** - Modal, jazzy (D-E-F-G-A-B-C)
5. **PHRYGIAN** - Spanish, exotic (E-F-G-A-B-C-D)
6. **LYDIAN** - Dreamy, floating (F-G-A-B-C-D-E)
7. **MIXOLYDIAN** - Bluesy, rock (G-A-B-C-D-E-F)
8. **PENTATONIC** - Simple, catchy (C-D-E-G-A)

---

## 🚀 Quick Reference Card

### Instant Actions
- **PLAY/STOP** → Start/stop music
- **VOICE SELECT** → Switch instruments  
- **TEMPO knob** → Speed up/down
- **SWING knob** → Add groove
- **Any encoder button** → Mode shortcuts

### Recording
- **SWING button** → Toggle REC mode
- **Matrix keys** → Record notes live
- **Same key twice** → Erase note

### Programming  
- **CLEAR/SHIFT** → Enter program mode
- **Matrix keys** → Toggle steps
- **ENV button** → Cycle ADSR params
- **ENV knob** → Adjust envelope

### Sound Design
- **ENV knob** → Change waveform (LIVE mode)
- **PITCH** → Transpose up/down
- **LENGTH** → Note duration
- **ADSR** → Shape note envelope

*VaporSynth ESP32-S3 Enhanced - The complete wavetable synthesizer experience!*