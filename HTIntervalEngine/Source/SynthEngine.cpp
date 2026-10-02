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

SynthEngine::SynthEngine()
{
    pedalNote = -1;
    
    PitchMapper::onMapChangeSync = [this] {
        for (int i = 0; i < synth.getNumVoices(); i++)
        {
            auto voice = synth.getVoice(i);
            
            if (voice->isVoiceActive())
            {
                int note = voice->getCurrentlyPlayingNote();
                
                voice->setFrequency(map(note));
            }
        }
    };
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

void SynthEngine::setCurrentBuffer(juce::MidiBuffer buffer)
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
        auto name = PitchMapper::getNoteNumberAsNote(noteNumber);
        auto current = PitchMapper::getNoteAsSemitones(name);
        
        if (*PluginParameters::keyCenterParameter - 1 > current)
        {
            return PitchMapper::map(noteNumber, 1200, true);
        }
    }

    return PitchMapper::map(noteNumber, !pedal);
}

float SynthEngine::map(int noteNumber, float transposeCents)
{
    auto pedal = pedalNote == noteNumber;
    
    if (pedal)
    {
        auto name = PitchMapper::getNoteNumberAsNote(noteNumber);
        auto current = PitchMapper::getNoteAsSemitones(name);
        
        if (*PluginParameters::keyCenterParameter - 1 > current)
        {
            return PitchMapper::map(noteNumber, transposeCents, true);
        }
    }

    return PitchMapper::map(noteNumber, !pedal);
}
