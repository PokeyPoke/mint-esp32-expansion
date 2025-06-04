/*
 * MintySynth Library Implementation
 * 
 * Core synthesis engine based on original MintySynth
 */

#include "MintySynth.h"
#include <math.h>

// Waveform tables (simplified versions - full tables would be in PROGMEM)
const float SINE_TABLE[256] = {
    // Sine wave lookup table would go here
    // For now, we'll calculate on the fly
};

MintySynth::MintySynth() {
    playing = false;
    currentStep = 0;
    lastStepTime = 0;
    
    // Initialize voices
    for (int i = 0; i < NUM_VOICES; i++) {
        voices[i].waveform = WAVE_SINE;
        voices[i].pitch = 60;
        voices[i].envelope = ENV_PLUCK;
        voices[i].length = 50;
        voices[i].modulation = 64;
        voices[i].volume = 100;
        voices[i].active = false;
        
        voicePhase[i] = 0;
        voiceFreq[i] = 440.0;
        voiceEnvPhase[i] = 0;
        voiceActive[i] = false;
    }
    
    // Initialize sequencer
    for (int v = 0; v < NUM_VOICES; v++) {
        for (int s = 0; s < NUM_STEPS; s++) {
            sequence[v][s].note = 60;
            sequence[v][s].active = false;
            sequence[v][s].velocity = 127;
            sequence[v][s].length = 50;
        }
    }
    
    // Initialize global parameters
    globals.tempo = 120;
    globals.swing = 0;
    globals.scale = 0;
    globals.transpose = 0;
    globals.masterVolume = 100;
    
    calculateStepDuration();
}

void MintySynth::begin() {
    // Initialize synthesis engine
    updateVoiceFrequencies();
}

void MintySynth::setVoiceParam(uint8_t voice, uint8_t param, uint8_t value) {
    if (voice >= NUM_VOICES) return;
    
    switch (param) {
        case PARAM_WAVEFORM:
            voices[voice].waveform = constrain(value, 0, NUM_WAVEFORMS - 1);
            break;
        case PARAM_PITCH:
            voices[voice].pitch = constrain(value, 0, 127);
            break;
        case PARAM_ENVELOPE:
            voices[voice].envelope = constrain(value, 0, 4);
            break;
        case PARAM_LENGTH:
            voices[voice].length = constrain(value, 0, 127);
            break;
        case PARAM_MODULATION:
            voices[voice].modulation = constrain(value, 0, 127);
            break;
        case PARAM_VOLUME:
            voices[voice].volume = constrain(value, 0, 127);
            break;
    }
    
    updateVoiceFrequencies();
}

uint8_t MintySynth::getVoiceParam(uint8_t voice, uint8_t param) {
    if (voice >= NUM_VOICES) return 0;
    
    switch (param) {
        case PARAM_WAVEFORM: return voices[voice].waveform;
        case PARAM_PITCH: return voices[voice].pitch;
        case PARAM_ENVELOPE: return voices[voice].envelope;
        case PARAM_LENGTH: return voices[voice].length;
        case PARAM_MODULATION: return voices[voice].modulation;
        case PARAM_VOLUME: return voices[voice].volume;
        default: return 0;
    }
}

void MintySynth::triggerVoice(uint8_t voice, uint8_t note, uint8_t velocity) {
    if (voice >= NUM_VOICES) return;
    
    voices[voice].pitch = note;
    voiceActive[voice] = true;
    voiceEnvPhase[voice] = 0;
    voicePhase[voice] = 0;
    
    // Calculate frequency for this note
    voiceFreq[voice] = 440.0 * pow(2.0, (note - 69) / 12.0);
}

void MintySynth::releaseVoice(uint8_t voice) {
    if (voice >= NUM_VOICES) return;
    voiceActive[voice] = false;
}

void MintySynth::setStep(uint8_t voice, uint8_t step, uint8_t note, bool active) {
    if (voice >= NUM_VOICES || step >= NUM_STEPS) return;
    
    sequence[voice][step].note = note;
    sequence[voice][step].active = active;
}

void MintySynth::clearStep(uint8_t voice, uint8_t step) {
    if (voice >= NUM_VOICES || step >= NUM_STEPS) return;
    sequence[voice][step].active = false;
}

bool MintySynth::isStepActive(uint8_t voice, uint8_t step) {
    if (voice >= NUM_VOICES || step >= NUM_STEPS) return false;
    return sequence[voice][step].active;
}

void MintySynth::setTempo(uint16_t bpm) {
    globals.tempo = constrain(bpm, 60, 200);
    calculateStepDuration();
}

void MintySynth::start() {
    playing = true;
    currentStep = 0;
    lastStepTime = millis();
}

void MintySynth::stop() {
    playing = false;
    // Release all voices
    for (int i = 0; i < NUM_VOICES; i++) {
        voiceActive[i] = false;
    }
}

bool MintySynth::isPlaying() {
    return playing;
}

void MintySynth::setGlobalParam(uint8_t param, uint16_t value) {
    switch (param) {
        case GLOBAL_TEMPO:
            setTempo(value);
            break;
        case GLOBAL_SWING:
            globals.swing = constrain(value, 0, 127);
            calculateStepDuration();
            break;
        case GLOBAL_SCALE:
            globals.scale = constrain(value, 0, 8);
            break;
        case GLOBAL_TRANSPOSE:
            globals.transpose = constrain((int16_t)value, -12, 12);
            updateVoiceFrequencies();
            break;
        case GLOBAL_VOLUME:
            globals.masterVolume = constrain(value, 0, 127);
            break;
    }
}

uint16_t MintySynth::getGlobalParam(uint8_t param) {
    switch (param) {
        case GLOBAL_TEMPO: return globals.tempo;
        case GLOBAL_SWING: return globals.swing;
        case GLOBAL_SCALE: return globals.scale;
        case GLOBAL_TRANSPOSE: return (uint16_t)(globals.transpose + 12);
        case GLOBAL_VOLUME: return globals.masterVolume;
        default: return 0;
    }
}

void MintySynth::updateSequencer() {
    if (!playing) return;
    
    if (millis() - lastStepTime >= stepDuration) {
        // Advance to next step
        currentStep = (currentStep + 1) % NUM_STEPS;
        lastStepTime = millis();
        
        // Trigger notes for active steps
        for (int voice = 0; voice < NUM_VOICES; voice++) {
            if (sequence[voice][currentStep].active) {
                triggerVoice(voice, 
                           sequence[voice][currentStep].note + globals.transpose,
                           sequence[voice][currentStep].velocity);
            }
        }
    }
}

void MintySynth::processAudio(int16_t* buffer, size_t length) {
    for (size_t i = 0; i < length; i += 2) {
        float mixL = 0, mixR = 0;
        
        // Process each voice
        for (int voice = 0; voice < NUM_VOICES; voice++) {
            if (!voiceActive[voice]) continue;
            
            // Generate waveform sample
            float sample = getWaveformSample(voices[voice].waveform, voicePhase[voice]);
            
            // Apply envelope
            float envSample = getEnvelopeSample(voices[voice].envelope, voiceEnvPhase[voice]);
            sample *= envSample;
            
            // Apply voice volume
            sample *= (voices[voice].volume / 127.0);
            
            // Mix to stereo (simple center for now)
            mixL += sample;
            mixR += sample;
            
            // Update phase
            voicePhase[voice] += (voiceFreq[voice] * 2.0 * PI) / SAMPLE_RATE;
            if (voicePhase[voice] > 2.0 * PI) {
                voicePhase[voice] -= 2.0 * PI;
            }
            
            // Update envelope
            voiceEnvPhase[voice]++;
            
            // Check if envelope finished
            uint16_t envLength = (voices[voice].length * SAMPLE_RATE) / 1000;
            if (voiceEnvPhase[voice] > envLength) {
                voiceActive[voice] = false;
            }
        }
        
        // Apply master volume
        mixL *= (globals.masterVolume / 127.0);
        mixR *= (globals.masterVolume / 127.0);
        
        // Convert to 16-bit integer
        buffer[i] = (int16_t)(constrain(mixL * 16000, -32767, 32767));     // Left
        buffer[i + 1] = (int16_t)(constrain(mixR * 16000, -32767, 32767)); // Right
    }
}

void MintySynth::calculateStepDuration() {
    // Calculate step duration in milliseconds (16th notes)
    stepDuration = (60000 / globals.tempo) / 4;
    
    // Apply swing (affects even/odd steps differently)
    // This is a simplified implementation
}

float MintySynth::getWaveformSample(uint8_t waveform, float phase) {
    switch (waveform) {
        case WAVE_SINE:
            return sin(phase);
        case WAVE_SQUARE:
            return (phase < PI) ? 1.0 : -1.0;
        case WAVE_SAW:
            return (2.0 * phase / (2.0 * PI)) - 1.0;
        case WAVE_TRIANGLE:
            if (phase < PI) {
                return (2.0 * phase / PI) - 1.0;
            } else {
                return 3.0 - (2.0 * phase / PI);
            }
        case WAVE_NOISE:
            return ((float)random(-32767, 32767)) / 32767.0;
        default:
            return sin(phase); // Default to sine
    }
}

float MintySynth::getEnvelopeSample(uint8_t envelope, uint16_t phase) {
    // Simplified envelope implementation
    // Real implementation would use lookup tables from original MintySynth
    
    float normalizedPhase = phase / 1000.0; // Rough normalization
    
    switch (envelope) {
        case ENV_ATTACK:
            return (normalizedPhase < 1.0) ? normalizedPhase : 1.0;
        case ENV_DECAY:
            return (normalizedPhase < 1.0) ? (1.0 - normalizedPhase) : 0.0;
        case ENV_PLUCK:
            return exp(-normalizedPhase * 3.0);
        case ENV_LONG:
            return (normalizedPhase < 2.0) ? 1.0 : exp(-(normalizedPhase - 2.0));
        case ENV_REVERSE:
            return (normalizedPhase < 1.0) ? (1.0 - normalizedPhase) : 0.0;
        default:
            return exp(-normalizedPhase * 2.0);
    }
}

void MintySynth::updateVoiceFrequencies() {
    for (int i = 0; i < NUM_VOICES; i++) {
        float note = voices[i].pitch + globals.transpose;
        voiceFreq[i] = 440.0 * pow(2.0, (note - 69) / 12.0);
    }
}