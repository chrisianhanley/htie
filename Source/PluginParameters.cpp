#include "PluginParameters.h"

using namespace juce;
using namespace std;

PluginParameters::PluginParameters() : gainValue(0.0631), wavetableResolutionValue(2048) {}

void PluginParameters::createReferences(juce::AudioProcessorValueTreeState* state)
{
    jassert(state);
    
    keyCenterParameter = state->getRawParameterValue("keyCenter");
    rootInputRangeParameter = state->getRawParameterValue("rootInputRange");
    quantizeRootParameter = state->getRawParameterValue("quantizeRoot");
    pedalRootParameter = state->getRawParameterValue("pedalRoot");
    wavetableResolutionParameter = state->getRawParameterValue("wavetableResolution");
    toggleNoteMapParameter = state->getRawParameterValue("toggleNoteMap");
    superimposeParameter = state->getRawParameterValue("superimpose");
    mixParameter = state->getRawParameterValue("mix");
    numVoicesParameter = state->getRawParameterValue("numVoices");
    
    mixer.createReferences(state);
    
    jassert(keyCenterParameter && rootInputRangeParameter && quantizeRootParameter && pedalRootParameter && wavetableResolutionParameter && toggleNoteMapParameter && superimposeParameter && mixParameter && numVoicesParameter);
    
    pluginState = state;
}

AudioProcessorValueTreeState* PluginParameters::getPluginState()
{
    return pluginState;
}

Mixer& PluginParameters::getMixer()
{
    return mixer;
}
