/*
  ==============================================================================

    SynthEngine.h
    Created: 10 Oct 2025 9:18:30pm
    Author:  Ian Hanley

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "WavetableOscillator.h"
#include "Mixer.h"

class SynthEngine : public juce::AudioSource
{
public:
    SynthEngine();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    
    int getPedalNote();
    int getPedalChannel();
    void setPedalNote(int note, int channel);
    void setPedalNote(int note);
    
    void setCurrentBuffer(juce::MidiBuffer buffer);
    
    juce::Synthesiser& getSynth();
    
    float map(int noteNumber);
    float map(int noteNumber, float transposeCents);
    
private:
    int pedalNote;
    int pedalChannel;
    
    juce::Synthesiser synth;
    juce::MidiBuffer currentBuffer;
};
