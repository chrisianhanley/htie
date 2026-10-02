#include "PluginParameters.h"

using namespace juce;
using namespace std;

AudioProcessorValueTreeState* PluginParameters::pluginState;

atomic<float>* PluginParameters::keyCenterParameter = nullptr;
atomic<float>* PluginParameters::rootInputRangeParameter = nullptr;
atomic<float>* PluginParameters::quantizeRootParameter = nullptr;
atomic<float>* PluginParameters::pedalRootParameter = nullptr;
atomic<float>* PluginParameters::wavetableResolutionParameter = nullptr;
atomic<float>* PluginParameters::toggleNoteMapParameter = nullptr;
atomic<float>* PluginParameters::superimposeParameter = nullptr;
atomic<float>* PluginParameters::mixParameter = nullptr;

atomic<float> PluginParameters::gainValue = 0.125893;
atomic<unsigned int> PluginParameters::wavetableResolutionValue = 2048;

Mixer PluginParameters::mixer = Mixer();

AudioProcessorValueTreeState::ParameterLayout PluginParameters::createLayout()
{
    AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(make_unique<juce::AudioParameterInt>(ParameterID { "keyCenter", 2 }, "key", 1, 12, 1));
    layout.add(make_unique<juce::AudioParameterInt>(ParameterID { "rootInputRange", 2 }, "input range", 1, 10, 5));
    layout.add(make_unique<juce::AudioParameterBool>(ParameterID { "quantizeRoot", 2 }, "quantize", false));
    layout.add(make_unique<juce::AudioParameterBool>(ParameterID { "pedalRoot", 2 }, "pedal", false));
    layout.add(make_unique<juce::AudioParameterInt>(ParameterID { "wavetableResolution", 2 }, "wavetable res", 1, 6, 5));
    layout.add(make_unique<juce::AudioParameterBool>(ParameterID { "toggleNoteMap", 1 }, "toggle nm", true));
    layout.add(make_unique<juce::AudioParameterInt>(ParameterID { "superimpose", 1 }, "superimpose", 1, 12, 12));
    layout.add(make_unique<juce::AudioParameterFloat>(ParameterID { "mix", 1 }, "mix", 0, 1, 0.5f));
    
    Mixer::initializeValues(layout);
    
    return layout;
}

void PluginParameters::createReferences(juce::AudioProcessorValueTreeState* state)
{
    keyCenterParameter = state->getRawParameterValue("keyCenter");
    rootInputRangeParameter = state->getRawParameterValue("rootInputRange");
    quantizeRootParameter = state->getRawParameterValue("quantizeRoot");
    pedalRootParameter = state->getRawParameterValue("pedalRoot");
    wavetableResolutionParameter = state->getRawParameterValue("wavetableResolution");
    toggleNoteMapParameter = state->getRawParameterValue("toggleNoteMap");
    superimposeParameter = state->getRawParameterValue("superimpose");
    mixParameter = state->getRawParameterValue("mix");
    
    Mixer::createReferences(state);
    
    jassert(keyCenterParameter && rootInputRangeParameter && quantizeRootParameter && pedalRootParameter && wavetableResolutionParameter);
    
    pluginState = state;
}

AudioProcessorValueTreeState* PluginParameters::getPluginState()
{
    jassert(pluginState);
    
    return pluginState;
}
