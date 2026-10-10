/*
  ==============================================================================

    SynthEngine.h
    Created: 10 Oct 2025 9:18:30pm
    Author:  Ian Hanley

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginParameters.h"
#include "PitchMapper.h"
#include "WavetableOscillator.h"
#include "Mixer.h"

class SynthEngine : public juce::AudioSource, juce::AudioProcessorValueTreeState::Listener
{
public:
    static const unsigned int NUM_VOICES;
    
    SynthEngine(PluginParameters& p, PitchMapper& m);
    
    ~SynthEngine();
    
    void redrawVoices();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    
    int getPedalNote();
    int getPedalChannel();
    void setPedalNote(int note, int channel);
    void setPedalNote(int note);
    
    void setCurrentBuffer(juce::MidiBuffer& buffer);
    
    float map(int noteNumber);
    float map(int noteNumber, float transposeCents);
    
    PluginParameters& getParameters();
    
    PitchMapper& getPitchMapper();
    
    juce::Synthesiser& getSynth();
    
    void createWavetable();
    
    void addListeners();
    
private:
    int pedalNote;
    
    int pedalChannel;
    
    PluginParameters& parameters;
    
    PitchMapper& pitchMapper;
    
    juce::Synthesiser synth;
    
    juce::MidiBuffer currentBuffer;
    
    juce::AudioSampleBuffer wavetable;
    
    void parameterChanged(const juce::String& parameterID, float newValue) override;
};
