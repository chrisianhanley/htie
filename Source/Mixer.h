/*
  ==============================================================================

    Mixer.h
    Created: 30 Oct 2025 12:13:45am
    Author:  Ian Hanley

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class Mixer
{
public:
    static void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createReferences(juce::AudioProcessorValueTreeState* state);
    
    std::atomic<float>* x1;
    std::atomic<float>* x2;
    std::atomic<float>* x3;
    std::atomic<float>* x4;
    std::atomic<float>* x5;
    std::atomic<float>* x6;
};
