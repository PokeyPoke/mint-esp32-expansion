/*
 * MintySynth ESP32-S3 Expansion - PlatformIO Version
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
 * Based on original MintySynth by Andrew Mowry
 * http://mintysynth.com
 * 
 * License: GPL v3
 */

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <ESP32Encoder.h>
#include <driver/i2s.h>
#include "MintySynth.h"

// Display
TFT_eSPI tft = TFT_eSPI();

// Rotary Encoders
ESP32Encoder encoders[5];

// Pin Definitions - CORRECTED for ESP32-S3 constraints
const uint8_t ENCODER_PINS[5][3] = {
    {1, 2, 4},      // Encoder 0: Tempo (CLK, DT, SW)
    {5, 6, 7},      // Encoder 1: Pitch
    {8, 9, 16},     // Encoder 2: Length  
    {17, 18, 21},   // Encoder 3: Envelope
    {47, 48, 38}    // Encoder 4: Swing
};

const uint8_t MATRIX_ROWS[4] = {39, 40, 41, 42};
const uint8_t MATRIX_COLS[4] = {26, 27, 28, 29};
const uint8_t DIRECT_BUTTONS[4] = {30, 31, 32, 33};

// I2S Audio Pins
const uint8_t I2S_BCLK = 34;
const uint8_t I2S_LRCLK = 35;
const uint8_t I2S_DOUT = 36;

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
    tft.setRotation(1);  // Landscape mode
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.drawString("MintySynth ESP32-S3", 10, 10);
    tft.setTextSize(1);
    tft.drawString("Initializing...", 10, 40);
}

void initEncoders() {
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
    // Clear display area
    tft.fillRect(0, 50, 320, 190, TFT_BLACK);
    
    // Display current parameters
    tft.setTextSize(1);
    tft.drawString("TEMPO: " + String(synth.tempo), 10, 60);
    tft.drawString("PITCH: " + String(synth.pitch), 10, 80);
    tft.drawString("LENGTH: " + String(synth.length), 10, 100);
    tft.drawString("ENVELOPE: " + String(synth.envelope), 10, 120);
    tft.drawString("SWING: " + String(synth.swing), 10, 140);
    
    // Display step grid
    tft.drawString("STEPS:", 10, 170);
    for (int i = 0; i < 16; i++) {
        int x = 60 + (i * 15);
        int y = 170;
        
        if (synth.stepActive[i]) {
            tft.fillRect(x, y, 12, 12, TFT_GREEN);
        } else {
            tft.drawRect(x, y, 12, 12, TFT_WHITE);
        }
        
        // Highlight current step
        if (i == synth.currentStep) {
            tft.drawRect(x-1, y-1, 14, 14, TFT_RED);
        }
    }
    
    // Display current voice and step
    tft.drawString("Voice: " + String(synth.currentVoice + 1), 10, 200);
    tft.drawString("Step: " + String(synth.currentStep + 1), 100, 200);
}

void scanEncoders() {
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
            } else {
                // Direct button pressed
                switch (i - 16) {
                    case 0:  // PLAY/STOP
                        // TODO: Implement play/stop
                        break;
                    case 1:  // VOICE SELECT
                        synth.currentVoice = (synth.currentVoice + 1) % 4;
                        break;
                    case 2:  // CLEAR STEP
                        synth.stepActive[synth.currentStep] = false;
                        break;
                    case 3:  // COPY STEP
                        // TODO: Implement step copy
                        break;
                }
            }
        }
        lastButtonState[i] = currentButtonState[i];
    }
}

void processAudio() {
    // TODO: Implement audio synthesis and I2S output
    // This is a placeholder for the synthesis engine
    static int16_t audioBuffer[64];
    size_t bytesWritten;
    
    // Generate simple test tone for now
    static float phase = 0;
    float frequency = 440.0 * pow(2.0, (synth.pitch - 69) / 12.0);
    
    for (int i = 0; i < 64; i += 2) {
        int16_t sample = (int16_t)(sin(phase) * 8000);
        audioBuffer[i] = sample;      // Left channel
        audioBuffer[i + 1] = sample;  // Right channel
        phase += 2.0 * PI * frequency / 44100.0;
        if (phase > 2.0 * PI) phase -= 2.0 * PI;
    }
    
    i2s_write(I2S_NUM_0, audioBuffer, sizeof(audioBuffer), &bytesWritten, 0);
}