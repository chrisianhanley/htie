#pragma once

#include <JuceHeader.h>
#include "PitchMapper.h"
#include "SynthEngine.h"
#include "libMTSMaster.h"

//==============================================================================
/**
*/

class HTIntervalEngineAudioProcessor : public juce::AudioProcessor, juce::AsyncUpdater, juce::AudioProcessorValueTreeState::Listener
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
    
    juce::MidiKeyboardState& getKeyboardState();
    
    juce::AudioProcessorValueTreeState& getPluginState();
    
    PluginParameters& getPluginParameters();
    
    PitchMapper& getPitchMapper();
    
    SynthEngine& getSynthEngine();
    
    void pushMTSTuning();
    
    void filterMTSTuning();
    
    static const juce::String FILE_TEXT_EMPTY;
    
    juce::String fileTextOutput;
    
    juce::String fileTextBuffer = "";
    
    juce::String fileTextAppend = "";
    
    bool resetOutputText = false;
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HTIntervalEngineAudioProcessor)
    
    juce::AudioProcessorValueTreeState pluginState;
    
    juce::MidiKeyboardState keyboardState;
    
    PluginParameters parameters;
    
    PitchMapper pitchMapper;
    
    SynthEngine synthEngine;
    
    int lastRootNote = -1;
    
    bool pedalEnabled = false;
  
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    
    void handleAsyncUpdate() override;
    
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    
    bool isMTSMaster = false;
};

using Processor = HTIntervalEngineAudioProcessor;
