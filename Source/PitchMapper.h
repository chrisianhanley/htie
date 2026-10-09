#pragma once

#include <JuceHeader.h>
#include "IntervalMap.h"
#include "PluginParameters.h"
#include "Map.h"

class PitchMapper
{
public:
    PitchMapper(PluginParameters&);
    
    static const bool USE_SHARPS;
    static const int OCTAVE_FOR_MIDDLE_C;
    
    static std::unordered_map<juce::String, int> noteToSemitones;
    
    int loadIntervalMap(juce::File* json);
    float ratioToDecimal(std::string ratio);
    float ratioToCents(float ratio);
    float centsToRatio(float cents);
    
    int getSelectedNoteMapIndex();
    int setNoteMap(unsigned int index, bool notify);
    
    int getCurrentRootInterval();
    void setCurrentRootInterval(int root);
    int getCurrentRootAsSemitones();
    juce::String getCurrentRootNote();
    
    juce::String getNoteNumberAsNote(unsigned int noteNumber);
    int getInterval(unsigned int a, unsigned int b);
    int getNoteAsSemitones(juce::String note);
    juce::String getSemitonesAsNote(unsigned int semitones);
    juce::String getIntervalAsNote(unsigned int from, unsigned int interval);
    float intervalToCents12(unsigned int interval);
    
    float map(int midiNoteNumber, bool useNoteMap);
    float map(int midiNoteNumber, float transposeCents, bool useNoteMap);
    float mapRelative(int midiNoteNumber, int root, bool useNoteMap);
    
    void reset(bool notify);
    
    std::atomic<bool> onMapChangeSync = false;
    std::atomic<bool> onMapChangeAsync = false;
    std::atomic<bool> onRootIntervalChange = false;
    
    bool isSubstituted(unsigned int noteNumber);
    
    std::shared_ptr<const IntervalMap> currentIntervalMap;
    
private:
    PluginParameters& parameters;
    
    int selectedNoteMapIndex = 0;
    int currentRootInterval = 0;
};

