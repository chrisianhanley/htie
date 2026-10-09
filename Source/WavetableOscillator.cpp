#include "WavetableOscillator.h"

using namespace std;

WavetableOscillator::WavetableOscillator(shared_ptr<juce::AudioSampleBuffer> table, unsigned int c) : wavetable(table), cycles(c), delta(0), currentIndex(0), lastSample(0)
{
    jassert(table);
    
    jassert(table->getNumChannels() == 1);
}

void WavetableOscillator::setFrequency(float hz, float sampleRate)
{
    float cycle = atomic_load(&wavetable).get()->getNumSamples() / cycles;
    
    delta = hz * (cycle / sampleRate);
}

float WavetableOscillator::getDelta()
{
    return delta;
}

float WavetableOscillator::getNextSample() noexcept
{
    auto wt = atomic_load(&wavetable);
    
    if (!wt)
    {
        return lastSample;
    }
    
    if (wt.get()->hasBeenCleared())
    {
        return lastSample;
    }
    
    auto tableSize = wt.get()->getNumSamples() - 1;
    auto index0 = (unsigned int) currentIndex;
    auto index1 = index0 + 1;

    auto frac = currentIndex - (float) index0;
    
    auto table = wavetable.get()->getReadPointer(0);
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

void WavetableOscillator::reset()
{
    delta = 0;
    currentIndex = 0;
}
