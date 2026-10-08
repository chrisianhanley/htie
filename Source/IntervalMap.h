#pragma once

#include <JuceHeader.h>
#include <unordered_map>
#include "PluginParameters.h"

#include "Map.h"

using namespace std;

struct IntervalMap
{
public:
    IntervalMap();
    IntervalMap(Map, unordered_map<int, Map>);
    
    ~IntervalMap();
    
    Map baseMap;
    unordered_map<int, Map> noteMaps;
    
    juce::Value getLastKnownFilePath(juce::AudioProcessorValueTreeState*);
    void resetLastKnownFilePath(juce::AudioProcessorValueTreeState*);
    
    void clear();
    
    bool isEmpty();
 
    bool operator==(const IntervalMap& m) const
    {
        return baseMap.map == m.baseMap.map && noteMaps == m.noteMaps;
    }
};

