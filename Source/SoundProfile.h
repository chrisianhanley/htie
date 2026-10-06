/*
  ==============================================================================

    SoundProfil.e.h
    Created: 30 Oct 2025 2:40:48pm
    Author:  Ian Hanley

  ==============================================================================
*/

#pragma once

#include "SynthEngine.h"
#include "Window.h"

class SoundProfile
{
public:
    SoundProfile(SynthEngine& engine, unsigned int i);
    
    virtual ~SoundProfile();
    
    SynthEngine& getSynthEngine();
    
    unsigned int getProfileId();
    
    virtual juce::String getDisplayName() = 0;
    
    virtual Window* createWindow(juce::AudioProcessorEditor& e) = 0;
    
    virtual void enable() = 0;
    virtual void update() = 0;
    virtual void disable() = 0;
    
protected:
    SynthEngine& synthEngine;
    
    unsigned int profileId;
};
