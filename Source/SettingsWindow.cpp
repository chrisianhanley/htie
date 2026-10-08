#include "SettingsWindow.h"
#include "PluginProcessor.h"

using namespace juce;

SettingsWindow::SettingsWindow(HTIntervalEngineAudioProcessor& p, AudioProcessorEditor& parent) : Window(parent), processor(p)
{
    titleLabel.setLookAndFeel(&lookAndFeel);
    titleLabel.setFont(CustomFont::REGULAR);
    titleLabel.setText("SETTINGS", dontSendNotification);
    
    resolutionLabel.setLookAndFeel(&lookAndFeel);
    resolutionLabel.setFont(CustomFont::REGULAR);
    resolutionLabel.setText("wavetable resolution: ", dontSendNotification);

    resolutionSelection.setLookAndFeel(&lookAndFeel);
    resolutionSelection.addItem("128 samples", 1);
    resolutionSelection.addItem("256 samples", 2);
    resolutionSelection.addItem("512 samples", 3);
    resolutionSelection.addItem("1024 samples", 4);
    resolutionSelection.addItem("2048 samples", 5);
    resolutionSelection.addItem("4096 samples", 6);
    
    toggleNoteMapLabel.setLookAndFeel(&lookAndFeel);
    toggleNoteMapLabel.setFont(CustomFont::REGULAR);
    toggleNoteMapLabel.setText("toggle note map: ", dontSendNotification);
    
    toggleNoteMapButton.setLookAndFeel(&lookAndFeel);
    
    /*
    int id = log2(PluginParameters::wavetableResolutionValue >> 6);
    
    resolutionSelection.setSelectedId(id);
    */
    
    resolutionSelection.onChange = [this]
    {
        int res = 1 << (resolutionSelection.getSelectedId() + 6);
        
        processor.getParameters().wavetableResolutionValue = res;
        
        processor.getSelectedSoundProfile()->update();
    };
    
    auto state = processor.getParameters().getPluginState();
    resolutionAttachment.reset(new ComboBoxAttachment(*state, "wavetableResolution", resolutionSelection));
    toggleNoteMapAttachment.reset(new ButtonAttachment(*state, "toggleNoteMap", toggleNoteMapButton));
    
    parent.addAndMakeVisible(&titleLabel);
    parent.addAndMakeVisible(&resolutionLabel);
    parent.addAndMakeVisible(&resolutionSelection);
    parent.addAndMakeVisible(&toggleNoteMapLabel);
    parent.addAndMakeVisible(&toggleNoteMapButton);
}

SettingsWindow::~SettingsWindow()
{
    
}

void SettingsWindow::paint(juce::Graphics& g) {}

void SettingsWindow::resized(Rectangle<int> bounds)
{
    titleLabel.setVisible(true);
    resolutionLabel.setVisible(true);
    resolutionSelection.setVisible(true);
    toggleNoteMapLabel.setVisible(true);
    toggleNoteMapButton.setVisible(true);
    
    float ratio = parent.getConstrainer()->getFixedAspectRatio();
    float reduction = 0.025;
    
    bounds.reduce(bounds.getWidth() * reduction, bounds.getHeight() * reduction * ratio);
    
    Rectangle<int> buffer;
    
    int width;
    int height;
    
    width = GlyphArrangement::getStringWidth(titleLabel.getFont(), titleLabel.getText()) + 10;
    height = titleLabel.getFont().getHeight() + 15;
    titleLabel.setBounds(bounds.removeFromTop(height).removeFromLeft(width));

    width = GlyphArrangement::getStringWidth(resolutionLabel.getFont(), resolutionLabel.getText()) + 10;
    height = resolutionLabel.getFont().getHeight() + 15;
    buffer = bounds.removeFromTop(height);
    resolutionLabel.setBounds(buffer.removeFromLeft(width));
    resolutionSelection.setBounds(buffer.removeFromLeft(148));
    
    width = GlyphArrangement::getStringWidth(toggleNoteMapLabel.getFont(), toggleNoteMapLabel.getText()) + 10;
    height = toggleNoteMapLabel.getFont().getHeight() + 15;
    buffer = bounds.removeFromTop(height);
    toggleNoteMapLabel.setBounds(buffer.removeFromLeft(width));
    toggleNoteMapButton.setBounds(buffer.removeFromLeft(148));
}
