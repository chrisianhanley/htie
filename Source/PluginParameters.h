#pragma once

#include <JuceHeader.h>
#include "Mixer.h"

class PluginParameters
{
public:
    PluginParameters();
    
    std::atomic<float>* keyCenterParameter = nullptr;
    std::atomic<float>* rootInputRangeParameter = nullptr;
    std::atomic<float>* quantizeRootParameter = nullptr;
    std::atomic<float>* pedalRootParameter = nullptr;
    std::atomic<float>* wavetableResolutionParameter = nullptr;
    std::atomic<float>* toggleNoteMapParameter = nullptr;
    
    std::atomic<float>* superimposeParameter = nullptr;
    std::atomic<float>* mixParameter = nullptr;
    std::atomic<float>* numVoicesParameter = nullptr;
    
    std::atomic<float> gainValue;
    std::atomic<unsigned int> wavetableResolutionValue;
    
    void createReferences(juce::AudioProcessorValueTreeState* state);
    juce::AudioProcessorValueTreeState* getPluginState();
    
    Mixer& getMixer();
    
private:
    juce::AudioProcessorValueTreeState* pluginState;
    Mixer mixer;
};
