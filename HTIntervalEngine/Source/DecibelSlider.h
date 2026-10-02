#pragma once

#include <JuceHeader.h>

class DecibelSlider : public juce::Slider
{
public:
    DecibelSlider();

    double getValueFromText (const juce::String& text) override;
    juce::String getTextFromValue (double value) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DecibelSlider)
};
