#include "WavetableVoice.h"

using namespace juce;
using namespace std;

WavetableVoice::WavetableVoice(SynthEngine& engine, AudioSampleBuffer& t, unsigned int c) : synthEngine(engine), cycles(c), wavetable(t)
{
    setOscillators(12);
}

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
    
    ratios.clear();
    
    for (int i = 0; i < numVoices; i++) {
        oscillators.push_back(WavetableOscillator(cycles));
        
        if (i >= 1)
        {
            ratios.push_back(1);
        }
    }
}

void WavetableVoice::setCurrentPlaybackSampleRate(double rate)
{
    SynthesiserVoice::setCurrentPlaybackSampleRate(rate);
    
    if (rate < 1)
    {
        return;
    }
    
    currentFrequency.reset(rate, 0.05);
    
    currentGain.reset(rate, 0.05);
}

bool WavetableVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return true;
}

void WavetableVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    auto& parameters = synthEngine.getParameters();
    
    auto& pitchMapper = synthEngine.getPitchMapper();
    
    auto gain = Decibels::decibelsToGain((float) *synthEngine.getParameters().gainParameter);
    
    initialFrequency = synthEngine.map(midiNoteNumber);
    
    currentFrequency.setCurrentAndTargetValue(initialFrequency);
    
    currentGain.setTargetValue(gain);
    
    level = midiNoteNumber == synthEngine.getPedalNote() ? 0.8 : velocity;
    
    tailOff = 0;
    
    oscillators[0].setFrequency(currentFrequency.getNextValue(), wavetable.getNumSamples(), getSampleRate());
    
    auto superimpose = parameters.superimposeParameter;

    if (*superimpose != 12 && !pitchMapper.isSubstituted(midiNoteNumber))
    {
        for (int i = 1; i < *parameters.numVoicesParameter + 1; i++)
        {
            auto super = (int) *superimpose * i;
            
            auto freq = pitchMapper.mapRelative(midiNoteNumber, super, false);
            
            auto ratio = freq / initialFrequency;
            
            ratios[i - 1] = ratio;
            
            oscillators[i].setFrequency(currentFrequency.getCurrentValue() * ratio, wavetable.getNumSamples(), getSampleRate());
        }
    }
    else
    {
        for (int i = 1; i < *parameters.numVoicesParameter + 1; i++)
        {
            oscillators[i].reset();
        }
    }
}

void WavetableVoice::stopNote(float velocity, bool allowTailOff)
{
    if (allowTailOff)
    {
        if (tailOff == 0)
        {
            tailOff = 1;
        }
    }
    else
    {
        reset();
    }
}

void WavetableVoice::pitchWheelMoved(int newPitchWheelValue)
{
    /*
    int range = 12;
    float valuePerSemitone = 8192 / range;
    float transpose = (newPitchWheelValue / valuePerSemitone) - range;

    TODO -- add pitch wheel functionality
    */
}

void WavetableVoice::controllerMoved(int controllerNumber, int newControllerValue) {}

void WavetableVoice::renderNextBlock(AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    if (oscillators.size() < 1)
    {
        jassertfalse;
    }
    
    if (oscillators[0].getDelta() != 0)
    {
        auto& parameters = synthEngine.getParameters();
        
        auto gain = Decibels::decibelsToGain((float) *synthEngine.getParameters().gainParameter);
        
        currentGain.setTargetValue(gain);
        
        while (numSamples > 0)
        {
            auto next = currentFrequency.getNextValue();
            
            oscillators[0].setFrequency(next, wavetable.getNumSamples(), getSampleRate());
            
            float nextSample = oscillators[0].getNextSample(wavetable);
            
            if (*parameters.superimposeParameter != 12)
            {
                for (int i = 1; i < *parameters.numVoicesParameter + 1; i++)
                {
                    if (oscillators[i].getDelta() != 0)
                    {
                        oscillators[i].setFrequency(next * ratios[i - 1], wavetable.getNumSamples(), getSampleRate());
                        
                        nextSample += oscillators[i].getNextSample(wavetable) * *parameters.mixParameter;
                    }
                }
            }
            
            if (tailOff > 0)
            {
                tailOff *= 0.99;
                nextSample *= tailOff;
                
                if (tailOff <= 0.05)
                {
                    reset();
                    
                    break;
                }
            }
            
            nextSample *= level * currentGain.getNextValue();
            
            for (int i = outputBuffer.getNumChannels() - 1; i >= 0; i--)
            {
                outputBuffer.addSample(i, startSample, nextSample);
            }
            
            startSample++;
            numSamples--;
        }
    }
}

void WavetableVoice::setFrequency(double hz)
{
    currentFrequency.setTargetValue(hz);
}

void WavetableVoice::reset()
{
    clearCurrentNote();
    
    for (auto& o : oscillators) {
        o.reset();
    }
}

bool WavetableSound::appliesToNote(int midiNoteNumber)
{
    return true;
}

bool WavetableSound::appliesToChannel(int midiChannel)
{
    return true;
}
