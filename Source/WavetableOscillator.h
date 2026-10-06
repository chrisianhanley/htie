#pragma once

#include <JuceHeader.h>

class WavetableOscillator
{
public:
    WavetableOscillator(juce::AudioSampleBuffer& table, unsigned int cycles);

    void setTable(juce::AudioSampleBuffer& table);
    void setFrequency(float frequency, float sampleRate);
    
    float getNextSample() noexcept;
    float getDelta();
    
    void reset();
    
private:
    juce::AudioSampleBuffer& wavetable;
    
    const unsigned int cycles;

    float delta;
    float currentIndex;
    float lastSample;
};
