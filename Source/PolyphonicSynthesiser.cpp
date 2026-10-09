#include "PolyphonicSynthesiser.h"
#include "PolyphonicSynthesiserWindow.h"
#include "PluginParameters.h"
#include "PitchMapper.h"

using namespace juce;
using namespace std;

PolyphonicSynthesiserVoice::PolyphonicSynthesiserVoice(SoundProfile& profile, AudioSampleBuffer& table) : WavetableVoice(profile, table, 4)
{
    if (*soundProfile.parameters.numVoicesParameter < 1)
    {
        jassertfalse;
    }
    
    setOscillators(12);
}

void PolyphonicSynthesiserVoice::setCurrentPlaybackSampleRate(double rate)
{
    SynthesiserVoice::setCurrentPlaybackSampleRate(rate);
    
    if (rate < 1)
    {
        return;
    }
    
    frequency.reset(rate, 0.05);
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
    auto& engine = soundProfile.synthEngine;
    
    initialFrequency = engine.map(midiNoteNumber);
    
    frequency.setCurrentAndTargetValue(initialFrequency);
    
    gain.setTargetValue(soundProfile.parameters.gainValue);
    
    level = midiNoteNumber == engine.getPedalNote() ? 0.8 : velocity;
    
    tailOff = 0;
    
    oscillators[0].setFrequency(frequency.getNextValue(), getSampleRate());
    
    auto superimpose = soundProfile.parameters.superimposeParameter;

    if (*superimpose != 12 && !soundProfile.pitchMapper.isSubstituted(midiNoteNumber))
    {
        for (int i = 1; i < *soundProfile.parameters.numVoicesParameter + 1; i++)
        {
            auto super = (int) *superimpose * i;
            
            auto freq = soundProfile.pitchMapper.mapRelative(midiNoteNumber, super, false);
            
            auto ratio = freq / initialFrequency;
            
            ratios[i - 1] = ratio;
            
            oscillators[i].setFrequency(frequency.getCurrentValue() * ratio, getSampleRate());
        }
    }
    else
    {
        for (int i = 1; i < *soundProfile.parameters.numVoicesParameter + 1; i++)
        {
            oscillators[i].reset();
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
    /*
    int range = 12;
    float valuePerSemitone = 8192 / range;
    float transpose = (newPitchWheelValue / valuePerSemitone) - range;

    TODO -- add pitch wheel functionality
    */
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
        gain.setTargetValue(soundProfile.parameters.gainValue);
        
        while (numSamples > 0)
        {
            auto next = frequency.getNextValue();
            
            oscillators[0].setFrequency(next, getSampleRate());
            
            float nextSample = oscillators[0].getNextSample();
            
            if (*soundProfile.parameters.superimposeParameter != 12)
            {
                for (int i = 1; i < *soundProfile.parameters.numVoicesParameter + 1; i++)
                {
                    if (oscillators[i].getDelta() != 0)
                    {
                        oscillators[i].setFrequency(next * ratios[i - 1], getSampleRate());
                        
                        nextSample += oscillators[i].getNextSample() * *soundProfile.parameters.mixParameter;
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
            
            nextSample *= level * gain.getNextValue();
            
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
    
    for (auto& o : oscillators) {
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

PolyphonicSynthesiser::PolyphonicSynthesiser(SynthEngine& engine, PitchMapper& pm, PluginParameters& p, unsigned int i) : SoundProfile(engine, pm, p, i), numVoices(8)
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
    return new PolyphonicSynthesiserWindow(parameters, parent, *this);
}

void PolyphonicSynthesiser::enable()
{
    auto& synth = synthEngine.getSynth();
    
    for (int i = 0; i < numVoices; i++)
    {
        synth.addVoice(new PolyphonicSynthesiserVoice(*this, wavetable));
    }
    
    synth.addSound(new PolyphonicSynthesiserSound());
    
    createTable();
}

void PolyphonicSynthesiser::update()
{
    
}

void PolyphonicSynthesiser::disable()
{
    synthEngine.getSynth().clearVoices();
    synthEngine.getSynth().clearSounds();
}

template <typename T>
int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

void PolyphonicSynthesiser::createTable()
{
    auto size = parameters.wavetableResolutionValue.load();
    
    auto totalSize = size * 4; // size multiplied by num cycles
    
    auto period = MathConstants<double>::twoPi;
    auto delta = period / size;
    auto angle = 0.0;
    
    AudioSampleBuffer buffer = AudioSampleBuffer(1, totalSize + 1);
    
    buffer.setSize(1, totalSize + 1);
    
    auto samples = buffer.getWritePointer(0);
    
    float sawWeight = *parameters.getMixer().x1;
    
    float triangleWeight = *parameters.getMixer().x2;
    
    float squareWeight = *parameters.getMixer().x3;
    
    float square1Weight = *parameters.getMixer().x4;
    
    float square2Weight = *parameters.getMixer().x5;
    
    float weights = sawWeight + triangleWeight + squareWeight + square1Weight + square2Weight;
    
    float normal = 1;
    
    if (weights > 1)
    {
        normal = 1 / weights;
    }
    
    for (int i = 0; i < totalSize; i++)
    {
        float saw = 2 * ((angle / period) - floor(0.5 + angle / period));
        
        float triangle = 2 * abs(2 * ((angle / period) - floor(0.5 + angle / period))) - 1;
        
        float square = sin(angle) < 0 ? 1 : -1;
        
        float square1 = sin(angle * 0.5) < 0 ? 1 : -1;
        
        float square2 = sin(angle * 0.25) < 0 ? 1 : -1;
        
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
    
    wavetable = buffer;
    
    //cout << "created wavetable with resolution [" + to_string(size) + "]" << endl;
}
