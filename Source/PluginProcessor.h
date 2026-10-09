#pragma once

#include <JuceHeader.h>
#include "PitchMapper.h"
#include "SoundProfile.h"

//==============================================================================
/**
*/

class HTIntervalEngineAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    HTIntervalEngineAudioProcessor();
    ~HTIntervalEngineAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
   #endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    
    bool noteWithinRange(int noteNumber);
    
    juce::OwnedArray<SoundProfile>& getSoundProfiles();
    
    SoundProfile* getSelectedSoundProfile();
    SoundProfile* setSelectedSoundProfile(int profileId);
    
    juce::MidiKeyboardState& getKeyboardState();
    
    juce::AudioProcessorValueTreeState& getPluginState();
    PluginParameters& getPluginParameters();
    PitchMapper& getPitchMapper();
    SynthEngine& getSynthEngine();
    
    static const juce::String FILE_TEXT_EMPTY;
    
    juce::String fileTextOutput;
    juce::String fileTextBuffer = "";
    juce::String fileTextAppend = "";
    
    bool resetOutputText = false;
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HTIntervalEngineAudioProcessor)
    
    juce::AudioProcessorValueTreeState pluginState;
    
    PluginParameters parameters;
    
    PitchMapper pitchMapper;
    
    SynthEngine synthEngine;
    
    juce::OwnedArray<SoundProfile> soundProfiles;
    
    SoundProfile* selectedSoundProfile;
    
    juce::MidiKeyboardState keyboardState;
    
    bool isAddingFromMidiInput = false;
    
    int lastRootNote = -1;
    
    bool pedalEnabled = false;
    
    //void handleNoteOn(juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;
    //void handleNoteOff(juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;
    //void handleIncomingMidiMessage(juce::MidiInput* input, const juce::MidiMessage& message) override;
    
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
};

using Processor = HTIntervalEngineAudioProcessor;
