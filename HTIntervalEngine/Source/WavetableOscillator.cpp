#include "WavetableOscillator.h"

using namespace std;

WavetableOscillator::WavetableOscillator(juce::AudioSampleBuffer& table, unsigned int c) : wavetable(table), cycles(c)
{
    jassert(wavetable.getNumChannels() == 1);
}

void WavetableOscillator::setTable(juce::AudioSampleBuffer& table)
{
    wavetable = table;
}

void WavetableOscillator::setFrequency(float hz, float sampleRate)
{
    float cycle = wavetable.getNumSamples() / cycles;
    
    delta = hz * (cycle / sampleRate);
}

float WavetableOscillator::getDelta()
{
    return delta;
}

float WavetableOscillator::getNextSample() noexcept
{
    if (wavetable.hasBeenCleared())
    {
        return lastSample;
    }
    
    auto tableSize = wavetable.getNumSamples() - 1;
    auto index0 = (unsigned int) currentIndex;
    auto index1 = index0 + 1;

    auto frac = currentIndex - (float) index0;
    
    auto table = wavetable.getReadPointer(0);
    auto value0 = table[index0];
    auto value1 = table[index1];
    
    auto currentSample = value0 + frac * (value1 - value0);
    
    if ((currentIndex += delta) > (float) tableSize)
    {
        currentIndex -= (float) tableSize;
    }
    
    lastSample = currentSample;
    
    return currentSample;
}

void WavetableOscillator::reset()
{
    delta = 0;
    currentIndex = 0;
}
