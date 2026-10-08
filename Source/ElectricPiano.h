#pragma once

#pragma once

#include "SoundProfile.h"
#include "WavetableVoice.h"

class ElectricPiano : public SoundProfile
{
public:
    ElectricPiano(SynthEngine& engine, PitchMapper& pm, PluginParameters& p, unsigned int i);
    
    juce::String getDisplayName() override;
    
    Window* createWindow(juce::AudioProcessorEditor& parent) override;
    
    void enable() override;
    void update() override;
    void disable() override;
    
private:
    unsigned int numVoices;
};
