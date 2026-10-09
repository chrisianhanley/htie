#include "CustomLookAndFeel.h"
#include "CustomFont.h"

using namespace juce;

CustomLookAndFeel::CustomLookAndFeel()
{
    const Colour black = Colours::black;
    const Colour transparentBlack = Colours::black.withAlpha((uint8) 115);
    const Colour transparentRed = Colour(251, 138, 101).withAlpha((uint8) 30);
    
    setColour(Slider::thumbColourId, black);
    setColour(Slider::backgroundColourId, transparentBlack);

    setColour(Label::textColourId, black);
    
    setColour(ComboBox::textColourId, black);
    setColour(ComboBox::arrowColourId, black);
    setColour(ComboBox::backgroundColourId, transparentRed);
    setColour(ComboBox::outlineColourId, Colours::transparentBlack);
    
    setColour(ToggleButton::tickColourId, black);
    setColour(ToggleButton::tickDisabledColourId, black.withAlpha((uint8) 15));
    
    setColour(TextButton::buttonColourId, Colours::transparentBlack);
    setColour(TextButton::buttonOnColourId, Colours::transparentBlack);
    setColour(TextButton::textColourOffId, transparentBlack);
    setColour(TextButton::textColourOnId, transparentBlack);
    
    /*
    setColour(TextEditor::textColourId, textColour1);
    setColour(TextEditor::highlightColourId, Colours::transparentBlack);
    setColour(TextEditor::backgroundColourId, backgroundColour2);
    setColour(TextEditor::outlineColourId, Colours::transparentBlack);
    setColour(TextEditor::focusedOutlineColourId, Colours::transparentBlack);
     */
}

Font CustomLookAndFeel::getComboBoxFont(juce::ComboBox& box)
{
    return CustomFont::getRegularFont(CustomFont::DEFAULT_SIZE + 1.5);
}

Font CustomLookAndFeel::getTextButtonFont(juce::TextButton& button, int buttonHeight)
{
    return CustomFont::getRegularFont(CustomFont::DEFAULT_SIZE);
}

/*
void CustomLookAndFeel::drawTickBox(juce::Graphics& g, Component& component, float x, float y, float w, float h, bool ticked, bool isEnabled, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    g.drawRoundedRectangle(x, y, w, h, 5, 1);
    
    if (ticked)
    {
        float reduce = 0.3;
        float nw = w * reduce;
        float nh = h * reduce;
        x += (w - nw) * 0.5;
        y += (h - nh) * 0.5;
        
        //g.fillRoundedRectangle(x, y, w, h, 5);
        
        auto bounds = Rectangle<float>(x, y, nw, nh);
        juce::Path tickPath;
        tickPath.startNewSubPath(bounds.getX() + 5, bounds.getCentreY());
        tickPath.lineTo(bounds.getX() + 10, bounds.getCentreY() + 5);
        tickPath.lineTo(bounds.getX() + 20, bounds.getCentreY() - 5);
        g.strokePath(tickPath, juce::PathStrokeType(2.0f));
    }
}
*/
