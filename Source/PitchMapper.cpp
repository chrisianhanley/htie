/*
  ==============================================================================

    PitchMapper.cpp
    Created: 14 Sep 2025 7:09:18pm
    Author:  Ian Hanley

  ==============================================================================
*/

#include "PitchMapper.h"
#include "PluginProcessor.h"
#include "PluginParameters.h"

#include <iostream>
#include <fstream>

using namespace juce;

// cents = 1200 × log2(ratio)
// ratio = 2^(1200/cents)

const bool PitchMapper::USE_SHARPS = true;
const int PitchMapper::OCTAVE_FOR_MIDDLE_C = 3;

unordered_map<String, int> PitchMapper::noteToSemitones = {
    { "C", 0 }, { "C#", 1 }, { "D", 2 }, { "D#", 3 }, { "E", 4 }, { "F", 5 },
    { "F#", 6 }, { "G", 7 }, { "G#", 8 }, { "A", 9 }, { "A#", 10 }, { "B", 11 }
};

PitchMapper::PitchMapper(PluginParameters& p) : parameters(p), currentIntervalMap(nullptr) {}

float PitchMapper::ratioToDecimal(string ratio)
{
    float a;
    float b;
    
    stringstream stream(ratio);
    string segment;
    
    getline(stream, segment, ':');
    a = round(stoi(segment));
    getline(stream, segment, ':');
    b = round(stoi(segment));
    
    cout << "converted ratio: " + to_string((int) a) + ":" + to_string((int) b) + " = " + to_string(a / b) << endl;
    
    return a / b;
}

float PitchMapper::ratioToCents(float ratio)
{
    return 1200 * log2(ratio);
}

float PitchMapper::centsToRatio(float cents)
{
    return pow(2, cents / 1200);
}

int PitchMapper::loadIntervalMap(File* json)
{
    if (!json->existsAsFile())
    {
        return -1;
    }
    
    var contents = JSON::parse(*json);
    
    Map baseMap;
    unordered_map<int, Map> noteMaps;
    
    if (contents.isObject())
    {
        if (auto* obj = contents.getDynamicObject())
        {
            if (obj->getProperty("basemap"))
            {
                if (auto* bmp = obj->getProperty("basemap").getDynamicObject())
                {
                    auto name = bmp->getProperty("name").toString().toStdString();
                    if (name.size() > 30)
                    {
                        name.resize(30);
                        name.append("...");
                    }
                    
                    baseMap.name = name;
                    
                    if (bmp->getProperty("intervals"))
                    {
                        if (auto* intervals = bmp->getProperty("intervals").getDynamicObject())
                        {
                            cout << "loading interval map [" + name + "]" << endl;
                            auto properties = intervals->getProperties();
                            for (const auto& prop : properties)
                            {
                                int key = prop.name.toString().getIntValue();
                                double value = -1;
                                
                                if (prop.value.isString())
                                {
                                    auto ratio = ratioToDecimal(prop.value.toString().toStdString());
                                    auto cents = ratioToCents(ratio);
                                    
                                    value = cents;
                                }
                                else if (prop.value.isInt() || prop.value.isDouble())
                                {
                                    value = (double) prop.value;
                                }
                                
                                if (value >= 0)
                                {
                                    baseMap.map[key] = value;
                                    
                                    cout << to_string(key) + " -> " + to_string(value) << endl;
                                    
                                    
                                }
                            }
                            
                            if (baseMap.map.size() != 12)
                            {
                                return -2;
                            }
                        }
                    }
                }
            }
            
            if (obj->getProperty("notemaps"))
            {
                if (auto* nms = obj->getProperty("notemaps").getDynamicObject())
                {
                    for (int i = 0; i <= 11; i++)
                    {
                        juce::String _id = to_string(i);
                        if (nms->getProperty(_id))
                        {
                            if (auto* nm = nms->getProperty(_id).getDynamicObject())
                            {
                                Map nmObj;
                                auto name = nm->getProperty("name").toString().toStdString();
                                
                                if (name.size() > 30)
                                {
                                    name.resize(30);
                                    name.append("...");
                                }
                                
                                nmObj.name = name;
                                
                                if (nm->getProperty("intervals"))
                                {
                                    cout << "loading note map [" + name + "]" << endl;
                                    
                                     if (auto* intervals = nm->getProperty("intervals").getDynamicObject())
                                     {
                                         auto interval = intervals->getProperties();
                                         for (const auto& i : interval)
                                         {
                                             int key = i.name.toString().getIntValue();
                                             double value = -1;
                                             
                                             if (i.value.isString())
                                             {
                                                 auto ratio = ratioToDecimal(i.value.toString().toStdString());
                                                 auto cents = ratioToCents(ratio);
                                                 
                                                 value = cents;
                                             }
                                             else if (i.value.isInt() || i.value.isDouble())
                                             {
                                                 value = (double) i.value;
                                             }
                                             
                                             if (value >= 0)
                                             {
                                                 nmObj.map[key] = value;
                                                 
                                                 cout << to_string(key) + " -> " + to_string(value) << endl;
                                             }
                                         }
                                     }
                                }

                                int key = _id.getIntValue();
                                noteMaps[key] = nmObj;
                            }
                        }
                    }
                }
            }
        }
    }
    
    /*
    if (!noteMaps.empty())
    {
        // debug
        cout << "loading (" + to_string(noteMaps.size()) + ") note maps" << endl;
        for (auto nms = noteMaps.begin(); nms != noteMaps.end(); nms++)
        {
            auto _id = (*nms).first;
            auto nm = (*nms).second;
            
            cout << "[" + to_string(_id) + "] " + nm.name << endl;
            
            for (auto in = nm.map.begin(); in != nm.map.end(); in++)
            {
                auto key = (*in).first;
                auto interval = (*in).second;
                cout << to_string(key) + " -> interval: " + to_string(interval) << endl;
            }
        }
    }
    */
    
    atomic_store(&currentIntervalMap, make_shared<IntervalMap>(baseMap, noteMaps));
    
    auto im = getCurrentIntervalMap().get();
    
    auto path = IntervalMap::getLastKnownFilePath(parameters.getPluginState());
    path.setValue(json->getFullPathName());
    
    setNoteMap(0, false);
    
    cout << "successfully loaded interval map [" + im->baseMap.name + "]" << endl;
    return 0;
}

int PitchMapper::getSelectedNoteMapIndex()
{
    return selectedNoteMapIndex;
}

int PitchMapper::setNoteMap(unsigned int index, bool notify)
{
    auto im = getCurrentIntervalMap().get();
    
    if (!im)
    {
        selectedNoteMapIndex = 0;
        
        return selectedNoteMapIndex;
    }
    
    if (index == selectedNoteMapIndex || im->noteMaps.find(index) == im->noteMaps.end())
    {
        index = 0;
    }
    
    selectedNoteMapIndex = index;
    
    if (notify)
    {
        onMapChangeSync = true;
        
        onMapChangeAsync = true;
        
        onRootIntervalChange = true;
    }
    
    return selectedNoteMapIndex;
}

int PitchMapper::getCurrentRootInterval()
{
    return currentRootInterval;
}

void PitchMapper::setCurrentRootInterval(int root)
{
    jassert(root > -1 && root < 12);
    
    currentRootInterval = root;
    
    onMapChangeSync = true;
    
    onMapChangeAsync = true;
    
    onRootIntervalChange = true;
}

int PitchMapper::getCurrentRootAsSemitones()
{
    int semitone = (*parameters.keyCenterParameter - 1) + currentRootInterval;
    
    if  (semitone >= 12)
    {
        semitone -= 12;
    }
    
    return semitone;
}

String PitchMapper::getCurrentRootNote()
{
    return getIntervalAsNote(*parameters.keyCenterParameter - 1, currentRootInterval);
}

juce::String PitchMapper::getNoteNumberAsNote(unsigned int noteNumber)
{
    return MidiMessage::getMidiNoteName(noteNumber, USE_SHARPS, false, OCTAVE_FOR_MIDDLE_C);
}

int PitchMapper::getInterval(unsigned int a, unsigned int b) {
    if (a > b)
    {
        b += 12;
    }
    
    int i = b - a;
    
    jassert(i < 12);
    
    return i;
}

int PitchMapper::getNoteAsSemitones(String note) {
    auto search = noteToSemitones.find(note);
    
    jassert(search != noteToSemitones.end());
    
    return search->second;
}


String PitchMapper::getSemitonesAsNote(unsigned int semitone) {
    auto it = find_if(begin(noteToSemitones), end(noteToSemitones), [&semitone] (auto&& p) { return p.second == semitone; });
    
    jassert(it != end(noteToSemitones));
    
    return it->first;
}

String PitchMapper::getIntervalAsNote(unsigned int from, unsigned int interval) {
    int semitone = from + interval;
    
    if  (semitone >= 12)
    {
        semitone -= 12;
    }
    
    auto it = find_if(std::begin(noteToSemitones), end(noteToSemitones), [&semitone] (auto&& p) { return p.second == semitone; });
    
    jassert(it != end(noteToSemitones));
    
    return it->first;
}

float PitchMapper::intervalToCents12(unsigned int interval)
{
    return interval * 100;
}

float PitchMapper::map(int midiNoteNumber, bool useNoteMap)
{
    const float startingFrequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    
    if (auto im = getCurrentIntervalMap().get())
    {
        auto name = getNoteNumberAsNote(midiNoteNumber);
        
        auto& baseMap = im->baseMap;
        auto rootNote = getCurrentRootAsSemitones();
        auto inputNote = getNoteAsSemitones(name);
        auto interval = getInterval(rootNote, inputNote);
        
        float startCents = 0;
        float targetCents = 0;
        float difference = 0;
        
        float mappedFrequency = startingFrequency;
        
        cout << "mapped " + name + " to " + to_string(interval) << endl;
        
        if (*parameters.quantizeRootParameter == 0)
        {
            startCents = intervalToCents12(currentRootInterval);
            targetCents = baseMap.map.at(currentRootInterval);
            difference = targetCents - startCents;
            
            cout << "root difference: " + to_string(difference) << endl;
            
            if (inputNote == rootNote)
            {
                mappedFrequency = startingFrequency * centsToRatio(difference);
                
                cout << "starting frequency: " + to_string(MidiMessage::getMidiNoteInHertz(midiNoteNumber)) + ", mapped frequency: " + to_string(mappedFrequency) << endl;
                
                return mappedFrequency;
            }
        }
        else if (inputNote == rootNote)
        {
            cout << "note is root, quantized" << endl;
            
            return startingFrequency;
        }
        
        auto& nms = im->noteMaps;
        if (useNoteMap && nms.find(selectedNoteMapIndex) != nms.end() && nms.at(selectedNoteMapIndex).map.find(interval) != nms.at(selectedNoteMapIndex).map.end())
        {
            if (inputNote != rootNote)
            {
                startCents = intervalToCents12(interval);
                targetCents = nms.at(selectedNoteMapIndex).map.at(interval);
                difference += targetCents - startCents;
                
                mappedFrequency = startingFrequency * centsToRatio(difference);
                
                cout << "difference: " + to_string(difference) << endl;
                cout << "starting frequency: " + to_string(MidiMessage::getMidiNoteInHertz(midiNoteNumber)) + ", mapped frequency: " + to_string(mappedFrequency) << endl;
            }
        }
        else
        {
            startCents = intervalToCents12(interval);
            targetCents = baseMap.map.at(interval);
            difference += targetCents - startCents;
            
            mappedFrequency = startingFrequency * centsToRatio(difference);
            
            cout << "total difference: " + to_string(difference) << endl;
            cout << "starting frequency: " + to_string(MidiMessage::getMidiNoteInHertz(midiNoteNumber)) + ", mapped frequency: " + to_string(mappedFrequency) << endl;
        }
        
        return mappedFrequency;
    }
    
    return startingFrequency;
}

float PitchMapper::map(int midiNoteNumber, float transposeCents, bool useNoteMap)
{
    return map(midiNoteNumber, useNoteMap) * centsToRatio(transposeCents);
}

float PitchMapper::mapRelative(int midiNoteNumber, int root, bool useNoteMap)
{
    const float startingFrequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    
    if (auto im = getCurrentIntervalMap().get())
    {
        auto name = getNoteNumberAsNote(midiNoteNumber);
        
        auto& baseMap = im->baseMap;
        auto rootNote = getCurrentRootAsSemitones() + root;
        
        rootNote %= 12;
        
        auto inputNote = getNoteAsSemitones(name);
        auto interval = getInterval(rootNote, inputNote);
        
        float startCents = 0;
        float targetCents = 0;
        float difference = 0;
        
        float mappedFrequency = startingFrequency;
        
        cout << "mapped " + name + " to " + to_string(interval) << endl;
        
        if (*parameters.quantizeRootParameter == 0)
        {
            startCents = intervalToCents12(currentRootInterval);
            targetCents = baseMap.map.at(currentRootInterval);
            difference = targetCents - startCents;
            
            cout << "root difference: " + to_string(difference) << endl;
            
            if (inputNote == rootNote)
            {
                mappedFrequency = startingFrequency * centsToRatio(difference);
                
                cout << "starting frequency: " + to_string(MidiMessage::getMidiNoteInHertz(midiNoteNumber)) + ", mapped frequency: " + to_string(mappedFrequency) << endl;
                
                return mappedFrequency;
            }
        }
        else if (inputNote == rootNote)
        {
            cout << "note is root, quantized" << endl;
            
            return startingFrequency;
        }
        
        auto& nms = im->noteMaps;
        if (useNoteMap && nms.find(selectedNoteMapIndex) != nms.end() && nms.at(selectedNoteMapIndex).map.find(interval) != nms.at(selectedNoteMapIndex).map.end())
        {
            if (inputNote != rootNote)
            {
                startCents = intervalToCents12(interval);
                targetCents = nms.at(selectedNoteMapIndex).map.at(interval);
                difference += targetCents - startCents;
                
                mappedFrequency = startingFrequency * centsToRatio(difference);
                
                cout << "difference: " + to_string(difference) << endl;
                cout << "starting frequency: " + to_string(MidiMessage::getMidiNoteInHertz(midiNoteNumber)) + ", mapped frequency: " + to_string(mappedFrequency) << endl;
            }
        }
        else
        {
            startCents = intervalToCents12(interval);
            targetCents = baseMap.map.at(interval);
            difference += targetCents - startCents;
            
            mappedFrequency = startingFrequency * centsToRatio(difference);
            
            cout << "total difference: " + to_string(difference) << endl;
            cout << "starting frequency: " + to_string(MidiMessage::getMidiNoteInHertz(midiNoteNumber)) + ", mapped frequency: " + to_string(mappedFrequency) << endl;
        }
        
        return mappedFrequency;
    }
    
    return startingFrequency;
}

void PitchMapper::reset(bool notify)
{
    IntervalMap::resetLastKnownFilePath(parameters.getPluginState());
    
    currentIntervalMap.reset();

    setNoteMap(0, notify);
}

bool PitchMapper::isSubstituted(unsigned int noteNumber)
{
    if (auto im = getCurrentIntervalMap().get())
    {
        auto rootNote = getCurrentRootAsSemitones();
        auto inputNote = getNoteAsSemitones(getNoteNumberAsNote(noteNumber));
        auto interval = getInterval(rootNote, inputNote);
        
        auto& nms = im->noteMaps;
        if (nms.find(selectedNoteMapIndex) != nms.end() && nms.at(selectedNoteMapIndex).map.find(interval) != nms.at(selectedNoteMapIndex).map.end() && inputNote != rootNote)
        {
            return true;
        }
    }
    
    return false;
}

shared_ptr<IntervalMap> PitchMapper::getCurrentIntervalMap()
{
    return atomic_load(&currentIntervalMap);
}
