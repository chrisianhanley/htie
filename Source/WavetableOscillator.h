#pragma once

#include <JuceHeader.h>

class WavetableOscillator
{
public:
    WavetableOscillator(unsigned int cycles);

    void setFrequency(float frequency, float numSamples, float sampleRate);
    
    float getNextSample(juce::AudioSampleBuffer& table) noexcept;
    float getDelta();
    
    void reset();
    
private:
    const unsigned int cycles;

    float delta;
    float currentIndex;
    float lastSample;
};
