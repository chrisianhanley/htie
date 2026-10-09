#include "WavetableOscillator.h"

using namespace std;

WavetableOscillator::WavetableOscillator(unsigned int c) : cycles(c), delta(0), currentIndex(0), lastSample(0) {}

void WavetableOscillator::setFrequency(float hz, float numSamples, float sampleRate)
{
    float cycle = numSamples / cycles;
    
    delta = hz * (cycle / sampleRate);
}

float WavetableOscillator::getDelta()
{
    return delta;
}

float WavetableOscillator::getNextSample(juce::AudioSampleBuffer& wt) noexcept
{
    if (wt.hasBeenCleared())
    {
        return lastSample;
    }
    
    try
    {
        auto tableSize = wt.getNumSamples() - 1;
        
        if (currentIndex >= tableSize)
        {
            currentIndex = 0;
        }
        
        auto index0 = (unsigned int) currentIndex;
        auto index1 = index0 + 1;

        auto frac = currentIndex - (float) index0;
        
        auto table = wt.getReadPointer(0);
        auto value0 = table[index0];
        auto value1 = table[index1];
        
        auto currentSample = value0 + frac * (value1 - value0);
        
        if ((currentIndex += delta) >= (float) tableSize)
        {
            currentIndex -= (float) tableSize;
        }
        
        lastSample = currentSample;
        
        return currentSample;
    }
    catch (const exception& e)
    {
        return lastSample;
    }
}

void WavetableOscillator::reset()
{
    delta = 0;
    currentIndex = 0;
}
