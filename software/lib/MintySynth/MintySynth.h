/*
 * MintySynth Library Header
 * 
 * Core synthesis engine and parameter management
 * Based on original MintySynth by Andrew Mowry
 */

#ifndef MINTYSYNTH_H
#define MINTYSYNTH_H

#include <Arduino.h>

// Audio configuration
#define SAMPLE_RATE 44100
#define AUDIO_BUFFER_SIZE 128
#define NUM_VOICES 4
#define NUM_STEPS 16
#define NUM_WAVEFORMS 15

// Waveform types (from original MintySynth)
enum WaveformType {
    WAVE_SINE = 0,
    WAVE_RAMP = 1,
    WAVE_TRIANGLE = 2,
    WAVE_SQUARE = 3,
    WAVE_NOISE = 4,
    WAVE_SAW = 5,
    WAVE_A = 6,
    WAVE_B = 7,
    WAVE_C = 8,
    WAVE_D = 9,
    WAVE_E = 10,
    WAVE_F = 11,
    WAVE_G = 12,
    WAVE_H = 13,
    WAVE_I = 14
};

// Envelope types
enum EnvelopeType {
    ENV_ATTACK = 0,
    ENV_DECAY = 1,
    ENV_PLUCK = 2,
    ENV_LONG = 3,
    ENV_REVERSE = 4
};

// Voice parameters structure
struct VoiceParams {
    uint8_t waveform;       // 0-14 waveform type
    uint8_t pitch;          // 0-127 MIDI note
    uint8_t envelope;       // 0-4 envelope type
    uint8_t length;         // 0-127 note duration
    uint8_t modulation;     // 0-127 pitch modulation
    uint8_t volume;         // 0-127 volume level
    bool active;            // Voice active flag
};

// Step sequencer data
struct SequencerStep {
    uint8_t note;           // MIDI note number
    bool active;            // Step active/inactive
    uint8_t velocity;       // Note velocity
    uint8_t length;         // Note length override
};

// Global synthesis parameters
struct SynthParams {
    uint16_t tempo;         // BPM
    uint8_t swing;          // Swing amount 0-127
    uint8_t scale;          // Scale type 0-8
    int8_t transpose;       // Global transpose -12 to +12
    uint8_t masterVolume;   // Master volume 0-127
};

class MintySynth {
public:
    MintySynth();
    
    // Initialization
    void begin();
    void setAudioCallback(void (*callback)(int16_t*, size_t));
    
    // Voice control
    void setVoiceParam(uint8_t voice, uint8_t param, uint8_t value);
    uint8_t getVoiceParam(uint8_t voice, uint8_t param);
    void triggerVoice(uint8_t voice, uint8_t note, uint8_t velocity = 127);
    void releaseVoice(uint8_t voice);
    
    // Sequencer control
    void setStep(uint8_t voice, uint8_t step, uint8_t note, bool active = true);
    void clearStep(uint8_t voice, uint8_t step);
    bool isStepActive(uint8_t voice, uint8_t step);
    void setTempo(uint16_t bpm);
    void start();
    void stop();
    bool isPlaying();
    
    // Global parameters
    void setGlobalParam(uint8_t param, uint16_t value);
    uint16_t getGlobalParam(uint8_t param);
    
    // Audio processing
    void processAudio(int16_t* buffer, size_t length);
    void updateSequencer();
    
    // Preset management
    void savePreset(uint8_t slot);
    void loadPreset(uint8_t slot);
    
private:
    VoiceParams voices[NUM_VOICES];
    SequencerStep sequence[NUM_VOICES][NUM_STEPS];
    SynthParams globals;
    
    bool playing;
    uint8_t currentStep;
    unsigned long lastStepTime;
    unsigned long stepDuration;
    
    // Audio synthesis
    float voicePhase[NUM_VOICES];
    float voiceFreq[NUM_VOICES];
    uint16_t voiceEnvPhase[NUM_VOICES];
    bool voiceActive[NUM_VOICES];
    
    // Internal methods
    void calculateStepDuration();
    float getWaveformSample(uint8_t waveform, float phase);
    float getEnvelopeSample(uint8_t envelope, uint16_t phase);
    uint16_t noteToFrequency(uint8_t note);
    void updateVoiceFrequencies();
};

// Parameter indices for setVoiceParam/getVoiceParam
#define PARAM_WAVEFORM    0
#define PARAM_PITCH       1
#define PARAM_ENVELOPE    2
#define PARAM_LENGTH      3
#define PARAM_MODULATION  4
#define PARAM_VOLUME      5

// Parameter indices for setGlobalParam/getGlobalParam
#define GLOBAL_TEMPO      0
#define GLOBAL_SWING      1
#define GLOBAL_SCALE      2
#define GLOBAL_TRANSPOSE  3
#define GLOBAL_VOLUME     4

#endif // MINTYSYNTH_H