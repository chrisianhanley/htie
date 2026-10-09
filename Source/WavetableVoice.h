#pragma once

#include "JuceHeader.h"
#include "SoundProfile.h"

class WavetableVoice : public juce::SynthesiserVoice
{
public:
    WavetableVoice(SoundProfile& profile, shared_ptr<juce::AudioSampleBuffer>& table, unsigned int cycles);
    
    ~WavetableVoice();
    
    bool canPlaySound(juce::SynthesiserSound* sound) override = 0;
    
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override = 0;
    
    void stopNote(float velocity, bool allowTailOff) override = 0;
    
    void pitchWheelMoved(int newPitchWheelValue) override = 0;
    
    void controllerMoved(int controllerNumber, int newControllerValue) override = 0;
    
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override = 0;

    void setFrequency(double frequency) override = 0;
    
    std::vector<WavetableOscillator>& getOscillators();
    
    virtual void setOscillators(unsigned int numVoices);
    
protected:
    SoundProfile& soundProfile;

    std::vector<WavetableOscillator> oscillators;
    
    shared_ptr<juce::AudioSampleBuffer>& table;
    
    unsigned int cycles;
};
