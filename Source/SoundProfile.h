/*
  ==============================================================================

    SoundProfil.e.h
    Created: 30 Oct 2025 2:40:48pm
    Author:  Ian Hanley

  ==============================================================================
*/

#pragma once

#include "PluginParameters.h"
#include "SynthEngine.h"
#include "Window.h"

struct SoundProfile
{
public:
    SoundProfile(SynthEngine& engine, PitchMapper& pm, PluginParameters& p, unsigned int i);
    
    virtual ~SoundProfile();
    
    SynthEngine& synthEngine;
    
    PitchMapper& pitchMapper;
    
    PluginParameters& parameters;
    
    const unsigned int profileId;
    
    virtual juce::String getDisplayName() = 0;
    
    virtual Window* createWindow(juce::AudioProcessorEditor& e) = 0;
    
    virtual void enable() = 0;
    
    virtual void update() = 0;
    
    virtual void disable() = 0;
};
