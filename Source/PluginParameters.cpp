#include "PluginParameters.h"

using namespace juce;
using namespace std;

PluginParameters::PluginParameters() {}

void PluginParameters::createReferences(juce::AudioProcessorValueTreeState* state)
{
    jassert(state);
    
    keyCenterParameter = state->getRawParameterValue("keyCenter");
    rootInputRangeParameter = state->getRawParameterValue("rootInputRange");
    quantizeRootParameter = state->getRawParameterValue("quantizeRoot");
    droneParameter = state->getRawParameterValue("drone");
    wavetableResolutionParameter = state->getRawParameterValue("wavetableResolution");
    noteMapPersistsParameter = state->getRawParameterValue("noteMapPersists");
    superimposeParameter = state->getRawParameterValue("superimpose");
    mixParameter = state->getRawParameterValue("mix");
    numVoicesParameter = state->getRawParameterValue("numVoices");
    gainParameter = state->getRawParameterValue("gain");
    
    mixer.createReferences(state);
    
    jassert(keyCenterParameter && rootInputRangeParameter && quantizeRootParameter && droneParameter && wavetableResolutionParameter && noteMapPersistsParameter && superimposeParameter && mixParameter && numVoicesParameter && gainParameter);
    
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
