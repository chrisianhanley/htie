#pragma once

#include <JuceHeader.h>
#include <unordered_map>
#include "PluginParameters.h"

#include "Map.h"

struct IntervalMap
{
public:
    IntervalMap();
    IntervalMap(Map, std::unordered_map<int, Map>);
    
    ~IntervalMap();
    
    Map baseMap;
    std::unordered_map<int, Map> noteMaps;
    
    static juce::Value getLastKnownFilePath(juce::AudioProcessorValueTreeState*);
    static void resetLastKnownFilePath(juce::AudioProcessorValueTreeState*);
};
