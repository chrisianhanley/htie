#include "Mixer.h"
#include "PluginParameters.h"

using namespace juce;
using namespace std;

/*

MixerListener::MixerListener(int channel, std::function<void(double)> c) : channel(channel), callback(c) {}

MixerListener::MixerListener(std::function<void(double)> c) : channel(-1), callback(c) {}

Mixer::Mixer(unsigned int i, unsigned int channels) : mixerId(i), numChannels(channels), pluginState(PluginParameters::getPluginState())
{
    jassert(pluginState);
    
    jassert(i > 0);
    
    jassert(channels > 0);
    
    startTimer(100);
}

Mixer::~Mixer()
{
    listeners.clear();
}

void Mixer::timerCallback()
{
    auto currentState = pluginState->state;
    
    if (currentState != lastState)
    {
        for (auto listener : listeners)
        {
            auto channel = listener->channel;
            
            if (channel == -1)
            {
                listener->callback(-1);
                
                continue;
            }
            
            for (int i = 0; i < numChannels; i++)
            {
                float value = getValue(i);
                
                //cout << value << endl;
                
                if (channel == i)
                {
                    listener->callback(value);
                }
            }
        }
    }
    
    lastState = currentState;
}

Identifier Mixer::getIdentifer(unsigned int channel) const
{
    return Identifier("mixer" + to_string(mixerId) + to_string(channel));
}

unsigned int Mixer::getNumChannels() const
{
    return numChannels;
}

double Mixer::getValue(unsigned int i)
{
    jassert(i < numChannels);
    
    auto id = getIdentifer(i);
    
    return pluginState->state.getPropertyAsValue(id, nullptr, true).getValue();
}

void Mixer::setValue(unsigned int i, double value)
{
    jassert(i < numChannels);
    
    MessageManager::callAsync([this, i, value]
    {
        auto id = getIdentifer(i);
        
        pluginState->state.getPropertyAsValue(id, nullptr, true).setValue(value);
    });
}

void Mixer::setValueAndNotify(unsigned int i, double value, MixerListener* source)
{
    jassert(i < numChannels);
    
    MessageManager::callAsync([this, i, value, source]
    {
        auto id = getIdentifer(i);
        
        pluginState->state.getPropertyAsValue(id, nullptr, true).setValue(value);
        
        notifyListeners(i, value, source);
    });
}

void Mixer::addListener(MixerListener* listener)
{
    listeners.add(listener);
}

void Mixer::removeListener(MixerListener* listener)
{
    listeners.removeObject(listener);
}

void Mixer::clearListeners()
{
    listeners.clear();
}

void Mixer::notifyListeners(unsigned int i, double value, MixerListener* source)
{
    cout << getIdentifer(i).toString() + " = " + to_string(value) << endl;
    
    for (auto listener : listeners)
    {
        if (listener == source)
        {
            continue;
        }
        
        auto channel = listener->channel;
        
        if (channel == -1)
        {
            listener->callback(-1);
            
            continue;
        }
        
        if (channel == i)
        {
            listener->callback(value);
        }
    }
}
 
 */

std::atomic<float>* Mixer::x1 = nullptr;
std::atomic<float>* Mixer::x2 = nullptr;
std::atomic<float>* Mixer::x3 = nullptr;
std::atomic<float>* Mixer::x4 = nullptr;
std::atomic<float>* Mixer::x5 = nullptr;
std::atomic<float>* Mixer::x6 = nullptr;

void Mixer::initializeValues(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    layout.add(make_unique<juce::AudioParameterFloat>(ParameterID { "x1", 1 }, "x1", 0, 1, .9));
    layout.add(make_unique<juce::AudioParameterFloat>(ParameterID { "x2", 1 }, "x2", 0, 1, .5));
    layout.add(make_unique<juce::AudioParameterFloat>(ParameterID { "x3", 1 }, "x3", 0, 1, .3));
    layout.add(make_unique<juce::AudioParameterFloat>(ParameterID { "x4", 1 }, "x4", 0, 1, .1));
    layout.add(make_unique<juce::AudioParameterFloat>(ParameterID { "x5", 1 }, "x5", 0, 1, 0));
    layout.add(make_unique<juce::AudioParameterFloat>(ParameterID { "x6", 1 }, "x6", 0, 1, 0));
}

void Mixer::createReferences(juce::AudioProcessorValueTreeState *state)
{
    x1 = state->getRawParameterValue("x1");
    x2 = state->getRawParameterValue("x2");
    x3 = state->getRawParameterValue("x3");
    x4 = state->getRawParameterValue("x4");
    x5 = state->getRawParameterValue("x5");
    x6 = state->getRawParameterValue("x6");
}
