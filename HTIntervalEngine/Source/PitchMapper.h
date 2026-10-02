#pragma once

#include <JuceHeader.h>
#include "IntervalMap.h"
#include "Map.h"

class PitchMapper
{
public:
    static const bool USE_SHARPS;
    static const int OCTAVE_FOR_MIDDLE_C;
    
    static std::unordered_map<juce::String, int> noteToSemitones;
    
    static int loadIntervalMap(juce::File* json);
    static float ratioToDecimal(std::string ratio);
    static float ratioToCents(float ratio);
    static float centsToRatio(float cents);
    
    static int getSelectedNoteMapIndex();
    static int setNoteMap(unsigned int index, bool notify);
    
    static int getCurrentRootInterval();
    static void setCurrentRootInterval(int root);
    static int getCurrentRootAsSemitones();
    static juce::String getCurrentRootNote();
    
    static juce::String getNoteNumberAsNote(unsigned int noteNumber);
    static int getInterval(unsigned int a, unsigned int b);
    static int getNoteAsSemitones(juce::String note);
    static juce::String getSemitonesAsNote(unsigned int semitones);
    static juce::String getIntervalAsNote(unsigned int from, unsigned int interval);
    static float intervalToCents12(unsigned int interval);
    
    static float map(int midiNoteNumber, bool useNoteMap);
    static float map(int midiNoteNumber, float transposeCents, bool useNoteMap);
    static float mapRelative(int midiNoteNumber, int root, bool useNoteMap);
    
    static IntervalMap* getCurrentIntervalMap();
    
    static void reset(bool notify);
    
    static std::function<void()> onMapChangeAsync;
    static std::function<void()> onMapChangeSync;
    static std::function<void()> onRootIntervalChangeAsync;
    
    static bool isSubstituted(unsigned int noteNumber);
    
private:
    PitchMapper();
    
    static int selectedNoteMapIndex;
    static int currentRootInterval;
    
    static std::unique_ptr<IntervalMap> currentIntervalMap;
};

