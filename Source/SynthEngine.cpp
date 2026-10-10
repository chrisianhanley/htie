/*
  ==============================================================================

    SynthEngine.cpp
    Created: 10 Oct 2025 9:18:30pm
    Author:  Ian Hanley

  ==============================================================================
*/

#include "SynthEngine.h"
#include "PitchMapper.h"
#include "PluginParameters.h"
#include "WavetableVoice.h"

using namespace juce;
using namespace std;

const unsigned int SynthEngine::NUM_VOICES = 10;

SynthEngine::SynthEngine(PluginParameters& p, PitchMapper& m) : parameters(p), pitchMapper(m)
{
    pedalNote = -1;
    
    pedalChannel = 0;
    
    for (int i = 0; i < NUM_VOICES; i++)
    {
        synth.addVoice(new WavetableVoice(*this, wavetable, 4));
    }
    
    synth.addSound(new WavetableSound());
    
    wavetable.setSize(1, 4096 * 4 + 1);
}

void SynthEngine::addListeners()
{
    auto pluginState = parameters.getPluginState();
    
    pluginState->addParameterListener("x1", this);
    
    pluginState->addParameterListener("x2", this);
    
    pluginState->addParameterListener("x3", this);
    
    pluginState->addParameterListener("x4", this);
    
    pluginState->addParameterListener("x5", this);
    
    pluginState->addParameterListener("x6", this);
    
    pluginState->addParameterListener("wavetableResolution", this);
}

SynthEngine::~SynthEngine()
{
    auto pluginState = parameters.getPluginState();
    
    pluginState->removeParameterListener("x1", this);
    
    pluginState->removeParameterListener("x2", this);
    
    pluginState->removeParameterListener("x3", this);
    
    pluginState->removeParameterListener("x4", this);
    
    pluginState->removeParameterListener("x5", this);
    
    pluginState->removeParameterListener("x6", this);
    
    pluginState->removeParameterListener("wavetableResolution", this);
}

void SynthEngine::parameterChanged(const juce::String& parameterID, float newValue)
{
    createWavetable();
}

void SynthEngine::createWavetable()
{
    auto size = 1 << ((int) *parameters.wavetableResolutionParameter + 6);
    
    auto totalSize = size * 4; // size multiplied by num cycles
    
    auto period = MathConstants<double>::twoPi;
    
    auto delta = period / size;
    
    auto angle = 0.0;
    
    wavetable.setSize(1, totalSize + 1, false, false, true);
    
    auto samples = wavetable.getWritePointer(0);
    
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
    
    cout << "created wavetable with resolution [" + to_string(size) + "]" << endl;
}

void SynthEngine::redrawVoices()
{
    for (int i = 0; i < synth.getNumVoices(); i++)
    {
        auto voice = synth.getVoice(i);
        
        if (auto wt = dynamic_cast<WavetableVoice*>(voice))
        {
            if (wt->isVoiceActive())
            {
                int note = wt->getCurrentlyPlayingNote();
                
                wt->setFrequency(map(note));
            }
        }
    }
}

PluginParameters& SynthEngine::getParameters()
{
    return parameters;
}

PitchMapper& SynthEngine::getPitchMapper()
{
    return pitchMapper;
}

Synthesiser& SynthEngine::getSynth()
{
    return synth;
}

int SynthEngine::getPedalNote()
{
    return pedalNote;
}

int SynthEngine::getPedalChannel()
{
    return pedalChannel;
}

void SynthEngine::setPedalNote(int note, int channel)
{
    pedalNote = note;
    pedalChannel = channel;
}

void SynthEngine::setPedalNote(int note)
{
    pedalNote = note;
}

void SynthEngine::setCurrentBuffer(juce::MidiBuffer& buffer)
{
    currentBuffer = buffer;
}

void SynthEngine::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
}

void SynthEngine::releaseResources() {}

void SynthEngine::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    synth.renderNextBlock(*bufferToFill.buffer, currentBuffer, bufferToFill.startSample, bufferToFill.numSamples);
    
    currentBuffer.clear();
}

float SynthEngine::map(int noteNumber)
{
    auto pedal = pedalNote == noteNumber;
    
    if (pedal)
    {
        auto name = pitchMapper.getNoteNumberAsNote(noteNumber);
        
        auto current = pitchMapper.getNoteAsSemitones(name);
        
        if (*parameters.keyCenterParameter - 1 > current)
        {
            return pitchMapper.map(noteNumber, 1200, true);
        }
    }

    return pitchMapper.map(noteNumber, !pedal);
}

float SynthEngine::map(int noteNumber, float transposeCents)
{
    auto pedal = pedalNote == noteNumber;
    
    if (pedal)
    {
        auto name = pitchMapper.getNoteNumberAsNote(noteNumber);
        
        auto current = pitchMapper.getNoteAsSemitones(name);
        
        if (*parameters.keyCenterParameter - 1 > current)
        {
            return pitchMapper.map(noteNumber, transposeCents, false);
        }
    }

    return pitchMapper.map(noteNumber, !pedal);
}
