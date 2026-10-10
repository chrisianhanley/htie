#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Window.h"

using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

class SettingsWindow : public Window
{
public:
    SettingsWindow(HTIntervalEngineAudioProcessor& processor, juce::AudioProcessorEditor& parent);
    ~SettingsWindow();
    
    void paint(juce::Graphics& g) override;
    void resized(juce::Rectangle<int> bounds) override;
    
private:
    HTIntervalEngineAudioProcessor& processor;
    
    CustomLookAndFeel lookAndFeel;
    
    juce::Label titleLabel;
    
    juce::Label resolutionLabel;
    
    juce::ComboBox resolutionSelection;
    
    juce::Label noteMapPersistsLabel;
    
    juce::ToggleButton noteMapPersistsButton;
    
    std::unique_ptr<ComboBoxAttachment> resolutionAttachment;
    
    std::unique_ptr<ButtonAttachment> toggleNoteMapAttachment;
};
