#pragma once

#include <JuceHeader.h>
#include "PluginParameters.h"
#include "CustomLookAndFeel.h"
#include "CustomFont.h"

class Window
{
public:
    Window(juce::AudioProcessorEditor& e);
    virtual ~Window();
    
    virtual void paint(juce::Graphics& g);
    virtual void resized(juce::Rectangle<int> b);
    
protected:
    juce::AudioProcessorEditor& parent;
};
