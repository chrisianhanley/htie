#pragma once

#include "Window.h"

class ElectricPianoWindow : public Window
{
public:
    ElectricPianoWindow(juce::AudioProcessorEditor& parent);
    
    void paint(juce::Graphics& g) override;
    void resized(juce::Rectangle<int> bounds) override;
};
