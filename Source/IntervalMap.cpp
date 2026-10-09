#include "IntervalMap.h"
#include "Map.h"

#include <iostream>
#include <fstream>

using namespace juce;
using namespace std;

IntervalMap::IntervalMap() : baseMap(Map()), noteMaps() {}

IntervalMap::IntervalMap(Map bm, unordered_map<int, Map> nms) : baseMap(bm), noteMaps(nms) {}

IntervalMap::~IntervalMap()
{
    noteMaps.clear();
}

Value IntervalMap::getLastKnownFilePath(juce::AudioProcessorValueTreeState* pluginState)
{
    jassert(pluginState);
    
    return pluginState->state.getPropertyAsValue("lastKnownFilePath", nullptr, true);
}

void IntervalMap::resetLastKnownFilePath(juce::AudioProcessorValueTreeState* pluginState)
{
    if (!pluginState)
    {
        return;
    }
    
    getLastKnownFilePath(pluginState).setValue(juce::var::undefined());
}
