#pragma once

#include "Window.h"
#include "PluginProcessor.h"
#include "SynthEngine.h"

using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

class SynthWindow : public Window
{
public:
    SynthWindow(PluginParameters& p, SynthEngine& engine, juce::AudioProcessorEditor& parent);
    
    ~SynthWindow();
    
    void paint(juce::Graphics& g) override;
    
    void resized(juce::Rectangle<int> bounds) override;

private:
    PluginParameters& parameters;
    
    SynthEngine& engine;
    
    const unsigned int numModules;
    
    std::vector<juce::Rectangle<int>> modules;
    
    CustomLookAndFeel lookAndFeel;
    
    juce::ImageComponent sineImage;
    juce::ImageComponent triangleImage;
    juce::ImageComponent sawImage;
    juce::ImageComponent squareImage;
    juce::ImageComponent square1Image;
    juce::ImageComponent square2Image;
    
    juce::Slider sineSlider;
    juce::Slider triangleSlider;
    juce::Slider sawSlider;
    juce::Slider squareSlider;
    juce::Slider square1Slider;
    juce::Slider square2Slider;
    
    juce::ComboBox oscillatorSelector;
    
    std::unique_ptr<SliderAttachment> sawAttachment;
    std::unique_ptr<SliderAttachment> triangleAttachment;
    std::unique_ptr<SliderAttachment> squareAttachment;
    std::unique_ptr<SliderAttachment> square1Attachment;
    std::unique_ptr<SliderAttachment> square2Attachment;
};
