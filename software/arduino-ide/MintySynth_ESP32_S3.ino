/*
 * MintySynth ESP32-S3 Expansion - Arduino IDE Version
 * 
 * Main firmware for ESP32-S3 based MintySynth expansion
 * 
 * Hardware:
 * - ESP32-S3-WROOM-1
 * - 2.8" ILI9341 TFT Display  
 * - 5x Rotary Encoders
 * - 4x4 Matrix Keyboard + 4 direct buttons
 * - PCM5102A I2S DAC
 * 
 * Libraries Required:
 * - TFT_eSPI by Bodmer
 * - ESP32Encoder by madhephaestus
 * 
 * Install via Arduino Library Manager
 * 
 * Based on original MintySynth by Andrew Mowry
 * http://mintysynth.com
 * 
 * License: GPL v3
 */

#include <SPI.h>
#include <TFT_eSPI.h>
#include <ESP32Encoder.h>
#include <driver/i2s.h>

// Display
TFT_eSPI tft = TFT_eSPI();

// Rotary Encoders
ESP32Encoder encoders[5];

// Pin Definitions - OPTIMIZED for YOUR ESP32-S3-WROOM-1 board
// Using only pins confirmed available on your hardware
// Available: 4,5,6,7,15,16,17,18,8,3,46,9,10,11,12,13,14,1,2,42,41,40,39,38,37,36,35,0,45,48,47,21,20,19

// Display (5 pins) - Software SPI
const uint8_t TFT_CS = 10;
const uint8_t TFT_DC = 9;
const uint8_t TFT_RST = 14;
const uint8_t TFT_MOSI = 11;
const uint8_t TFT_SCK = 13;

// Rotary Encoders (15 pins) - All 5 encoders preserved
const uint8_t ENCODER_PINS[5][3] = {
    {4, 5, 6},       // Encoder 0: Tempo (CLK, DT, SW)
    {7, 15, 16},     // Encoder 1: Pitch
    {17, 18, 8},     // Encoder 2: Length  
    {1, 2, 42},      // Encoder 3: Envelope
    {41, 40, 39}     // Encoder 4: Swing
};

// Matrix Keyboard (8 pins) - Full 4x4 matrix preserved
const uint8_t MATRIX_ROWS[4] = {38, 37, 36, 35};
const uint8_t MATRIX_COLS[4] = {48, 47, 21, 46};

// Direct Buttons (3 pins) - Essential controls only
const uint8_t DIRECT_BUTTONS[3] = {45, 3, 12};
// Button 0: PLAY/STOP, Button 1: VOICE SELECT, Button 2: CLEAR/SHIFT

// I2S Audio Pins (3 pins) - Using USB pins (no USB debugging)
const uint8_t I2S_BCLK = 19;
const uint8_t I2S_LRCLK = 20;
const uint8_t I2S_DOUT = 0;  // GPIO 0 - CAUTION: may affect boot

// Synthesis Parameters
struct SynthParams {
    uint16_t tempo = 120;
    uint8_t pitch = 60;
    uint8_t length = 50;
    uint8_t envelope = 2;
    uint8_t swing = 0;
    uint8_t currentStep = 0;
    uint8_t currentVoice = 0;
    bool stepActive[16] = {false};
    uint8_t stepNotes[16] = {60};
    bool isPlaying = false;
    unsigned long lastStepTime = 0;
} synth;

// Function Prototypes
void initDisplay();
void initEncoders();
void initMatrix();
void initAudio();
void updateDisplay();
void scanEncoders();
void scanMatrix();
void processAudio();
void processSequencer();

void setup() {
    Serial.begin(115200);
    Serial.println("MintySynth ESP32-S3 Starting...");
    
    // Initialize subsystems
    initDisplay();
    initEncoders();
    initMatrix();
    initAudio();
    
    // Initial display update
    updateDisplay();
    
    Serial.println("MintySynth ESP32-S3 Ready!");
}

void loop() {
    scanEncoders();
    scanMatrix();
    processSequencer();
    processAudio();
    
    static unsigned long lastDisplayUpdate = 0;
    if (millis() - lastDisplayUpdate > 50) {  // 20fps display updates
        updateDisplay();
        lastDisplayUpdate = millis();
    }
    
    delay(1);
}

void initDisplay() {
    tft.init();
    tft.setRotation(1);  // Landscape mode (320x240)
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.drawString("MintySynth ESP32-S3", 10, 10);
    tft.setTextSize(1);
    tft.drawString("Arduino IDE Version", 10, 35);
    tft.drawString("Initializing...", 10, 50);
}

void initEncoders() {
    // Enable the weak pull up resistors
    ESP32Encoder::useInternalWeakPullResistors = UP;
    
    for (int i = 0; i < 5; i++) {
        encoders[i].attachFullQuad(ENCODER_PINS[i][0], ENCODER_PINS[i][1]);
        encoders[i].clearCount();
        pinMode(ENCODER_PINS[i][2], INPUT_PULLUP);  // Switch pin
    }
}

void initMatrix() {
    // Set up matrix keyboard
    for (int i = 0; i < 4; i++) {
        pinMode(MATRIX_ROWS[i], INPUT_PULLUP);
        pinMode(MATRIX_COLS[i], OUTPUT);
        digitalWrite(MATRIX_COLS[i], HIGH);
    }
    
    // Set up direct buttons
    for (int i = 0; i < 4; i++) {
        pinMode(DIRECT_BUTTONS[i], INPUT_PULLUP);
    }
}

void initAudio() {
    // Configure I2S for audio output
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = 44100,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };
    
    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BCLK,
        .ws_io_num = I2S_LRCLK,
        .data_out_num = I2S_DOUT,
        .data_in_num = I2S_PIN_NO_CHANGE
    };
    
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
}

void updateDisplay() {
    // Clear parameter area
    tft.fillRect(0, 70, 320, 170, TFT_BLACK);
    
    // Display current parameters
    tft.setTextSize(1);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawString("TEMPO: " + String(synth.tempo) + " BPM", 10, 80);
    tft.drawString("PITCH: " + String(synth.pitch) + " (" + noteToString(synth.pitch) + ")", 10, 95);
    tft.drawString("LENGTH: " + String(synth.length) + "%", 10, 110);
    tft.drawString("ENVELOPE: " + String(synth.envelope), 10, 125);
    tft.drawString("SWING: " + String(synth.swing) + "%", 10, 140);
    
    // Display play status
    tft.setTextColor(synth.isPlaying ? TFT_GREEN : TFT_RED, TFT_BLACK);
    tft.drawString(synth.isPlaying ? "PLAYING" : "STOPPED", 170, 80);
    
    // Display voice and step info
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawString("Voice: " + String(synth.currentVoice + 1) + "/4", 170, 110);
    tft.drawString("Step: " + String(synth.currentStep + 1) + "/16", 170, 125);
    
    // Display step grid
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("STEPS:", 10, 160);
    for (int i = 0; i < 16; i++) {
        int x = 10 + (i * 19);
        int y = 175;
        
        // Color coding: Active=Green, Current=Red, Inactive=Gray
        uint16_t color = TFT_DARKGREY;
        if (synth.stepActive[i]) {
            color = TFT_GREEN;
        }
        if (i == synth.currentStep && synth.isPlaying) {
            color = TFT_RED;
        }
        
        tft.fillRect(x, y, 16, 16, color);
        tft.drawRect(x, y, 16, 16, TFT_WHITE);
        
        // Step number
        tft.setTextColor(TFT_BLACK, color);
        tft.drawString(String(i + 1), x + 2, y + 4);
    }
    
    // Display current step note
    tft.setTextColor(TFT_MAGENTA, TFT_BLACK);
    tft.drawString("Current Note: " + noteToString(synth.stepNotes[synth.currentStep]), 10, 200);
}

String noteToString(uint8_t midiNote) {
    const char* noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    int octave = (midiNote / 12) - 1;
    int note = midiNote % 12;
    return String(noteNames[note]) + String(octave);
}

void scanEncoders() {
    // Check encoder switches first
    static bool lastSwitchState[5] = {true, true, true, true, true};
    for (int i = 0; i < 5; i++) {
        bool currentSwitchState = digitalRead(ENCODER_PINS[i][2]);
        if (!currentSwitchState && lastSwitchState[i]) {
            // Encoder switch pressed
            handleEncoderPress(i);
        }
        lastSwitchState[i] = currentSwitchState;
    }
    
    // Read encoder changes
    int32_t changes[5];
    for (int i = 0; i < 5; i++) {
        changes[i] = encoders[i].getCount();
        encoders[i].clearCount();
    }
    
    // Update parameters based on encoder changes
    if (changes[0] != 0) {  // Tempo
        synth.tempo = constrain(synth.tempo + changes[0], 60, 200);
    }
    if (changes[1] != 0) {  // Pitch
        synth.pitch = constrain(synth.pitch + changes[1], 24, 96);
        synth.stepNotes[synth.currentStep] = synth.pitch;
    }
    if (changes[2] != 0) {  // Length
        synth.length = constrain(synth.length + changes[2], 10, 100);
    }
    if (changes[3] != 0) {  // Envelope
        synth.envelope = constrain(synth.envelope + changes[3], 0, 4);
    }
    if (changes[4] != 0) {  // Swing
        synth.swing = constrain(synth.swing + changes[4], 0, 50);
    }
}

void handleEncoderPress(int encoder) {
    switch (encoder) {
        case 0:  // Tempo - Tap tempo (future feature)
            Serial.println("Tempo encoder pressed - Tap tempo");
            break;
        case 1:  // Pitch - Octave jump
            synth.pitch = ((synth.pitch / 12) + 1) * 12 + (synth.pitch % 12);
            if (synth.pitch > 96) synth.pitch = 24 + (synth.pitch % 12);
            synth.stepNotes[synth.currentStep] = synth.pitch;
            break;
        case 2:  // Length - Quantize to beat divisions
            if (synth.length < 25) synth.length = 25;
            else if (synth.length < 50) synth.length = 50;
            else if (synth.length < 75) synth.length = 75;
            else synth.length = 100;
            break;
        case 3:  // Envelope - Cycle through types
            synth.envelope = (synth.envelope + 1) % 5;
            break;
        case 4:  // Swing - Reset to 0
            synth.swing = 0;
            break;
    }
}

void scanMatrix() {
    static bool lastButtonState[20] = {false};
    bool currentButtonState[20] = {false};
    
    // Scan 4x4 matrix
    for (int col = 0; col < 4; col++) {
        digitalWrite(MATRIX_COLS[col], LOW);
        delayMicroseconds(10);
        
        for (int row = 0; row < 4; row++) {
            int buttonIndex = row * 4 + col;
            currentButtonState[buttonIndex] = !digitalRead(MATRIX_ROWS[row]);
        }
        
        digitalWrite(MATRIX_COLS[col], HIGH);
    }
    
    // Scan direct buttons
    for (int i = 0; i < 4; i++) {
        currentButtonState[16 + i] = !digitalRead(DIRECT_BUTTONS[i]);
    }
    
    // Process button presses (just pressed, not held)
    for (int i = 0; i < 20; i++) {
        if (currentButtonState[i] && !lastButtonState[i]) {
            if (i < 16) {
                // Step button pressed
                synth.currentStep = i;
                synth.stepActive[i] = !synth.stepActive[i];  // Toggle step
                Serial.println("Step " + String(i + 1) + " " + (synth.stepActive[i] ? "ON" : "OFF"));
            } else {
                // Direct button pressed
                switch (i - 16) {
                    case 0:  // PLAY/STOP
                        synth.isPlaying = !synth.isPlaying;
                        if (synth.isPlaying) {
                            synth.lastStepTime = millis();
                            synth.currentStep = 0;
                        }
                        Serial.println(synth.isPlaying ? "PLAY" : "STOP");
                        break;
                    case 1:  // VOICE SELECT
                        synth.currentVoice = (synth.currentVoice + 1) % 4;
                        Serial.println("Voice: " + String(synth.currentVoice + 1));
                        break;
                    case 2:  // CLEAR STEP
                        synth.stepActive[synth.currentStep] = false;
                        Serial.println("Cleared step " + String(synth.currentStep + 1));
                        break;
                    case 3:  // COPY STEP
                        // TODO: Implement step copy functionality
                        Serial.println("Copy step (not implemented)");
                        break;
                }
            }
        }
        lastButtonState[i] = currentButtonState[i];
    }
}

void processSequencer() {
    if (!synth.isPlaying) return;
    
    unsigned long stepDuration = 60000 / synth.tempo / 4;  // 16th notes
    
    if (millis() - synth.lastStepTime >= stepDuration) {
        // Advance to next step
        synth.currentStep = (synth.currentStep + 1) % 16;
        synth.lastStepTime = millis();
        
        // If this step is active, trigger note
        if (synth.stepActive[synth.currentStep]) {
            Serial.println("Trigger step " + String(synth.currentStep + 1) + 
                         " Note: " + noteToString(synth.stepNotes[synth.currentStep]));
        }
    }
}

void processAudio() {
    // TODO: Implement full synthesis engine from original MintySynth
    // For now, generate simple test tone based on current step
    
    static int16_t audioBuffer[128];
    size_t bytesWritten;
    
    // Generate audio based on active step
    static float phase = 0;
    float frequency = 440.0;
    
    if (synth.isPlaying && synth.stepActive[synth.currentStep]) {
        frequency = 440.0 * pow(2.0, (synth.stepNotes[synth.currentStep] - 69) / 12.0);
    } else {
        frequency = 0;  // Silence
    }
    
    for (int i = 0; i < 128; i += 2) {
        int16_t sample = 0;
        if (frequency > 0) {
            sample = (int16_t)(sin(phase) * 4000 * (synth.length / 100.0));
        }
        
        audioBuffer[i] = sample;      // Left channel
        audioBuffer[i + 1] = sample;  // Right channel
        
        if (frequency > 0) {
            phase += 2.0 * PI * frequency / 44100.0;
            if (phase > 2.0 * PI) phase -= 2.0 * PI;
        }
    }
    
    i2s_write(I2S_NUM_0, audioBuffer, sizeof(audioBuffer), &bytesWritten, portMAX_DELAY);
}