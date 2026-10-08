#pragma once

#include <JuceHeader.h>
#include "Mixer.h"

class PluginParameters
{
public:
    PluginParameters();
    
    std::atomic<float>* keyCenterParameter;
    std::atomic<float>* rootInputRangeParameter;
    std::atomic<float>* quantizeRootParameter;
    std::atomic<float>* pedalRootParameter;
    std::atomic<float>* wavetableResolutionParameter;
    std::atomic<float>* toggleNoteMapParameter;
    
    std::atomic<float>* superimposeParameter;
    std::atomic<float>* mixParameter;
    std::atomic<float>* numVoicesParameter;
    
    std::atomic<float> gainValue;
    std::atomic<unsigned int> wavetableResolutionValue;
    
    void createReferences(juce::AudioProcessorValueTreeState* state);
    juce::AudioProcessorValueTreeState* getPluginState();
    
    Mixer getMixer();
    
private:
    juce::AudioProcessorValueTreeState* pluginState;
    Mixer mixer;
};
