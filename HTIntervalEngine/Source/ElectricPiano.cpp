#include "ElectricPiano.h"

ElectricPiano::ElectricPiano(SynthEngine& engine, unsigned int i) : SoundProfile(engine, i), numVoices(8)
{
    
}

juce::String ElectricPiano::getDisplayName()
{
    return "electric piano";
}

Window* ElectricPiano::createWindow(juce::AudioProcessorEditor& parent)
{
    return nullptr;
}

void ElectricPiano::enable()
{

}

void ElectricPiano::update()
{
    
}

void ElectricPiano::disable()
{
    
}

