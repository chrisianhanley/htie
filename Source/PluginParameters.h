#pragma once

#include <JuceHeader.h>
#include "Mixer.h"

class PluginParameters
{
public:
    static std::atomic<float>* keyCenterParameter;
    static std::atomic<float>* rootInputRangeParameter;
    static std::atomic<float>* quantizeRootParameter;
    static std::atomic<float>* pedalRootParameter;
    static std::atomic<float>* wavetableResolutionParameter;
    static std::atomic<float>* toggleNoteMapParameter;
    
    static std::atomic<float>* superimposeParameter;
    static std::atomic<float>* mixParameter;
    static std::atomic<float>* numVoicesParameter;
    
    static std::atomic<float> gainValue;
    static std::atomic<unsigned int> wavetableResolutionValue;
    
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    static void createReferences(juce::AudioProcessorValueTreeState* state);
    static juce::AudioProcessorValueTreeState* getPluginState();
    
    static Mixer getMixer();
    
private:
    PluginParameters();
    
    static juce::AudioProcessorValueTreeState* pluginState;
    static Mixer mixer;
};
