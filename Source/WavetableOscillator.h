#pragma once

#include <JuceHeader.h>

class WavetableOscillator
{
public:
    WavetableOscillator(std::shared_ptr<juce::AudioSampleBuffer>& table, unsigned int cycles);

    void setFrequency(float frequency, float sampleRate);
    
    float getNextSample() noexcept;
    float getDelta();
    
    void reset();
    
private:
    std::shared_ptr<juce::AudioSampleBuffer>& table;
    
    const unsigned int cycles;

    float delta;
    float currentIndex;
    float lastSample;
};
