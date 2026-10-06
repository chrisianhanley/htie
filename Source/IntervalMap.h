#pragma once

#include <JuceHeader.h>
#include <unordered_map>

#include "Map.h"

using namespace std;

class IntervalMap
{
public:
    IntervalMap(Map, unordered_map<int, Map>);
    ~IntervalMap();
    
    Map baseMap;
    unordered_map<int, Map> noteMaps;
    
    static juce::Value getLastKnownFilePath();
    static void resetLastKnownFilePath();
};

