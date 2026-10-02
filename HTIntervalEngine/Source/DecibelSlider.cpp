#include "DecibelSlider.h"

using namespace juce;

DecibelSlider::DecibelSlider() {}

double DecibelSlider::getValueFromText(const juce::String& text)
{
    auto minusInfinitydB = -100;
    
    auto decibelText = text.upToFirstOccurrenceOf("dB", false, false).trim();

    return decibelText.equalsIgnoreCase("-INF") ? minusInfinitydB
                                                 : decibelText.getDoubleValue();
}

String DecibelSlider::getTextFromValue(double value)
{
    return juce::Decibels::toString(value);
}
