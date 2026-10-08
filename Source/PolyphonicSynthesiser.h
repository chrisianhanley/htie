#pragma once

#include "SoundProfile.h"
#include "WavetableVoice.h"
#include "Mixer.h"

class PolyphonicSynthesiserVoice : public WavetableVoice
{
public:
    PolyphonicSynthesiserVoice(SoundProfile& profile, juce::AudioSampleBuffer& table);

    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    void setFrequency(double frequency) override;
    void setOscillators(unsigned int n) override;
    
    void reset();

private:
    juce::SmoothedValue<float> frequency;
    juce::SmoothedValue<float> gain;
    
    float initialFrequency;
    float level;
    float tailOff;
    std::vector<float> ratios;
};

class PolyphonicSynthesiserSound : public juce::SynthesiserSound
{
    bool appliesToNote(int midiNoteNumber) override;
    bool appliesToChannel(int midiChannel) override;
};

class PolyphonicSynthesiser : public SoundProfile
{
public:
    PolyphonicSynthesiser(SynthEngine& engine, PitchMapper& pm, PluginParameters& p, unsigned int i);
    ~PolyphonicSynthesiser();
    
    juce::String getDisplayName() override;
    
    Window* createWindow(juce::AudioProcessorEditor& parent) override;
    
    void enable() override;
    void update() override;
    void disable() override;
    
    void createTable();
    
private:
    unsigned int numVoices;
    
    juce::AudioSampleBuffer wavetable;
};
