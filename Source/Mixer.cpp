#include "Mixer.h"
#include "PluginParameters.h"

using namespace juce;
using namespace std;

std::atomic<float>* Mixer::x1 = nullptr;
std::atomic<float>* Mixer::x2 = nullptr;
std::atomic<float>* Mixer::x3 = nullptr;
std::atomic<float>* Mixer::x4 = nullptr;
std::atomic<float>* Mixer::x5 = nullptr;
std::atomic<float>* Mixer::x6 = nullptr;

void Mixer::initializeValues(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    layout.add(make_unique<juce::AudioParameterFloat>(ParameterID { "x1", 2 }, "x1", 0, 1, .35f));
    layout.add(make_unique<juce::AudioParameterFloat>(ParameterID { "x2", 2 }, "x2", 0, 1, .45f));
    layout.add(make_unique<juce::AudioParameterFloat>(ParameterID { "x3", 2 }, "x3", 0, 1, .3f));
    layout.add(make_unique<juce::AudioParameterFloat>(ParameterID { "x4", 2 }, "x4", 0, 1, .4f));
    layout.add(make_unique<juce::AudioParameterFloat>(ParameterID { "x5", 2 }, "x5", 0, 1, .5f));
    layout.add(make_unique<juce::AudioParameterFloat>(ParameterID { "x6", 2 }, "x6", 0, 1, 0));
}

void Mixer::createReferences(juce::AudioProcessorValueTreeState* state)
{
    jassert(state);
    
    x1 = state->getRawParameterValue("x1");
    x2 = state->getRawParameterValue("x2");
    x3 = state->getRawParameterValue("x3");
    x4 = state->getRawParameterValue("x4");
    x5 = state->getRawParameterValue("x5");
    x6 = state->getRawParameterValue("x6");
}
