#include "IntervalMap.h"
#include "Map.h"
#include "PluginParameters.h"

#include <iostream>
#include <fstream>

using namespace juce;

IntervalMap::IntervalMap(Map bm, unordered_map<int, Map> nms) : baseMap(bm), noteMaps(nms) {}

IntervalMap::~IntervalMap()
{
    noteMaps.clear();
}

Value IntervalMap::getLastKnownFilePath()
{
    auto state = PluginParameters::getPluginState();
    
    jassert(state);
    
    return state->state.getPropertyAsValue("lastKnownFilePath", nullptr, true);
}

void IntervalMap::resetLastKnownFilePath()
{
    getLastKnownFilePath().setValue(juce::var::undefined());
}
