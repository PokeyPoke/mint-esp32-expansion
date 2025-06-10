/*
 * MintySynth ESP32-S3 Enhanced - Complete Feature Set
 * 
 * Enhanced version with all original MintySynth features plus modern improvements
 * Features neon-style UI with purple/teal theme
 * 
 * Original MintySynth by Andrew Mowry - ESP32-S3 Enhancement 2024
 * Licensed under GPL v3
 */

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <driver/i2s.h>
#include <Preferences.h>

// Display Configuration
#define TFT_CS   10
#define TFT_DC    9
#define TFT_RST  14
#define TFT_MOSI 11
#define TFT_CLK  13

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);

// Hardware Pin Definitions
const uint8_t MATRIX_ROWS[4] = {38, 37, 36, 35};
const uint8_t MATRIX_COLS[4] = {48, 47, 21, 46};
const uint8_t ENCODER_CLK[5] = {4, 7, 17, 1, 41};
const uint8_t ENCODER_DT[5] = {5, 15, 18, 2, 40};
const uint8_t ENCODER_SW[5] = {6, 16, 8, 42, 39};
const uint8_t DIRECT_BUTTONS[3] = {45, 3, 19};  // Button 1=GPIO45, Button 2=GPIO3, Button 3=GPIO19

// Audio Configuration
// Define SERIAL_AUDIO_DEBUG to hear audio values via Serial Monitor
// #define SERIAL_AUDIO_DEBUG  // Uncomment for audio debugging

// I2S Audio Configuration (for PCM5102)
#define I2S_NUM         I2S_NUM_0
#define I2S_BCK_PIN     12
#define I2S_WS_PIN      20  // LRCLK (Word Select) - moved from GPIO45 to GPIO20
#define I2S_DATA_PIN    0
#define SAMPLE_RATE     20000
#define I2S_BUFFER_SIZE 512

// Enhanced Color Palette - Neon Theme (Fixed for RGB565)
#define COLOR_BG        0x0000    // Pure black background
#define COLOR_ACCENT1   0x781F    // Bright purple (primary) - for title bar
#define COLOR_ACCENT2   0xF81F    // Bright magenta (secondary)
#define COLOR_ACCENT3   0x07FF    // Cyan (tertiary)
#define COLOR_TEXT      0xFFFF    // White text
#define COLOR_DIM       0x4208    // Dimmed purple
#define COLOR_WARNING   0xFD20    // Orange warning
#define COLOR_SUCCESS   0x07E0    // Green success
#define COLOR_STEP_ON   0xF81F    // Magenta for active steps
#define COLOR_STEP_OFF  0x4208    // Dim purple for inactive steps
#define COLOR_CURRENT   0x07FF    // Cyan for current step
#define COLOR_GRID      0x8410    // Medium gray grid lines

// Synthesis Constants
#define NUM_VOICES      4
#define NUM_STEPS       16
#define NUM_WAVEFORMS   15
#define NUM_SCALES      8
#define WAVETABLE_SIZE  256

// Waveform Types
enum Waveforms {
  WAVE_SINE = 0, WAVE_TRIANGLE, WAVE_SAWTOOTH, WAVE_SQUARE, WAVE_NOISE,
  WAVE_RAMP, WAVE_A, WAVE_B, WAVE_C, WAVE_D, WAVE_E, WAVE_F, WAVE_G, WAVE_H, WAVE_I
};

// Operating Modes
enum OperatingMode {
  MODE_LIVE = 0,      // Live performance mode
  MODE_PROGRAM_0,     // Program voice 0
  MODE_PROGRAM_1,     // Program voice 1
  MODE_PROGRAM_2,     // Program voice 2
  MODE_PROGRAM_3,     // Program voice 3
  MODE_MIXER,         // Volume mixing mode
  MODE_SCALE,         // Scale selection mode
  MODE_SONG           // Song management mode
};

// Scale Types
const char* scale_names[NUM_SCALES] = {
  "CHROMATIC", "MAJOR", "MINOR", "DORIAN", "PHRYGIAN", "LYDIAN", "MIXOLYDIAN", "PENTATONIC"
};

const int8_t scales[NUM_SCALES][12] = {
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},  // Chromatic
  {0, 2, 4, 5, 7, 9, 11, 0, 2, 4, 5, 7},   // Major
  {0, 2, 3, 5, 7, 8, 10, 0, 2, 3, 5, 7},   // Minor
  {0, 2, 3, 5, 7, 9, 10, 0, 2, 3, 5, 7},   // Dorian
  {0, 1, 3, 5, 7, 8, 10, 0, 1, 3, 5, 7},   // Phrygian
  {0, 2, 4, 6, 7, 9, 11, 0, 2, 4, 6, 7},   // Lydian
  {0, 2, 4, 5, 7, 9, 10, 0, 2, 4, 5, 7},   // Mixolydian
  {0, 2, 4, 7, 9, 0, 2, 4, 7, 9, 0, 2}     // Pentatonic
};

// Enhanced Encoder Structure
struct EncoderState {
  bool last_clk = HIGH;
  bool last_dt = HIGH;
  int position = 64;
  bool button = false;
  bool button_pressed = false;
  uint32_t last_change_time = 0;
  int8_t direction_buffer[4] = {0, 0, 0, 0};
  uint8_t buffer_index = 0;
  int8_t last_valid_direction = 0;
};

EncoderState encoders[5];

// ADSR Envelope stages
enum EnvelopeStage {
  ENV_ATTACK = 0,
  ENV_DECAY = 1,
  ENV_SUSTAIN = 2,
  ENV_RELEASE = 3,
  ENV_OFF = 4
};

// Enhanced Voice Structure with full ADSR
struct Voice {
  bool active = false;
  uint32_t phase_accumulator = 0;
  uint32_t frequency_tuning_word = 0;
  uint8_t waveform = WAVE_SINE;
  
  // ADSR Envelope System
  EnvelopeStage envelope_stage = ENV_OFF;
  uint32_t envelope_counter = 0;
  uint16_t current_amplitude = 0;      // Current envelope level (0-32767)
  
  // ADSR Parameters (0-127 each)
  uint8_t attack_time = 10;            // Attack time
  uint8_t decay_time = 30;             // Decay time  
  uint8_t sustain_level = 80;          // Sustain level (0-127)
  uint8_t release_time = 50;           // Release time
  
  // Note parameters
  uint8_t note = 60;
  uint16_t length = 500;               // Note duration in ms
  uint32_t note_start_time = 0;
  uint32_t note_release_time = 0;      // When release stage started
  bool note_released = false;          // Whether note is in release phase
  
  // Sequence data
  bool step_sequence[NUM_STEPS] = {false};
  uint8_t step_notes[NUM_STEPS];
  uint8_t volume = 100;         // 0-127
  int8_t transpose = 0;         // -24 to +24 semitones
  uint8_t swing_offset = 0;     // Per-voice swing
} voices[NUM_VOICES];

// Enhanced Sequencer Structure
struct Sequencer {
  bool playing = false;
  uint8_t current_step = 0;
  uint32_t last_step_time = 0;
  uint16_t step_length = 250;
  uint8_t swing = 0;
  OperatingMode mode = MODE_LIVE;
  uint8_t current_voice = 0;
  uint8_t current_scale = 0;
  int8_t song_transpose = 0;    // Global transpose
  uint8_t current_song = 0;     // 0-3 for saved songs
  bool record_mode = false;
} sequencer;

// UI State
struct UIState {
  bool needs_full_redraw = true;
  uint32_t last_blink = 0;
  bool blink_state = false;
  int8_t menu_selection = 0;
  bool in_menu = false;
  uint32_t last_activity = 0;
  
  // Fun animations
  float record_angle = 0.0;
  uint32_t last_record_update = 0;
  int16_t waveform_history[80] = {0};  // For waveform display
  uint8_t waveform_index = 0;
  uint32_t last_waveform_update = 0;
  
  // Dancing person animation
  uint8_t dance_frame = 0;
  uint32_t last_dance_update = 0;
} ui;

// Input States
bool matrix_keys[16] = {false};
bool last_matrix_keys[16] = {false};
bool direct_button_state[3] = {false};
bool last_direct_button_state[3] = {false};

// ADSR parameter selection
uint8_t current_adsr_param = 0; // 0=Attack, 1=Decay, 2=Sustain, 3=Release
const char* adsr_param_names[4] = {"ATK", "DEC", "SUS", "REL"};

// Non-blocking Event Scheduler for Per-Voice Swing
#define MAX_SCHEDULED_EVENTS 16
struct ScheduledEvent {
  bool active = false;
  uint32_t trigger_time = 0;
  uint8_t voice = 0;
  uint8_t note = 60;
};

ScheduledEvent event_queue[MAX_SCHEDULED_EVENTS];

// Audio
int16_t audio_buffer[I2S_BUFFER_SIZE * 2];
Preferences preferences;

// Voice Names with Neon Style
const char* voice_names[4] = {"BASS", "LEAD", "PAD", "PERC"};
const char* encoder_names[5] = {"TEMPO", "PITCH", "LENGTH", "ENV", "SWING"};
const char* waveform_names[NUM_WAVEFORMS] = {
  "SINE", "TRI", "SAW", "SQR", "NOIS", "RAMP", "WAV-A", "WAV-B", "WAV-C", 
  "WAV-D", "WAV-E", "WAV-F", "WAV-G", "WAV-H", "WAV-I"
};

// Enhanced wavetables
const int16_t wavetable_sine[WAVETABLE_SIZE] PROGMEM = {
  0, 804, 1608, 2410, 3212, 4011, 4808, 5602, 6393, 7179, 7962, 8739, 9512, 10278, 11039, 11793,
  12539, 13279, 14010, 14732, 15446, 16151, 16846, 17530, 18204, 18868, 19519, 20159, 20787, 21403, 22005, 22594,
  23170, 23731, 24279, 24811, 25329, 25832, 26319, 26790, 27245, 27683, 28105, 28510, 28898, 29268, 29621, 29956,
  30273, 30571, 30852, 31113, 31356, 31580, 31785, 31971, 32137, 32285, 32412, 32521, 32609, 32678, 32728, 32757,
  32767, 32757, 32728, 32678, 32609, 32521, 32412, 32285, 32137, 31971, 31785, 31580, 31356, 31113, 30852, 30571,
  30273, 29956, 29621, 29268, 28898, 28510, 28105, 27683, 27245, 26790, 26319, 25832, 25329, 24811, 24279, 23731,
  23170, 22594, 22005, 21403, 20787, 20159, 19519, 18868, 18204, 17530, 16846, 16151, 15446, 14732, 14010, 13279,
  12539, 11793, 11039, 10278, 9512, 8739, 7962, 7179, 6393, 5602, 4808, 4011, 3212, 2410, 1608, 804,
  0, -804, -1608, -2410, -3212, -4011, -4808, -5602, -6393, -7179, -7962, -8739, -9512, -10278, -11039, -11793,
  -12539, -13279, -14010, -14732, -15446, -16151, -16846, -17530, -18204, -18868, -19519, -20159, -20787, -21403, -22005, -22594,
  -23170, -23731, -24279, -24811, -25329, -25832, -26319, -26790, -27245, -27683, -28105, -28510, -28898, -29268, -29621, -29956,
  -30273, -30571, -30852, -31113, -31356, -31580, -31785, -31971, -32137, -32285, -32412, -32521, -32609, -32678, -32728, -32757,
  -32767, -32757, -32728, -32678, -32609, -32521, -32412, -32285, -32137, -31971, -31785, -31580, -31356, -31113, -30852, -30571,
  -30273, -29956, -29621, -29268, -28898, -28510, -28105, -27683, -27245, -26790, -26319, -25832, -25329, -24811, -24279, -23731,
  -23170, -22594, -22005, -21403, -20787, -20159, -19519, -18868, -18204, -17530, -16846, -16151, -15446, -14732, -14010, -13279,
  -12539, -11793, -11039, -10278, -9512, -8739, -7962, -7179, -6393, -5602, -4808, -4011, -3212, -2410, -1608, -804
};

// MIDI note to frequency conversion
const uint32_t note_frequencies[128] PROGMEM = {
  0x001A, 0x001C, 0x001E, 0x001F, 0x0021, 0x0023, 0x0025, 0x0028, 0x002A, 0x002D, 0x002F, 0x0032,
  0x0035, 0x0038, 0x003C, 0x003F, 0x0043, 0x0047, 0x004B, 0x0050, 0x0055, 0x005A, 0x005F, 0x0065,
  0x006B, 0x0071, 0x0078, 0x007F, 0x0087, 0x008F, 0x0097, 0x00A0, 0x00AA, 0x00B4, 0x00BE, 0x00CA,
  0x00D6, 0x00E3, 0x00F0, 0x00FE, 0x010E, 0x011E, 0x012F, 0x0141, 0x0154, 0x0168, 0x017D, 0x0194,
  0x01AC, 0x01C6, 0x01E1, 0x01FD, 0x021C, 0x023C, 0x025E, 0x0282, 0x02A8, 0x02D0, 0x02FB, 0x0329,
  0x0359, 0x038C, 0x03C2, 0x03FB, 0x0438, 0x0478, 0x04BC, 0x0504, 0x0550, 0x05A1, 0x05F7, 0x0652,
  0x06B2, 0x0718, 0x0784, 0x07F6, 0x0870, 0x08F0, 0x0978, 0x0A08, 0x0AA1, 0x0B43, 0x0BEF, 0x0CA4,
  0x0D65, 0x0E31, 0x0F09, 0x0FED, 0x10E0, 0x11E1, 0x12F1, 0x1411, 0x1543, 0x1687, 0x17DE, 0x1949,
  0x1ACA, 0x1C62, 0x1E12, 0x1FDB, 0x21C0, 0x23C2, 0x25E3, 0x2823, 0x2A86, 0x2D0E, 0x2FBC, 0x3292,
  0x3594, 0x38C4, 0x3C24, 0x3FB7, 0x4381, 0x4785, 0x4BC6, 0x5047, 0x550D, 0x5A1C, 0x5F78, 0x6525,
  0x6B29, 0x7188, 0x7848, 0x7F6F, 0x8703, 0x8F0A, 0x978C, 0xA08F
};

// Function Prototypes
void setup();
void loop();
void initializeSystem();
void readInputs();
void processSequencer();
void generateAudio();
uint16_t calculateADSR(uint8_t voice);
void scheduleNoteEvent(uint8_t voice, uint8_t note, uint32_t delay_ms);
void processNoteQueue();
void playBootUpSound();
void loadDemoSong();
void startDemoSong();
void updateDisplay();
void drawNeonInterface();
void drawEncoderValues();
void drawVaporwaveLoadingScreen();
void drawStatusLine();
void drawBigStepSequencer();
void drawWaveformDisplay();
void drawBigVoiceIndicators();
void drawParameterDisplay();
void handleEncoderChange(int encoder, int old_value, int new_value);
void handleMatrixKey(int key, bool pressed);
void handleDirectButton(int button, bool pressed);
void changeMode(OperatingMode new_mode);
void savePattern(uint8_t slot);
void loadPattern(uint8_t slot);
void triggerNote(uint8_t voice, uint8_t note);
void stopNote(uint8_t voice);
int16_t getWaveformSample(uint8_t waveform, uint32_t phase);
uint32_t midiNoteToFrequencyWord(uint8_t note);
uint8_t applyScale(uint8_t note, uint8_t scale_type);
int8_t getDirection(uint8_t last_state, uint8_t current_state);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("=== MintySynth ESP32-S3 Enhanced - Neon Edition ===");
  Serial.println("Initializing enhanced synthesizer...");
  
  initializeSystem();
  
  // Initialize default patterns
  for (int v = 0; v < NUM_VOICES; v++) {
    voices[v].waveform = v % NUM_WAVEFORMS;
    voices[v].volume = 80;
    voices[v].transpose = 0;
    
    for (int s = 0; s < NUM_STEPS; s++) {
      voices[v].step_notes[s] = 60 + v * 12 + (s % 8);
      // Create different default patterns for each voice
      switch (v) {
        case 0: voices[v].step_sequence[s] = (s % 4 == 0); break;        // Kick pattern
        case 1: voices[v].step_sequence[s] = (s % 8 == 4); break;        // Snare pattern
        case 2: voices[v].step_sequence[s] = (s % 2 == 1); break;        // Hi-hat pattern
        case 3: voices[v].step_sequence[s] = (s == 0 || s == 10); break; // Accent pattern
      }
    }
  }
  
  // Set initial encoder positions
  encoders[0].position = 120;  // Tempo (120 BPM)
  encoders[1].position = 64;   // Pitch (middle)
  encoders[2].position = 50;   // Length
  encoders[3].position = 2;    // Envelope
  encoders[4].position = 0;    // Swing
  
  // Initialize demo song (but don't start playing yet)
  loadDemoSong();
  
  Serial.println("Demo song loaded successfully");
  
  // Optional boot sound (comment out if it causes issues)
  // playBootUpSound();
  
  Serial.println("MintySynth ESP32-S3 Enhanced Ready!");
  Serial.println("Features: Live Mode, Program Mode, Mixer, Scales, Save/Load");
  Serial.println("Press PLAY/STOP to start demo song!");
  
  ui.needs_full_redraw = true;
}

void loop() {
  uint32_t current_time = millis();
  static uint32_t last_audio_time = 0;
  static uint32_t last_display_time = 0;
  
  // Read inputs
  readInputs();
  
  // Process scheduled note events (for per-voice swing)
  processNoteQueue();
  
  // Process sequencer
  if (sequencer.playing) {
    processSequencer();
  }
  
  // Generate audio
  if (current_time - last_audio_time >= 25) {
    generateAudio();
    last_audio_time = current_time;
  }
  
  // Update display
  if (current_time - last_display_time >= 50) {
    updateDisplay();
    last_display_time = current_time;
  }
  
  // Update blink state for UI elements
  if (current_time - ui.last_blink >= 500) {
    ui.blink_state = !ui.blink_state;
    ui.last_blink = current_time;
  }
}

void initializeSystem() {
  // Initialize display with neon theme
  tft.begin();
  tft.setRotation(1);
  
  // Try display inversion - this fixes many ILI9341 displays that show inverted colors
  tft.invertDisplay(true);
  delay(200);
  
  // VAPORWAVE PSYCHEDELIC LOADING SCREEN
  drawVaporwaveLoadingScreen();
  
  delay(2000);  // Show the screen for 2 seconds (no audio during init)
  
  // Clear again after startup
  tft.fillScreen(0x0000);
  
  // Initialize I2S Audio for PCM5102
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = I2S_BUFFER_SIZE,
    .use_apll = false,
    .tx_desc_auto_clear = true
  };
  
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCK_PIN,
    .ws_io_num = I2S_WS_PIN,
    .data_out_num = I2S_DATA_PIN,
    .data_in_num = I2S_PIN_NO_CHANGE
  };
  
  i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM, &pin_config);
  
  Serial.println("I2S Audio initialized:");
  Serial.println("BCK (Bit Clock) = GPIO 12");
  Serial.println("WS (Word Select/LRCLK) = GPIO 20");
  Serial.println("DATA = GPIO 0");
  
  // Test I2S with a simple sine wave burst
  Serial.println("Testing I2S with sine wave burst...");
  int16_t test_buffer[1024];
  for (int i = 0; i < 512; i++) {
    int16_t sample = pgm_read_word(&wavetable_sine[i % WAVETABLE_SIZE]) / 4; // Quiet test tone
    test_buffer[i * 2] = sample;     // Left channel
    test_buffer[i * 2 + 1] = sample; // Right channel
  }
  
  size_t bytes_written;
  esp_err_t result = i2s_write(I2S_NUM, test_buffer, 1024 * 2, &bytes_written, 1000);
  Serial.printf("I2S test write: %s, bytes: %d\n", esp_err_to_name(result), bytes_written);
  
  // Initialize GPIO
  for (int i = 0; i < 4; i++) {
    pinMode(MATRIX_ROWS[i], OUTPUT);
    digitalWrite(MATRIX_ROWS[i], HIGH);
    pinMode(MATRIX_COLS[i], INPUT_PULLUP);
  }
  
  for (int i = 0; i < 5; i++) {
    pinMode(ENCODER_CLK[i], INPUT_PULLUP);
    pinMode(ENCODER_DT[i], INPUT_PULLUP);
    pinMode(ENCODER_SW[i], INPUT_PULLUP);
    encoders[i].last_clk = digitalRead(ENCODER_CLK[i]);
    encoders[i].last_dt = digitalRead(ENCODER_DT[i]);
  }
  
  for (int i = 0; i < 3; i++) {
    pinMode(DIRECT_BUTTONS[i], INPUT_PULLUP);
  }
  
  // Initialize preferences
  preferences.begin("mintysynth", false);
  
  // Initialize sequencer
  sequencer.step_length = 250;
  sequencer.mode = MODE_LIVE;
  
  ui.needs_full_redraw = true;
}

void readInputs() {
  // Read encoders with enhanced debouncing
  for (int i = 0; i < 5; i++) {
    bool clk = digitalRead(ENCODER_CLK[i]);
    bool dt = digitalRead(ENCODER_DT[i]);
    
    uint8_t current_state = (clk << 1) | dt;
    uint8_t last_state = (encoders[i].last_clk << 1) | encoders[i].last_dt;
    
    if (clk != encoders[i].last_clk || dt != encoders[i].last_dt) {
      uint32_t current_time = millis();
      
      if (current_time - encoders[i].last_change_time >= 5) {
        int8_t direction = getDirection(last_state, current_state);
        
        if (direction != 0) {
          int old_position = encoders[i].position;
          encoders[i].position += direction;
          
          // Constrain based on function
          switch (i) {
            case 0: encoders[i].position = constrain(encoders[i].position, 40, 300); break;  // Tempo
            case 1: encoders[i].position = constrain(encoders[i].position, 0, 127); break;   // Pitch
            case 2: encoders[i].position = constrain(encoders[i].position, 0, 127); break;   // Length
            case 3: encoders[i].position = constrain(encoders[i].position, 0, 127); break;   // Envelope
            case 4: encoders[i].position = constrain(encoders[i].position, 0, 100); break;   // Swing
          }
          
          if (encoders[i].position != old_position) {
            handleEncoderChange(i, old_position, encoders[i].position);
            ui.last_activity = millis();
          }
        }
        
        encoders[i].last_change_time = current_time;
      }
      
      encoders[i].last_clk = clk;
      encoders[i].last_dt = dt;
    }
    
    // Read encoder button
    bool button = (digitalRead(ENCODER_SW[i]) == LOW);
    if (button && !encoders[i].button_pressed) {
      encoders[i].button_pressed = true;
      ui.last_activity = millis();
      
      // Encoder button functions
      switch (i) {
        case 0: changeMode(MODE_LIVE); break;           // Tempo encoder = Live mode
        case 1: changeMode(MODE_SCALE); break;          // Pitch encoder = Scale mode
        case 2: changeMode(MODE_MIXER); break;          // Length encoder = Mixer mode
        case 3: current_adsr_param = (current_adsr_param + 1) % 4; break; // Envelope encoder = Cycle ADSR param
        case 4: sequencer.record_mode = !sequencer.record_mode; break; // Swing encoder = Record toggle
      }
    } else if (!button && encoders[i].button_pressed) {
      encoders[i].button_pressed = false;
    }
    encoders[i].button = button;
  }
  
  // Read matrix keyboard
  for (int row = 0; row < 4; row++) {
    for (int r = 0; r < 4; r++) {
      digitalWrite(MATRIX_ROWS[r], (r == row) ? LOW : HIGH);
    }
    delayMicroseconds(10);
    
    for (int col = 0; col < 4; col++) {
      int key_index = row * 4 + col;
      bool current_state = (digitalRead(MATRIX_COLS[col]) == LOW);
      
      if (current_state != last_matrix_keys[key_index]) {
        handleMatrixKey(key_index, current_state);
        ui.last_activity = millis();
      }
      
      matrix_keys[key_index] = current_state;
      last_matrix_keys[key_index] = current_state;
    }
  }
  
  for (int r = 0; r < 4; r++) {
    digitalWrite(MATRIX_ROWS[r], HIGH);
  }
  
  // Read direct buttons
  for (int i = 0; i < 3; i++) {
    bool current_state = (digitalRead(DIRECT_BUTTONS[i]) == LOW);
    
    if (current_state != last_direct_button_state[i]) {
      handleDirectButton(i, current_state);
      ui.last_activity = millis();
    }
    
    direct_button_state[i] = current_state;
    last_direct_button_state[i] = current_state;
  }
}

void handleEncoderChange(int encoder, int old_value, int new_value) {
  switch (encoder) {
    case 0: // Tempo
      sequencer.step_length = map(new_value, 40, 300, 800, 100);
      break;
      
    case 1: // Pitch
      if (sequencer.mode >= MODE_PROGRAM_0 && sequencer.mode <= MODE_PROGRAM_3) {
        // In program mode, transpose the current voice
        int voice = sequencer.mode - MODE_PROGRAM_0;
        voices[voice].transpose = map(new_value, 0, 127, -24, 24);
      } else if (sequencer.mode == MODE_SCALE) {
        // In scale mode, change scale
        sequencer.current_scale = map(new_value, 0, 127, 0, NUM_SCALES - 1);
      } else {
        // In live mode, global transpose
        sequencer.song_transpose = map(new_value, 0, 127, -24, 24);
      }
      break;
      
    case 2: // Length
      if (sequencer.mode >= MODE_PROGRAM_0 && sequencer.mode <= MODE_PROGRAM_3) {
        int voice = sequencer.mode - MODE_PROGRAM_0;
        voices[voice].length = map(new_value, 0, 127, 50, 2000);
      }
      break;
      
    case 3: // Envelope - Controls ADSR parameters
      if (sequencer.mode >= MODE_PROGRAM_0 && sequencer.mode <= MODE_PROGRAM_3) {
        int voice = sequencer.mode - MODE_PROGRAM_0;
        // Use global ADSR parameter selection
        
        switch (current_adsr_param) {
          case 0: // Attack
            voices[voice].attack_time = new_value;
            break;
          case 1: // Decay  
            voices[voice].decay_time = new_value;
            break;
          case 2: // Sustain
            voices[voice].sustain_level = new_value;
            break;
          case 3: // Release
            voices[voice].release_time = new_value;
            break;
        }
      } else {
        // In live mode, control waveform for current voice
        voices[sequencer.current_voice].waveform = map(new_value, 0, 127, 0, NUM_WAVEFORMS - 1);
      }
      break;
      
    case 4: // Swing
      sequencer.swing = new_value;
      break;
  }
}

void handleMatrixKey(int key, bool pressed) {
  if (!pressed) return;  // Only handle key presses
  
  if (key < NUM_STEPS) {
    switch (sequencer.mode) {
      case MODE_LIVE:
        if (sequencer.record_mode && sequencer.playing) {
          // Live recording: record note to current step for active voice
          uint8_t note = applyScale(60 + key, sequencer.current_scale) + sequencer.song_transpose;
          note = constrain(note, 0, 127);
          
          // If current step already has a note, erase it (toggle behavior)
          if (voices[sequencer.current_voice].step_sequence[sequencer.current_step]) {
            voices[sequencer.current_voice].step_sequence[sequencer.current_step] = false;
          } else {
            // Record new note to the current step
            voices[sequencer.current_voice].step_sequence[sequencer.current_step] = true;
            voices[sequencer.current_voice].step_notes[sequencer.current_step] = note;
          }
          
          // Always trigger the note for immediate feedback
          triggerNote(sequencer.current_voice, note);
        } else {
          // Standard live mode: just trigger preview note
          triggerNote(sequencer.current_voice, 
                     applyScale(60 + key, sequencer.current_scale) + sequencer.song_transpose);
        }
        break;
        
      case MODE_PROGRAM_0:
      case MODE_PROGRAM_1:
      case MODE_PROGRAM_2:
      case MODE_PROGRAM_3: {
        // In program mode, toggle step
        int voice = sequencer.mode - MODE_PROGRAM_0;
        voices[voice].step_sequence[key] = !voices[voice].step_sequence[key];
        
        if (voices[voice].step_sequence[key]) {
          // Play preview of this step
          triggerNote(voice, voices[voice].step_notes[key] + voices[voice].transpose);
        }
        break;
      }
      
      case MODE_MIXER:
        // In mixer mode, adjust voice volumes and swing
        if (key < NUM_VOICES) {
          voices[key].volume = (voices[key].volume > 0) ? 0 : 80; // Toggle mute
        } else if (key >= 8 && key < 12) {
          // Keys 8-11 control per-voice swing for voices 0-3
          int voice = key - 8;
          voices[voice].swing_offset = (voices[voice].swing_offset > 0) ? 0 : 25; // Toggle swing
        }
        break;
        
      case MODE_SONG:
        // In song mode, save/load patterns
        if (key < 8) {
          savePattern(key);
        } else if (key < 16) {
          loadPattern(key - 8);
        }
        break;
    }
  }
}

void handleDirectButton(int button, bool pressed) {
  if (!pressed) return;
  
  Serial.printf("Physical Button %d pressed (array index %d)!\n", button + 1, button);
  
  switch (button) {
    case 0: // Button 1 - Play/Stop
      sequencer.playing = !sequencer.playing;
      Serial.printf("BUTTON 1 (PLAY/STOP) pressed - now %s\n", sequencer.playing ? "PLAYING" : "STOPPED");
      if (sequencer.playing) {
        sequencer.current_step = 0;
        sequencer.last_step_time = millis();
        // Start the demo song for immediate audio
        startDemoSong();
      }
      break;
      
    case 1: // Button 2 - Voice Select / Mode
      Serial.printf("BUTTON 2 (VOICE SELECT) pressed\n");
      if (sequencer.mode == MODE_LIVE) {
        sequencer.current_voice = (sequencer.current_voice + 1) % NUM_VOICES;
        Serial.printf("Switched to voice %s\n", voice_names[sequencer.current_voice]);
      } else if (sequencer.mode >= MODE_PROGRAM_0 && sequencer.mode <= MODE_PROGRAM_3) {
        changeMode((OperatingMode)((sequencer.mode - MODE_PROGRAM_0 + 1) % NUM_VOICES + MODE_PROGRAM_0));
      } else {
        changeMode(MODE_LIVE);
      }
      break;
      
    case 2: // Button 3 - Clear/Program
      Serial.printf("BUTTON 3 (CLEAR/SHIFT) pressed\n");
      if (sequencer.mode == MODE_LIVE) {
        // TEST: Trigger a simple test tone instead of mode change
        Serial.println("TRIGGERING TEST TONE - Voice 0, Note 60 (Middle C)");
        triggerNote(0, 60);
      } else if (sequencer.mode >= MODE_PROGRAM_0 && sequencer.mode <= MODE_PROGRAM_3) {
        // Clear pattern
        int voice = sequencer.mode - MODE_PROGRAM_0;
        for (int s = 0; s < NUM_STEPS; s++) {
          voices[voice].step_sequence[s] = false;
        }
        Serial.printf("Cleared pattern for voice %s\n", voice_names[voice]);
      } else {
        changeMode(MODE_LIVE);
      }
      break;
  }
}

void changeMode(OperatingMode new_mode) {
  OperatingMode old_mode = sequencer.mode;
  sequencer.mode = new_mode;
  
  // Only trigger full redraw if mode actually changed
  if (old_mode != new_mode) {
    ui.needs_full_redraw = true;
  }
  
  // Mode-specific initialization
  switch (new_mode) {
    case MODE_SCALE:
      encoders[1].position = map(sequencer.current_scale, 0, NUM_SCALES - 1, 0, 127);
      break;
    case MODE_MIXER:
      // Set encoder positions to current volumes
      for (int i = 0; i < NUM_VOICES && i < 4; i++) {
        encoders[i].position = voices[i].volume;
      }
      break;
  }
}

void processSequencer() {
  uint32_t current_time = millis();
  uint32_t step_duration = sequencer.step_length;
  
  // Apply global swing
  if (sequencer.swing > 0 && sequencer.current_step % 2 == 1) {
    step_duration += (sequencer.step_length * sequencer.swing) / 200;
  }
  
  if (current_time - sequencer.last_step_time >= step_duration) {
    Serial.printf("Step %d - ", sequencer.current_step);
    
    // Trigger notes for all voices on this step
    bool any_notes = false;
    for (int v = 0; v < NUM_VOICES; v++) {
      if (voices[v].step_sequence[sequencer.current_step]) {
        uint8_t note = voices[v].step_notes[sequencer.current_step];
        note = applyScale(note, sequencer.current_scale);
        note += voices[v].transpose + sequencer.song_transpose;
        note = constrain(note, 0, 127);
        
        Serial.printf("V%d:Note%d ", v, note);
        any_notes = true;
        
        // Apply per-voice swing offset using non-blocking scheduler
        uint32_t voice_delay = 0;
        if (voices[v].swing_offset > 0 && sequencer.current_step % 2 == 1) {
          voice_delay = (sequencer.step_length * voices[v].swing_offset) / 200;
        }
        
        if (voice_delay > 0) {
          // Schedule the note to trigger later (non-blocking)
          scheduleNoteEvent(v, note, voice_delay);
        } else {
          // Trigger immediately if no delay
          triggerNote(v, note);
        }
      }
    }
    
    if (!any_notes) Serial.print("(silence)");
    Serial.println();
    
    sequencer.current_step = (sequencer.current_step + 1) % NUM_STEPS;
    sequencer.last_step_time = current_time;
  }
}

void scheduleNoteEvent(uint8_t voice, uint8_t note, uint32_t delay_ms) {
  // Find an available slot in the event queue
  for (int i = 0; i < MAX_SCHEDULED_EVENTS; i++) {
    if (!event_queue[i].active) {
      event_queue[i].active = true;
      event_queue[i].trigger_time = millis() + delay_ms;
      event_queue[i].voice = voice;
      event_queue[i].note = note;
      return;
    }
  }
  
  // If no slot available, trigger immediately (fallback)
  triggerNote(voice, note);
}

void processNoteQueue() {
  uint32_t current_time = millis();
  
  // Check all scheduled events
  for (int i = 0; i < MAX_SCHEDULED_EVENTS; i++) {
    if (event_queue[i].active && current_time >= event_queue[i].trigger_time) {
      // Time to trigger this note
      triggerNote(event_queue[i].voice, event_queue[i].note);
      
      // Mark slot as available
      event_queue[i].active = false;
    }
  }
}

void generateAudio() {
  static uint32_t debug_counter = 0;
  static uint32_t last_debug = 0;
  
  // I2S Audio Generation (for PCM5102)
  for (int i = 0; i < I2S_BUFFER_SIZE; i++) {
    int32_t mix_left = 0;
    int32_t mix_right = 0;
    
    for (int v = 0; v < NUM_VOICES; v++) {
      if (voices[v].active) {
        int16_t sample = getWaveformSample(voices[v].waveform, voices[v].phase_accumulator);
        
        // Apply ADSR envelope
        uint16_t envelope_amplitude = calculateADSR(v);
        
        sample = (sample * envelope_amplitude * voices[v].volume) >> 14;
        
        // Pan voices
        if (v % 2 == 0) {
          mix_left += sample;
        } else {
          mix_right += sample;
        }
        
        voices[v].phase_accumulator += voices[v].frequency_tuning_word;
      }
    }
    
    mix_left = constrain(mix_left, -32767, 32767);
    mix_right = constrain(mix_right, -32767, 32767);
    
    audio_buffer[i * 2] = mix_left;
    audio_buffer[i * 2 + 1] = mix_right;
  }
  
  // Debug output every 2 seconds
  debug_counter++;
  if (millis() - last_debug > 2000) {
    int active_count = voices[0].active + voices[1].active + voices[2].active + voices[3].active;
    Serial.printf("Audio gen: %d active voices, buffer calls: %d\n", active_count, debug_counter);
    
    // Show details of active voices
    for (int v = 0; v < NUM_VOICES; v++) {
      if (voices[v].active) {
        Serial.printf("  Voice %d: Note=%d, Amp=%d, Env=%d, Wave=%d\n", 
                     v, voices[v].note, voices[v].current_amplitude, voices[v].envelope_stage, voices[v].waveform);
      }
    }
    
    // Show sample values from last buffer
    if (active_count > 0) {
      Serial.printf("  Last audio samples: L=%d, R=%d\n", audio_buffer[0], audio_buffer[1]);
    }
    
    last_debug = millis();
    debug_counter = 0;
  }
  
  size_t bytes_written;
  esp_err_t result = i2s_write(I2S_NUM, audio_buffer, I2S_BUFFER_SIZE * 4, &bytes_written, portMAX_DELAY);
  if (result != ESP_OK) {
    Serial.printf("I2S write error: %s\n", esp_err_to_name(result));
  }
}

void updateDisplay() {
  static bool first_draw = true;
  static uint8_t last_step = 255;
  static uint8_t last_voice = 255;
  static bool last_playing = false;
  static OperatingMode last_mode = MODE_LIVE;
  static int last_tempo = -1;
  static int last_pitch = -1;
  static int last_length = -1;
  static int last_env = -1;
  static int last_swing = -1;
  static bool last_record = false;
  
  if (ui.needs_full_redraw || first_draw) {
    drawNeonInterface();
    ui.needs_full_redraw = false;
    first_draw = false;
    
    // Update all cached values
    last_step = sequencer.current_step;
    last_voice = sequencer.current_voice;
    last_playing = sequencer.playing;
    last_mode = sequencer.mode;
    last_tempo = encoders[0].position;
    last_pitch = encoders[1].position;
    last_length = encoders[2].position;
    last_env = encoders[3].position;
    last_swing = encoders[4].position;
    last_record = sequencer.record_mode;
  }
  
  // Update mode indicator if changed
  if (sequencer.mode != last_mode) {
    // Clear and update mode indicator
    tft.fillRect(200, 12, 100, 15, COLOR_ACCENT1);
    tft.setTextColor(COLOR_ACCENT2);
    tft.setTextSize(1);
    tft.setCursor(200, 12);
    switch (sequencer.mode) {
      case MODE_LIVE: tft.print("LIVE"); break;
      case MODE_PROGRAM_0: case MODE_PROGRAM_1: case MODE_PROGRAM_2: case MODE_PROGRAM_3:
        tft.printf("PROG-%s", voice_names[sequencer.mode - MODE_PROGRAM_0]); break;
      case MODE_MIXER: tft.print("MIXER"); break;
      case MODE_SCALE: tft.print("SCALE"); break;
      case MODE_SONG: tft.print("SONG"); break;
    }
    last_mode = sequencer.mode;
  }

  // Always update status line - NO FLASHING
  if (sequencer.current_step != last_step || 
      sequencer.current_voice != last_voice ||
      sequencer.playing != last_playing) {
    
    // Clear and update status line
    tft.fillRect(5, 40, 310, 15, 0x0000);  // Black background
    tft.setTextColor(COLOR_TEXT);
    tft.setCursor(5, 40);
    tft.printf("Voice:%s %s Step:%02d BPM:%d", 
               voice_names[sequencer.current_voice],
               sequencer.playing ? "PLAY" : "STOP",
               sequencer.current_step + 1,
               map(sequencer.step_length, 100, 800, 300, 40));
    
    last_step = sequencer.current_step;
    last_voice = sequencer.current_voice;
    last_playing = sequencer.playing;
  }
  
  // Update step grid when voice changes or step advances
  if (sequencer.current_voice != last_voice || sequencer.current_step != last_step) {
    drawBigStepSequencer();
    last_voice = sequencer.current_voice;
    last_step = sequencer.current_step;
  }
  
  // This section moved to drawEncoderValues() function
  
  // Update record indicator
  if (sequencer.record_mode != last_record) {
    if (sequencer.record_mode && ui.blink_state) {
      tft.fillCircle(310, 45, 5, COLOR_WARNING);
      tft.setTextColor(COLOR_WARNING);
      tft.setCursor(280, 40);
      tft.print("REC");
    } else {
      tft.fillCircle(310, 45, 5, 0x0000);  // Black background
      tft.fillRect(280, 40, 30, 10, 0x0000);  // Black background
    }
    last_record = sequencer.record_mode;
  }
  
  // Always update encoder values to fix real-time display issue
  drawEncoderValues();
}

void drawNeonInterface() {
  // Clear screen with pure black background
  tft.fillScreen(0x0000);  // Force pure black
  
  // Layout plan (320x240):
  // Row 1: Title bar (0-30)
  // Row 2: Status info (35-55) 
  // Row 3: Step sequencer (60-115)
  // Row 4: Waveform + Record (120-165)
  // Row 5: Voice indicators (170-210)
  // Row 6: Parameters (215-240)
  
  // ═══ TITLE BAR (0-30) ═══
  tft.fillRect(0, 0, 320, 30, COLOR_ACCENT1);
  tft.setTextColor(0x0000);
  tft.setTextSize(2);
  tft.setCursor(8, 8);
  tft.print("MintySynth ESP32-S3");
  
  // Mode indicator (top right)
  tft.fillRect(240, 5, 75, 20, COLOR_ACCENT2);
  tft.setTextColor(0x0000);
  tft.setTextSize(1);
  tft.setCursor(245, 12);
  switch (sequencer.mode) {
    case MODE_LIVE: tft.print("LIVE"); break;
    case MODE_PROGRAM_0: case MODE_PROGRAM_1: case MODE_PROGRAM_2: case MODE_PROGRAM_3:
      tft.printf("PROG-%s", voice_names[sequencer.mode - MODE_PROGRAM_0]); break;
    case MODE_MIXER: tft.print("MIXER"); break;
    case MODE_SCALE: tft.print("SCALE"); break;
    case MODE_SONG: tft.print("SONG"); break;
  }
  
  // ═══ STATUS ROW (35-55) ═══
  // Status is handled in updateDisplay() to avoid flashing
  
  // ═══ STEP SEQUENCER (60-115) ═══
  drawBigStepSequencer();
  
  // ═══ WAVEFORM ROW (120-165) ═══
  drawWaveformDisplay();
  
  // ═══ VOICE INDICATORS (170-210) ═══
  drawBigVoiceIndicators();
  
  // ═══ ENCODER VALUES (Right side, avoid overlap) ═══
  drawEncoderValues();
  
  // ═══ PARAMETERS (215-240) ═══
  drawParameterDisplay();
  
  // Record indicator (overlay)
  if (sequencer.record_mode && ui.blink_state) {
    tft.fillCircle(300, 40, 6, COLOR_WARNING);
    tft.setTextColor(COLOR_WARNING);
    tft.setTextSize(1);
    tft.setCursor(275, 37);
    tft.print("REC");
  }
}

void drawVaporwaveLoadingScreen() {
  tft.fillScreen(0x0000);  // Black background
  
  // Simple gradient background
  for (int y = 0; y < 200; y++) {
    uint16_t color = map(y, 0, 200, 0x1000, 0x6000);  // Dark to purple gradient
    tft.drawLine(0, y, 320, y, color);
  }
  
  // Sun with rays
  int sun_x = 160;
  int sun_y = 100;
  
  // Draw sun rays first (behind the sun)
  for (int i = 0; i < 12; i++) {
    float angle = (i * 30) * PI / 180.0;  // 12 rays, 30 degrees apart
    int ray_start = 40;
    int ray_end = 60;
    int x1 = sun_x + cos(angle) * ray_start;
    int y1 = sun_y + sin(angle) * ray_start;
    int x2 = sun_x + cos(angle) * ray_end;
    int y2 = sun_y + sin(angle) * ray_end;
    
    tft.drawLine(x1, y1, x2, y2, COLOR_WARNING);  // Orange rays
    tft.drawLine(x1+1, y1, x2+1, y2, COLOR_WARNING);  // Thicker rays
  }
  
  // Draw the sun
  tft.fillCircle(sun_x, sun_y, 35, COLOR_WARNING);  // Orange sun
  tft.drawCircle(sun_x, sun_y, 37, COLOR_ACCENT2);  // Outer ring
  
  // Black bar at bottom
  tft.fillRect(0, 200, 320, 40, 0x0000);
  
  // Title in black bar
  tft.setTextSize(2);
  tft.setTextColor(COLOR_ACCENT2);
  tft.setCursor(60, 210);
  tft.print("VaporSynth");
  
  tft.setTextSize(1);
  tft.setTextColor(COLOR_ACCENT3);
  tft.setCursor(85, 225);
  tft.print("(Based on MintySynth)");
  
  delay(2000);
}

void drawStatusLine() {
  // STATUS ROW (35-55) - NEVER flash, always visible
  // Clear the status area
  tft.fillRect(0, 35, 320, 20, 0x0000);
  
  int bpm = map(sequencer.step_length, 100, 800, 300, 40);
  
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(1);
  tft.setCursor(10, 40);
  tft.printf("Voice: %s", voice_names[sequencer.current_voice]);
  
  tft.setCursor(10, 48);
  tft.setTextColor(sequencer.playing ? COLOR_SUCCESS : COLOR_WARNING);
  tft.printf("Step: %02d/%d", sequencer.current_step + 1, NUM_STEPS);
  
  tft.setTextColor(COLOR_ACCENT3);
  tft.setCursor(200, 40);
  tft.printf("BPM: %d", bpm);
  
  // Dancing person (when playing)
  if (sequencer.playing) {
    uint32_t dance_speed = sequencer.step_length / 4;
    if (millis() - ui.last_dance_update >= dance_speed) {
      ui.dance_frame = (ui.dance_frame + 1) % 4;
      ui.last_dance_update = millis();
    }
    
    tft.setTextColor(COLOR_ACCENT2);
    tft.setTextSize(1);
    tft.setCursor(280, 45);
    switch (ui.dance_frame) {
      case 0: tft.print("\\o/"); break;
      case 1: tft.print("-o-"); break;
      case 2: tft.print("/o\\"); break;
      case 3: tft.print("-o-"); break;
    }
  }
}

void drawBigStepSequencer() {
  // STEP SEQUENCER (60-115) - 55 pixels high
  int grid_x = 10;
  int grid_y = 60;
  int step_size = 25;  // Fits perfectly: 8*25 = 200, leaves room for margins
  
  for (int step = 0; step < NUM_STEPS; step++) {
    int x = grid_x + (step % 8) * step_size;
    int y = grid_y + (step / 8) * step_size;
    
    uint16_t color = COLOR_STEP_OFF;
    bool is_current = (step == sequencer.current_step && sequencer.playing);
    
    if (sequencer.mode >= MODE_PROGRAM_0 && sequencer.mode <= MODE_PROGRAM_3) {
      int voice = sequencer.mode - MODE_PROGRAM_0;
      if (voices[voice].step_sequence[step]) {
        color = is_current ? COLOR_CURRENT : COLOR_STEP_ON;
      } else {
        color = is_current ? COLOR_WARNING : COLOR_STEP_OFF;
      }
    } else {
      // Show current voice pattern in live mode
      if (voices[sequencer.current_voice].step_sequence[step]) {
        color = is_current ? COLOR_CURRENT : COLOR_STEP_ON;
      } else {
        color = is_current ? COLOR_WARNING : COLOR_STEP_OFF;
      }
    }
    
    // Draw step with enhanced neon glow effect
    tft.fillRect(x, y, step_size - 3, step_size - 3, color);
    tft.drawRect(x - 1, y - 1, step_size - 1, step_size - 1, COLOR_GRID);
    
    // Add glow for active steps
    if (color == COLOR_STEP_ON || color == COLOR_CURRENT) {
      tft.drawRect(x - 2, y - 2, step_size + 1, step_size + 1, COLOR_DIM);
      if (is_current) {
        tft.drawRect(x - 3, y - 3, step_size + 3, step_size + 3, COLOR_TEXT);
      }
    }
    
    // Step numbers (only for first row to save space)
    if (step < 8) {
      tft.setTextColor(COLOR_TEXT);
      tft.setTextSize(1);
      tft.setCursor(x + 7, y + 7);
      tft.printf("%d", step + 1);
    }
  }
}

void drawWaveformDisplay() {
  // WAVEFORM ROW (120-165) - Full width now!
  if (millis() - ui.last_waveform_update >= 50) {  // Update 20 times per second
    // Generate waveform data based on active voices
    int16_t current_sample = 0;
    for (int v = 0; v < NUM_VOICES; v++) {
      if (voices[v].active) {
        current_sample += getWaveformSample(voices[v].waveform, voices[v].phase_accumulator) / 4;
      }
    }
    
    ui.waveform_history[ui.waveform_index] = current_sample / 1000;  // Scale down
    ui.waveform_index = (ui.waveform_index + 1) % 80;
    ui.last_waveform_update = millis();
  }
  
  // Draw waveform (full width)
  int waveform_x = 10;
  int waveform_y = 125;
  int waveform_width = 200;  // Narrower to avoid encoder overlap
  int waveform_height = 35;
  
  // Background
  tft.fillRect(waveform_x, waveform_y, waveform_width, waveform_height, 0x0000);
  tft.drawRect(waveform_x - 1, waveform_y - 1, waveform_width + 2, waveform_height + 2, COLOR_GRID);
  
  // Draw waveform line (scale to full width)
  for (int i = 1; i < 80; i++) {
    int x1 = waveform_x + ((i - 1) * waveform_width) / 80;
    int y1 = waveform_y + waveform_height/2 + ui.waveform_history[(ui.waveform_index + i - 1) % 80];
    int x2 = waveform_x + (i * waveform_width) / 80;
    int y2 = waveform_y + waveform_height/2 + ui.waveform_history[(ui.waveform_index + i) % 80];
    
    tft.drawLine(x1, y1, x2, y2, COLOR_ACCENT3);
  }
  
  // Center line
  tft.drawLine(waveform_x, waveform_y + waveform_height/2, 
               waveform_x + waveform_width, waveform_y + waveform_height/2, COLOR_DIM);
  
  // Label
  tft.setTextColor(COLOR_ACCENT1);
  tft.setTextSize(1);
  tft.setCursor(waveform_x, waveform_y - 10);
  tft.print("WAVEFORM VISUALIZATION");
}

// Spinning record function removed - using full width waveform instead

void drawBigVoiceIndicators() {
  // VOICE INDICATORS (170-210) - 40 pixels high
  for (int v = 0; v < NUM_VOICES; v++) {
    int x = 10 + v * 75;  // Spread out more: 4*75 = 300, fits well
    int y = 175;
    
    uint16_t color = (v == sequencer.current_voice) ? COLOR_CURRENT : COLOR_DIM;
    if (sequencer.mode >= MODE_PROGRAM_0 && sequencer.mode <= MODE_PROGRAM_3) {
      color = (v == (sequencer.mode - MODE_PROGRAM_0)) ? COLOR_ACCENT2 : COLOR_DIM;
    }
    
    // Voice indicators
    tft.fillRect(x, y, 65, 20, color);
    tft.drawRect(x - 1, y - 1, 67, 22, COLOR_GRID);
    if (v == sequencer.current_voice) {
      tft.drawRect(x - 2, y - 2, 69, 24, COLOR_TEXT);
    }
    
    tft.setTextColor(0x0000);  // Black text on colored background
    tft.setTextSize(1);
    tft.setCursor(x + 2, y + 6);
    tft.printf("%s", voice_names[v]);
    
    // Volume bar (below voice name)
    int vol_width = (voices[v].volume * 60) / 127;
    tft.fillRect(x, y + 25, vol_width, 6, COLOR_SUCCESS);
    tft.drawRect(x - 1, y + 24, 62, 8, COLOR_GRID);
    
    // Volume number
    tft.setTextColor(COLOR_TEXT);
    tft.setCursor(x + 45, y + 6);
    tft.printf("V%d", voices[v].volume / 10);
  }
}

void drawEncoderValues() {
  // ENCODER VALUES - Place them on the right side to avoid overlap
  // Cache last drawn values to prevent unnecessary redraws
  static int last_tempo = -1;
  static int last_pitch = -1;
  static int last_length = -1;
  static int last_env = -1;
  static int last_swing = -1;
  static uint8_t last_scale = 255;
  static OperatingMode last_mode = MODE_LIVE;
  static uint8_t last_adsr_param = 255;
  static uint8_t last_adsr_value = 255;
  static bool first_draw = true;
  
  tft.setTextColor(COLOR_ACCENT3);
  tft.setTextSize(1);
  
  // Clear and redraw everything on first draw or mode change
  if (first_draw || sequencer.mode != last_mode) {
    tft.fillRect(220, 65, 100, 80, 0x0000);
    first_draw = false;
    last_mode = sequencer.mode;
    // Reset all cache values to force redraw
    last_tempo = last_pitch = last_length = last_env = last_swing = -1;
    last_scale = 255;
    last_adsr_param = last_adsr_value = 255;
  }
  
  // TEMPO - Only redraw if changed
  if (encoders[0].position != last_tempo) {
    tft.fillRect(225, 70, 95, 10, 0x0000);  // Clear this line
    tft.setCursor(225, 70);
    tft.printf("TEMPO: %3d", encoders[0].position);
    last_tempo = encoders[0].position;
  }
  
  // PITCH/SCALE - Only redraw if changed
  if (sequencer.mode == MODE_SCALE) {
    if (sequencer.current_scale != last_scale) {
      tft.fillRect(225, 82, 95, 10, 0x0000);
      tft.setCursor(225, 82);
      tft.printf("SCALE: %s", scale_names[sequencer.current_scale]);
      last_scale = sequencer.current_scale;
    }
  } else {
    if (encoders[1].position != last_pitch) {
      tft.fillRect(225, 82, 95, 10, 0x0000);
      tft.setCursor(225, 82);
      tft.printf("PITCH: %3d", encoders[1].position);
      last_pitch = encoders[1].position;
    }
  }
  
  // LENGTH - Only redraw if changed
  if (encoders[2].position != last_length) {
    tft.fillRect(225, 94, 95, 10, 0x0000);
    tft.setCursor(225, 94);
    tft.printf("LENGTH:%3d", encoders[2].position);
    last_length = encoders[2].position;
  }
  
  // ENVELOPE/ADSR - Only redraw if changed
  if (sequencer.mode >= MODE_PROGRAM_0 && sequencer.mode <= MODE_PROGRAM_3) {
    int voice = sequencer.mode - MODE_PROGRAM_0;
    uint8_t adsr_value = 0;
    switch (current_adsr_param) {
      case 0: adsr_value = voices[voice].attack_time; break;
      case 1: adsr_value = voices[voice].decay_time; break;
      case 2: adsr_value = voices[voice].sustain_level; break;
      case 3: adsr_value = voices[voice].release_time; break;
    }
    
    if (current_adsr_param != last_adsr_param || adsr_value != last_adsr_value) {
      tft.fillRect(225, 106, 95, 10, 0x0000);
      tft.setCursor(225, 106);
      tft.printf("%s:%3d", adsr_param_names[current_adsr_param], adsr_value);
      last_adsr_param = current_adsr_param;
      last_adsr_value = adsr_value;
    }
  } else {
    if (encoders[3].position != last_env) {
      tft.fillRect(225, 106, 95, 10, 0x0000);
      tft.setCursor(225, 106);
      tft.printf("ENV: %3d", encoders[3].position);
      last_env = encoders[3].position;
    }
  }
  
  // SWING - Only redraw if changed
  if (sequencer.swing != last_swing) {
    tft.fillRect(225, 118, 95, 10, 0x0000);
    tft.setCursor(225, 118);
    tft.printf("SWING:%3d%%", sequencer.swing);
    last_swing = sequencer.swing;
  }
}

void drawParameterDisplay() {
  // PARAMETERS (215-240) - Bottom 25 pixels
  // Clear the entire parameter area first
  tft.fillRect(0, 215, 320, 25, 0x0000);
  
  tft.setTextColor(COLOR_ACCENT1);
  tft.setTextSize(1);
  tft.setCursor(10, 218);
  tft.print("MODE:");
  
  // Current mode
  tft.setTextColor(COLOR_ACCENT3);
  tft.setCursor(50, 218);
  
  switch (sequencer.mode) {
    case MODE_LIVE: tft.print("LIVE"); break;
    case MODE_PROGRAM_0: case MODE_PROGRAM_1: case MODE_PROGRAM_2: case MODE_PROGRAM_3:
      tft.printf("PROG-%s", voice_names[sequencer.mode - MODE_PROGRAM_0]); break;
    case MODE_MIXER: tft.print("MIXER"); break;
    case MODE_SCALE: tft.print("SCALE"); break;
    case MODE_SONG: tft.print("SONG"); break;
  }
  
  // Show current encoder values in compact form
  tft.setTextColor(COLOR_DIM);
  tft.setCursor(10, 230);
  tft.printf("T:%d P:%d L:%d E:%d S:%d", 
             encoders[0].position, encoders[1].position, encoders[2].position,
             encoders[3].position, encoders[4].position);
}

// Removed redundant function - use drawBigStepSequencer directly

// Implementation of remaining functions...
uint8_t applyScale(uint8_t note, uint8_t scale_type) {
  if (scale_type >= NUM_SCALES) return note;
  
  uint8_t octave = note / 12;
  uint8_t scale_note = note % 12;
  
  return octave * 12 + scales[scale_type][scale_note];
}

void triggerNote(uint8_t voice, uint8_t note) {
  if (voice >= NUM_VOICES) return;
  
  uint32_t freq_word = pgm_read_dword(&note_frequencies[note]);
  Serial.printf("TRIGGER Voice %d Note %d (freq_word=0x%X)\n", voice, note, freq_word);
  
  voices[voice].active = true;
  voices[voice].note = note;
  voices[voice].frequency_tuning_word = freq_word;
  voices[voice].phase_accumulator = 0;
  voices[voice].note_start_time = millis();
  voices[voice].note_released = false;
  voices[voice].note_release_time = 0;
  
  // Initialize ADSR envelope
  voices[voice].envelope_stage = ENV_ATTACK;
  voices[voice].envelope_counter = 0;
  voices[voice].current_amplitude = 0;
  
  Serial.printf("Voice %d activated: freq=0x%X, wave=%d\n", voice, freq_word, voices[voice].waveform);
}

void stopNote(uint8_t voice) {
  if (voice >= NUM_VOICES) return;
  
  // Don't immediately stop - start release phase instead
  if (voices[voice].active && !voices[voice].note_released) {
    voices[voice].note_released = true;
    voices[voice].note_release_time = millis();
    voices[voice].envelope_stage = ENV_RELEASE;
    voices[voice].envelope_counter = 0;
  }
}

uint16_t calculateADSR(uint8_t voice) {
  if (voice >= NUM_VOICES || !voices[voice].active) return 0;
  
  Voice* v = &voices[voice];
  uint32_t elapsed = millis() - v->note_start_time;
  
  // Convert ADSR parameters to time values (in ms)
  uint32_t attack_time_ms = map(v->attack_time, 0, 127, 1, 2000);
  uint32_t decay_time_ms = map(v->decay_time, 0, 127, 10, 2000); 
  uint32_t sustain_amplitude = map(v->sustain_level, 0, 127, 0, 32767);
  uint32_t release_time_ms = map(v->release_time, 0, 127, 10, 4000);
  
  switch (v->envelope_stage) {
    case ENV_ATTACK:
      if (elapsed < attack_time_ms) {
        // Linear attack from 0 to max amplitude
        v->current_amplitude = (elapsed * 32767) / attack_time_ms;
      } else {
        // Attack complete, move to decay
        v->current_amplitude = 32767;
        v->envelope_stage = ENV_DECAY;
        v->envelope_counter = elapsed;
      }
      break;
      
    case ENV_DECAY:
      {
        uint32_t decay_elapsed = elapsed - v->envelope_counter;
        if (decay_elapsed < decay_time_ms) {
          // Linear decay from max amplitude to sustain level
          uint32_t decay_amount = ((32767 - sustain_amplitude) * decay_elapsed) / decay_time_ms;
          v->current_amplitude = 32767 - decay_amount;
        } else {
          // Decay complete, move to sustain
          v->current_amplitude = sustain_amplitude;
          v->envelope_stage = ENV_SUSTAIN;
        }
      }
      break;
      
    case ENV_SUSTAIN:
      // Hold sustain level until note is released
      v->current_amplitude = sustain_amplitude;
      
      // Check if note should be released due to length
      if (elapsed >= v->length && !v->note_released) {
        v->note_released = true;
        v->note_release_time = millis();
        v->envelope_stage = ENV_RELEASE;
        v->envelope_counter = 0;
      }
      break;
      
    case ENV_RELEASE:
      {
        uint32_t release_elapsed = millis() - v->note_release_time;
        if (release_elapsed < release_time_ms) {
          // Linear release from current level to 0
          uint32_t release_start_amplitude = sustain_amplitude;
          if (v->envelope_counter == 0) {
            // Store the amplitude when release started
            release_start_amplitude = v->current_amplitude;
            v->envelope_counter = release_start_amplitude;
          } else {
            release_start_amplitude = v->envelope_counter;
          }
          
          uint32_t release_amount = (release_start_amplitude * release_elapsed) / release_time_ms;
          v->current_amplitude = release_start_amplitude - release_amount;
        } else {
          // Release complete, turn off voice
          v->current_amplitude = 0;
          v->active = false;
          v->envelope_stage = ENV_OFF;
        }
      }
      break;
      
    case ENV_OFF:
    default:
      v->current_amplitude = 0;
      v->active = false;
      break;
  }
  
  return v->current_amplitude;
}

int16_t getWaveformSample(uint8_t waveform, uint32_t phase) {
  uint8_t table_index = (phase >> 24);
  
  switch (waveform) {
    case WAVE_SINE:
      return pgm_read_word(&wavetable_sine[table_index]);
    case WAVE_TRIANGLE:
      return (table_index < 128) ? (table_index * 512) - 32767 : 32767 - ((table_index - 128) * 512);
    case WAVE_SAWTOOTH:
      return (table_index * 256) - 32767;
    case WAVE_SQUARE:
      return (table_index < 128) ? 16383 : -16383;
    case WAVE_NOISE:
      return random(-16383, 16383);
    case WAVE_RAMP:  // Inverted sawtooth
      return 32767 - (table_index * 256);
    case WAVE_A:  // 25% pulse wave
      return (table_index < 64) ? 16383 : -16383;
    case WAVE_B:  // 12.5% pulse wave
      return (table_index < 32) ? 16383 : -16383;
    case WAVE_C:  // Two-step square
      if (table_index < 64) return 16383;
      else if (table_index < 128) return 0;
      else if (table_index < 192) return -16383;
      else return 0;
    case WAVE_D:  // Four-step staircase
      if (table_index < 64) return 16383;
      else if (table_index < 128) return 5461;
      else if (table_index < 192) return -5461;
      else return -16383;
    case WAVE_E:  // Exponential decay
      return (32767 >> (table_index >> 5)) - 16383;
    case WAVE_F: { // Half rectified sine
      int16_t sine_val = pgm_read_word(&wavetable_sine[table_index]);
      return (sine_val > 0) ? sine_val : 0;
    }
    case WAVE_G: { // Absolute sine
      int16_t abs_sine = pgm_read_word(&wavetable_sine[table_index]);
      return abs(abs_sine);
    }
    case WAVE_H: { // Harmonic series (odd harmonics)
      int16_t h1 = pgm_read_word(&wavetable_sine[table_index]);
      int16_t h3 = pgm_read_word(&wavetable_sine[(table_index * 3) & 0xFF]) / 3;
      int16_t h5 = pgm_read_word(&wavetable_sine[(table_index * 5) & 0xFF]) / 5;
      return constrain(h1 + h3 + h5, -32767, 32767);
    }
    case WAVE_I: { // FM synthesis style
      uint8_t mod_index = (table_index + (pgm_read_word(&wavetable_sine[(table_index * 3) & 0xFF]) >> 10)) & 0xFF;
      return pgm_read_word(&wavetable_sine[mod_index]);
    }
    default:
      return pgm_read_word(&wavetable_sine[table_index]);
  }
}

void savePattern(uint8_t slot) {
  char key[32];
  sprintf(key, "pattern_%d", slot);
  
  // Save current voice patterns
  preferences.putBytes(key, voices, sizeof(voices));
  preferences.putUChar("current_scale", sequencer.current_scale);
  preferences.putInt("song_transpose", sequencer.song_transpose);
}

void loadPattern(uint8_t slot) {
  char key[32];
  sprintf(key, "pattern_%d", slot);
  
  // Load voice patterns
  size_t len = preferences.getBytesLength(key);
  if (len == sizeof(voices)) {
    preferences.getBytes(key, voices, len);
    sequencer.current_scale = preferences.getUChar("current_scale", 0);
    sequencer.song_transpose = preferences.getInt("song_transpose", 0);
    ui.needs_full_redraw = true;
  }
}

int8_t getDirection(uint8_t last_state, uint8_t current_state) {
  uint8_t transition = (last_state << 2) | current_state;
  switch(transition) {
    case 0b0010: case 0b1011: case 0b1101: case 0b0100: return 1;   // CW
    case 0b0001: case 0b0111: case 0b1110: case 0b1000: return -1;  // CCW
    default: return 0;
  }
}

void playBootUpSound() {
  // Simplified boot sound - just trigger a demo note after I2S is ready
  Serial.println("Boot sound ready - I2S initialized");
  
  // Play a simple welcome tone using the synthesizer engine
  delay(500);
  triggerNote(0, 60); // Middle C on voice 0
  delay(200);
  triggerNote(1, 64); // E on voice 1  
  delay(200);
  triggerNote(2, 67); // G on voice 2
}

void loadDemoSong() {
  Serial.println("Loading demo song...");
  
  // Create a demonstration song showcasing all features
  
  // BASS voice (voice 0) - Four-on-the-floor kick pattern
  voices[0].waveform = WAVE_SINE;
  voices[0].attack_time = 5;
  voices[0].decay_time = 20;
  voices[0].sustain_level = 40;
  voices[0].release_time = 30;
  voices[0].volume = 100;
  voices[0].transpose = -24; // Low bass
  
  for (int s = 0; s < NUM_STEPS; s++) {
    voices[0].step_sequence[s] = (s % 4 == 0); // Steps 1, 5, 9, 13
    voices[0].step_notes[s] = 36; // Low C
  }
  Serial.printf("BASS: Steps 0,4,8,12 active\n");
  
  // LEAD voice (voice 1) - Melodic sequence
  voices[1].waveform = WAVE_SAWTOOTH;
  voices[1].attack_time = 15;
  voices[1].decay_time = 40;
  voices[1].sustain_level = 70;
  voices[1].release_time = 60;
  voices[1].volume = 80;
  voices[1].transpose = 12; // Higher octave
  
  // Melodic pattern: C-E-G-E C-F-A-F
  uint8_t melody[16] = {60, 0, 64, 0, 67, 0, 64, 0, 60, 0, 65, 0, 69, 0, 65, 0};
  for (int s = 0; s < NUM_STEPS; s++) {
    voices[1].step_sequence[s] = (melody[s] > 0);
    voices[1].step_notes[s] = (melody[s] > 0) ? melody[s] : 60; // Default to C if 0
  }
  Serial.printf("LEAD: Melodic pattern loaded\n");
  
  // PAD voice (voice 2) - Sustained chords
  voices[2].waveform = WAVE_TRIANGLE;
  voices[2].attack_time = 60;
  voices[2].decay_time = 30;
  voices[2].sustain_level = 90;
  voices[2].release_time = 120;
  voices[2].volume = 60;
  voices[2].transpose = 0;
  
  for (int s = 0; s < NUM_STEPS; s++) {
    voices[2].step_sequence[s] = (s % 8 == 0); // Steps 1, 9
    voices[2].step_notes[s] = 48; // Mid C
  }
  Serial.printf("PAD: Steps 0,8 active\n");
  
  // PERC voice (voice 3) - Hi-hat pattern
  voices[3].waveform = WAVE_NOISE;
  voices[3].attack_time = 2;
  voices[3].decay_time = 10;
  voices[3].sustain_level = 20;
  voices[3].release_time = 15;
  voices[3].volume = 70;
  voices[3].transpose = 0;
  
  for (int s = 0; s < NUM_STEPS; s++) {
    voices[3].step_sequence[s] = (s % 2 == 1); // Off-beats
    voices[3].step_notes[s] = 72; // High pitched
  }
  Serial.printf("PERC: Off-beat pattern (1,3,5,7,9,11,13,15)\n");
  
  // Set demo song parameters
  sequencer.step_length = 200; // 150 BPM
  sequencer.swing = 15; // Add some groove
  sequencer.current_scale = 1; // Major scale
  
  Serial.println("Demo song loaded - ready to play!");
}

void startDemoSong() {
  // Start the demo song playing
  sequencer.playing = true;
  sequencer.current_step = 0;
  sequencer.last_step_time = millis();
  sequencer.mode = MODE_LIVE;
  sequencer.current_voice = 0;
}