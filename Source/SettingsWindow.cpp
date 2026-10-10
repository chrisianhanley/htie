#include "SettingsWindow.h"
#include "PluginProcessor.h"

using namespace juce;

SettingsWindow::SettingsWindow(HTIntervalEngineAudioProcessor& p, AudioProcessorEditor& parent) : Window(parent), processor(p)
{
    auto state = processor.getPluginParameters().getPluginState();
    
    auto rfont = CustomFont::getRegularFont(CustomFont::DEFAULT_SIZE);
    
    titleLabel.setLookAndFeel(&lookAndFeel);
    titleLabel.setFont(rfont);
    titleLabel.setText("SETTINGS", dontSendNotification);
    
    resolutionLabel.setLookAndFeel(&lookAndFeel);
    resolutionLabel.setFont(rfont);
    resolutionLabel.setText("wavetable resolution: ", dontSendNotification);

    resolutionSelection.setLookAndFeel(&lookAndFeel);
    resolutionSelection.addItem("128 samples", 1);
    resolutionSelection.addItem("256 samples", 2);
    resolutionSelection.addItem("512 samples", 3);
    resolutionSelection.addItem("1024 samples", 4);
    resolutionSelection.addItem("2048 samples", 5);
    resolutionSelection.addItem("4096 samples", 6);
    resolutionAttachment.reset(new ComboBoxAttachment(*state, "wavetableResolution", resolutionSelection));
    
    noteMapPersistsLabel.setLookAndFeel(&lookAndFeel);
    noteMapPersistsLabel.setFont(rfont);
    noteMapPersistsLabel.setText("note map persists: ", dontSendNotification);
    
    noteMapPersistsButton.setLookAndFeel(&lookAndFeel);
    toggleNoteMapAttachment.reset(new ButtonAttachment(*state, "noteMapPersists", noteMapPersistsButton));
    
    parent.addAndMakeVisible(&titleLabel);
    parent.addAndMakeVisible(&resolutionLabel);
    parent.addAndMakeVisible(&resolutionSelection);
    parent.addAndMakeVisible(&noteMapPersistsLabel);
    parent.addAndMakeVisible(&noteMapPersistsButton);
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
    noteMapPersistsLabel.setVisible(true);
    noteMapPersistsButton.setVisible(true);
    
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
    
    width = GlyphArrangement::getStringWidth(noteMapPersistsLabel.getFont(), noteMapPersistsLabel.getText()) + 10;
    height = noteMapPersistsLabel.getFont().getHeight() + 15;
    buffer = bounds.removeFromTop(height);
    noteMapPersistsLabel.setBounds(buffer.removeFromLeft(width));
    noteMapPersistsButton.setBounds(buffer.removeFromLeft(148));
}
