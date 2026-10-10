#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"
#include "Window.h"
#include "SettingsWindow.h"
#include "DecibelSlider.h"

//==============================================================================
/**
*/

using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

class HTIntervalEngineAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    HTIntervalEngineAudioProcessorEditor(Processor&);
    ~HTIntervalEngineAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    Processor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HTIntervalEngineAudioProcessorEditor)
    
    CustomLookAndFeel lookAndFeel;
    
    std::unique_ptr<Window> currentWindow;
    
    juce::Rectangle<int> boundsToFill;
    
    DecibelSlider volumeSlider;
    juce::MidiKeyboardComponent keyboardComponent;
    
    juce::Label keyCenterLabel;
    juce::Label rootInputRangeLabel;
    juce::Label quantizeRootLabel;
    juce::Label pedalRootLabel;
    juce::Label currentRootIntervalLabel;

    juce::ComboBox keyCenterSelection;
    juce::ComboBox rootInputRangeSelection;
    juce::ToggleButton quantizeRootToggle;
    juce::ToggleButton pedalRootToggle;
    
    std::unique_ptr<ComboBoxAttachment> keyCenterAttachment;
    std::unique_ptr<ComboBoxAttachment> rootInputRangeAttachment;
    std::unique_ptr<ButtonAttachment> quantizeRootAttachment;
    std::unique_ptr<ButtonAttachment> pedalRootAttachment;
    std::unique_ptr<SliderAttachment> gainAttachment;
    
    std::unique_ptr<juce::FileChooser> intervalMapChooser;
    juce::TextButton intervalMapButton;
    juce::TextButton reloadButton;
    juce::Label fileLoadLabel;

    juce::TextButton settingsButton;
    
    juce::Label superimposeLabel;
    juce::ComboBox superimposeSelection;
    juce::Label mixLabel;
    juce::Slider mixSlider;
    juce::Label numVoicesLabel;
    juce::ComboBox numVoicesSelection;
    
    std::unique_ptr<ComboBoxAttachment> superimposeAttachment;
    std::unique_ptr<SliderAttachment> mixAttachment;
    std::unique_ptr<ComboBoxAttachment> numVoicesAttachment;
    
    void setCurrentWindow(Window* window);
    void keyCenterSelectionChange();
    void rootInputRangeSelectionChange();
    void updateCurrentRootIntervalText();
    void calculateDynamicComponentBounds(juce::Rectangle<int>& bounds);
    
    void timerCallback() override;
};
