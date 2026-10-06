#include "PolyphonicSynthesiser.h"
#include "PolyphonicSynthesiserWindow.h"
#include "PluginParameters.h"
#include "PitchMapper.h"

using namespace juce;
using namespace std;

PolyphonicSynthesiserVoice::PolyphonicSynthesiserVoice(SoundProfile& profile, AudioSampleBuffer& table) : WavetableVoice(profile, table, 4)
{
    if (*PluginParameters::numVoicesParameter < 1)
    {
        jassertfalse;
    }
    
    setOscillators(12);
}

void PolyphonicSynthesiserVoice::setOscillators(unsigned int numVoices)
{
    if (numVoices < 1)
    {
        jassertfalse;
    }
    
    oscillators.clear();
    ratios.clear();
    
    for (int i = 0; i < numVoices; i++) {
        oscillators.push_back(WavetableOscillator(table, cycles));
        
        if (i >= 1)
        {
            ratios.push_back(1);
        }
    }
}

bool PolyphonicSynthesiserVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return true;
}

void PolyphonicSynthesiserVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    auto& engine = soundProfile.getSynthEngine();
    
    initialFrequency = engine.map(midiNoteNumber);
    
    oscillators[0].setFrequency(initialFrequency, getSampleRate());
    
    frequency.setCurrentAndTargetValue(initialFrequency);
    gain.setCurrentAndTargetValue(PluginParameters::gainValue);
    
    level = midiNoteNumber == engine.getPedalNote() ? 0.8 : velocity;
    tailOff = 0;
    
    auto superimpose = PluginParameters::superimposeParameter;
    if (!PitchMapper::isSubstituted(midiNoteNumber) && *superimpose != 12)
    {
        for (int i = 1; i < *PluginParameters::numVoicesParameter + 1; i++)
        {
            auto super = (int) *superimpose * i;
            
            super %= 12;
            
            auto freq = PitchMapper::mapRelative(midiNoteNumber, super, false);
            
            ratios[i - 1] = freq / initialFrequency;
            
            oscillators[i].setFrequency(freq, getSampleRate());
        }
    }
}

void PolyphonicSynthesiserVoice::stopNote(float velocity, bool allowTailOff)
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

void PolyphonicSynthesiserVoice::pitchWheelMoved(int newPitchWheelValue)
{
    int range = 12;
    float valuePerSemitone = 8192 / range;
    float transpose = (newPitchWheelValue / valuePerSemitone) - range;
    
    frequency.setTargetValue(soundProfile.getSynthEngine().map(initialFrequency, transpose));
}

void PolyphonicSynthesiserVoice::controllerMoved(int controllerNumber, int newControllerValue) {}

void PolyphonicSynthesiserVoice::renderNextBlock(AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    if (oscillators.size() < 1)
    {
        jassertfalse;
    }
    
    if (oscillators[0].getDelta() != 0)
    {
        gain.setTargetValue(PluginParameters::gainValue);
        
        while (numSamples > 0)
        {
            oscillators[0].setFrequency(frequency.getCurrentValue(), getSampleRate());
            
            float nextSample = oscillators[0].getNextSample();
            
            if (*PluginParameters::superimposeParameter != 12)
            {
                for (int i = 1; i < *PluginParameters::numVoicesParameter + 1; i++)
                {
                    if (oscillators[i].getDelta() != 0)
                    {
                        oscillators[i].setFrequency(frequency.getCurrentValue() * ratios[i - 1], getSampleRate());
                        
                        nextSample += oscillators[i].getNextSample() * *PluginParameters::mixParameter;
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
            
            nextSample *= level * gain.getCurrentValue();
            
            for (int i = outputBuffer.getNumChannels() - 1; i >= 0; i--)
            {
                outputBuffer.addSample(i, startSample, nextSample);
            }
            
            startSample++;
            numSamples--;
        }
    }
}

void PolyphonicSynthesiserVoice::setFrequency(double hz)
{
    frequency.setTargetValue(hz);
}

void PolyphonicSynthesiserVoice::reset()
{
    clearCurrentNote();
    
    for (auto o : oscillators) {
        o.reset();
    }
}

bool PolyphonicSynthesiserSound::appliesToNote(int midiNoteNumber)
{
    return true;
}

bool PolyphonicSynthesiserSound::appliesToChannel(int midiChannel)
{
    return true;
}

PolyphonicSynthesiser::PolyphonicSynthesiser(SynthEngine& engine, unsigned int i) : SoundProfile(engine, i), numVoices(8)
{
    createTable();
}

PolyphonicSynthesiser::~PolyphonicSynthesiser()
{
    wavetable.clear();
}

juce::String PolyphonicSynthesiser::getDisplayName()
{
    return "polyphonic synthesizer";
}

Window* PolyphonicSynthesiser::createWindow(juce::AudioProcessorEditor& parent)
{
    return new PolyphonicSynthesiserWindow(parent, *this);
}

void PolyphonicSynthesiser::enable()
{
    auto& synth = synthEngine.getSynth();
    
    for (int i = 0; i < numVoices; i++)
    {
        synth.addVoice(new PolyphonicSynthesiserVoice(*this, wavetable));
    }
    
    synth.addSound(new PolyphonicSynthesiserSound());
}

void PolyphonicSynthesiser::update()
{
    createTable();
}

void PolyphonicSynthesiser::disable()
{
    synthEngine.getSynth().clearVoices();
}

template <typename T>
int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

void PolyphonicSynthesiser::createTable()
{
    wavetable.clear();
    
    auto size = PluginParameters::wavetableResolutionValue.load();
    
    auto totalSize = size * 4; // size multiplied by num cycles
    
    auto period = MathConstants<double>::twoPi;
    auto delta = period / (size - 1);
    auto angle = 0.0;
    
    wavetable.setSize(1, totalSize + 1);
    
    auto samples = wavetable.getWritePointer(0);
    
    float sawWeight = *Mixer::x1;
    
    float triangleWeight = *Mixer::x2;
    
    float squareWeight = *Mixer::x3;
    
    float square1Weight = *Mixer::x4;
    
    float square2Weight = *Mixer::x5;
    
    float weights = sawWeight + triangleWeight + squareWeight + square1Weight + square2Weight;
    
    float normal = 1;
    
    if (weights > 1)
    {
        normal = 1 / weights;
    }
    
    for (int i = 0; i < totalSize; i++)
    {
        float saw = (angle / period) - floor(angle / period);
        
        float triangle = 2 * abs((angle / period) - floor(angle / period + 0.5));
        
        float square = sgn(sin(angle));
        
        float square1 = sgn(sin(angle * 0.5));
        
        float square2 = sgn(sin(angle * 0.25));
        
        saw *= sawWeight;
        
        triangle *= triangleWeight;
        
        square *= squareWeight;
        
        square1 *= square1Weight;
        
        square2 *= square2Weight;
        
        float sum = saw + triangle + square + square1 + square2;
        
        samples[i] = sum * normal;
        
        angle += delta;
    }
    
    samples[totalSize] = samples[0];
    
    cout << "created wavetable with resolution [" + to_string(size) + "]" << endl;
}
