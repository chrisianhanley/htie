/*
  ==============================================================================

    Mixer.h
    Created: 30 Oct 2025 12:13:45am
    Author:  Ian Hanley

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

/*
struct MixerListener
{
    MixerListener(int channel, std::function<void(double)> callback);
    MixerListener(std::function<void(double)> callback);
    
    int channel;
    std::function<void(float)> callback;
};

class Mixer : public juce::Timer
{
public:
    Mixer(unsigned int mixerId, unsigned int numChannels);
    ~Mixer();
    
    juce::Identifier getIdentifer(unsigned int channel) const;
    
    unsigned int getNumChannels() const;
    
    double getValue(unsigned int channel);
    void setValue(unsigned int channel, double value);
    void setValueAndNotify(unsigned int channel, double value, MixerListener* source);
    
    void addListener(MixerListener* listener);
    void removeListener(MixerListener* listener);
    void clearListeners();
    
    void timerCallback() override;
    
private:
    const unsigned int mixerId;
    const unsigned int numChannels;
    
    juce::AudioProcessorValueTreeState* pluginState;
    
    juce::ValueTree lastState; // use for updating gui on state change
    
    juce::OwnedArray<MixerListener> listeners;
    
    void notifyListeners(unsigned int channel, double value, MixerListener* source);
};
*/

class Mixer
{
public:
    static void initializeValues(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    static void createReferences(juce::AudioProcessorValueTreeState* state);
    
    static std::atomic<float>* x1;
    static std::atomic<float>* x2;
    static std::atomic<float>* x3;
    static std::atomic<float>* x4;
    static std::atomic<float>* x5;
    static std::atomic<float>* x6;
};
