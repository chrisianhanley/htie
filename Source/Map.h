#pragma once

#include <JuceHeader.h>

struct Map
{
    std::string name;
    std::unordered_map<int, double> map;
    
    bool operator==(const Map& m) const
    {
        return name == m.name && map == m.map;
    }
};
