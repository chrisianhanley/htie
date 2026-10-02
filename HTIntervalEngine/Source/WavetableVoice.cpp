#include "WavetableVoice.h"

using namespace juce;
using namespace std;

WavetableVoice::WavetableVoice(SoundProfile& profile, AudioSampleBuffer& table, unsigned int c) : soundProfile(profile), cycles(c) {}

WavetableVoice::~WavetableVoice() {}

vector<WavetableOscillator>& WavetableVoice::getOscillators()
{
    return oscillators;
}
