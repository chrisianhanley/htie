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
    static void initializeValues(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    static void createReferences(juce::AudioProcessorValueTreeState* state);
    
    static std::atomic<float>* x1;
    static std::atomic<float>* x2;
    static std::atomic<float>* x3;
    static std::atomic<float>* x4;
    static std::atomic<float>* x5;
    static std::atomic<float>* x6;
};
