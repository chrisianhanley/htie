#include "WavetableVoice.h"

using namespace juce;
using namespace std;

WavetableVoice::WavetableVoice(SoundProfile& profile, AudioSampleBuffer& t, unsigned int c) : soundProfile(profile), table(t), cycles(c) {}

WavetableVoice::~WavetableVoice() {}

vector<WavetableOscillator>& WavetableVoice::getOscillators()
{
    return oscillators;
}

void WavetableVoice::setOscillators(unsigned int numVoices)
{
    if (numVoices < 1)
    {
        jassertfalse;
    }
    
    oscillators.clear();
    
    for (int i = 0; i < numVoices; i++) {
        oscillators.push_back(WavetableOscillator(cycles));
    }
}
