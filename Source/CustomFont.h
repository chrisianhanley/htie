#pragma once

#include <JuceHeader.h>

struct CustomFont
{
    static const juce::Font& getRegularFont(float size);
    static const juce::Font& getItalicFont(float size);
    static const juce::Font& getBoldFont(float size);
    
    static const juce::String TYPEFACE_NAME;
    static const float DEFAULT_SIZE;
    
    static const juce::Font& REGULAR;
    static const juce::Font& ITALIC;
    //static const juce::Font& BOLD;
    
private:
    CustomFont();
};
