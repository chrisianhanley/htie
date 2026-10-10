#pragma once

#include "JuceHeader.h"
#include "SynthEngine.h"

class WavetableVoice : public juce::SynthesiserVoice
{
public:
    WavetableVoice(SynthEngine& engine, juce::AudioSampleBuffer& table, unsigned int cycles);
    
    ~WavetableVoice();
    
    bool canPlaySound(juce::SynthesiserSound* sound) override;
    
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    
    void stopNote(float velocity, bool allowTailOff) override;
    
    void pitchWheelMoved(int newPitchWheelValue) override;
    
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    
    void setCurrentPlaybackSampleRate(double rate) override;

    void setFrequency(double frequency);
    
    std::vector<WavetableOscillator>& getOscillators();
    
    virtual void setOscillators(unsigned int numVoices);
    
    void reset();
    
protected:
    SynthEngine& synthEngine;
    
    std::vector<WavetableOscillator> oscillators;
    
    unsigned int cycles;
    
    float initialFrequency;
    
    float level;
    
    float tailOff;
    
    std::vector<float> ratios;
    
    juce::SmoothedValue<float> currentFrequency;
    
    juce::SmoothedValue<float> currentGain;
    
    juce::AudioSampleBuffer& wavetable;
};

class WavetableSound : public juce::SynthesiserSound
{
    bool appliesToNote(int midiNoteNumber) override;
    
    bool appliesToChannel(int midiChannel) override;
};
