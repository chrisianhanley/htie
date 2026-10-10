#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Map.h"

//==============================================================================

using namespace juce;
using namespace std;

//using Parameter = AudioProcessorValueTreeState::Parameter;

const String HTIntervalEngineAudioProcessor::FILE_TEXT_EMPTY = "[empty]";

HTIntervalEngineAudioProcessor::HTIntervalEngineAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor(BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput("Output", AudioChannelSet::stereo(), true)
                     #endif
                      ), fileTextOutput(FILE_TEXT_EMPTY), pluginState(*this, nullptr, Identifier("HTIntervalEngine"), createLayout()), pitchMapper(parameters), synthEngine(parameters, pitchMapper)
#endif
{
    parameters.createReferences(&pluginState);
    
    if (MTS_CanRegisterMaster())
    {
        MTS_RegisterMaster();
        
        isMTSMaster = true;
    }
    
    pluginState.addParameterListener("keyCenter", this);
    
    pluginState.addParameterListener("rootInputRange", this);
    
    triggerAsyncUpdate();
    
    synthEngine.createWavetable();
    
    synthEngine.addListeners();
}

HTIntervalEngineAudioProcessor::~HTIntervalEngineAudioProcessor()
{
    if (isMTSMaster)
    {
        MTS_DeregisterMaster();
    }
    
    pluginState.removeParameterListener("keyCenter", this);
    
    pluginState.removeParameterListener("rootInputRange", this);
    
    cancelPendingUpdate();
}

void HTIntervalEngineAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    triggerAsyncUpdate();
}

void HTIntervalEngineAudioProcessor::handleAsyncUpdate()
{
    pushMTSTuning();
    filterMTSTuning();
}

void HTIntervalEngineAudioProcessor::pushMTSTuning()
{
    double freqs[128];
    for (int i = 0; i < 128; ++i)
    {
        freqs[i] = pitchMapper.map(i, true);
    }
    
    MTS_SetNoteTunings(freqs);
    
    if (pitchMapper.getCurrentIntervalMap())
    {
        MTS_SetScaleName(pitchMapper.getCurrentIntervalMap().get()->baseMap.name.c_str());
    }
}

void HTIntervalEngineAudioProcessor::filterMTSTuning()
{
    for (int i = 0; i < 128; ++i)
    {
        MTS_FilterNote(noteWithinRange(i), (char) i, -1);
    }
}

AudioProcessorValueTreeState::ParameterLayout HTIntervalEngineAudioProcessor::createLayout()
{
    AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(make_unique<juce::AudioParameterInt>(ParameterID { "keyCenter", 1 }, "key", 1, 12, 1));
    layout.add(make_unique<juce::AudioParameterInt>(ParameterID { "rootInputRange", 1 }, "input range", 1, 10, 5));
    layout.add(make_unique<juce::AudioParameterBool>(ParameterID { "quantizeRoot", 1 }, "quantize", false));
    layout.add(make_unique<juce::AudioParameterBool>(ParameterID { "drone", 1 }, "drone", false));
    layout.add(make_unique<juce::AudioParameterInt>(ParameterID { "wavetableResolution", 1 }, "wavetable resolution", 1, 6, 5));
    layout.add(make_unique<juce::AudioParameterBool>(ParameterID { "noteMapPersists", 1 }, "nm persists", true));
    layout.add(make_unique<juce::AudioParameterInt>(ParameterID { "superimpose", 1 }, "superimpose", 1, 12, 12));
    layout.add(make_unique<juce::AudioParameterFloat>(ParameterID { "mix", 1 }, "mix", 0, 1, 0.5f));
    layout.add(make_unique<juce::AudioParameterInt>(ParameterID { "numVoices", 1 }, "num voices", 1, 11, 1));
    layout.add(make_unique<juce::AudioParameterFloat>(ParameterID { "gain", 1 }, "gain", -100, -12, -18));
    
    Mixer::createLayout(layout);
    
    return layout;
}

AudioProcessorValueTreeState& HTIntervalEngineAudioProcessor::getPluginState()
{
    return pluginState;
}

PluginParameters& HTIntervalEngineAudioProcessor::getPluginParameters()
{
    return parameters;
}

PitchMapper& HTIntervalEngineAudioProcessor::getPitchMapper()
{
    return pitchMapper;
}

SynthEngine& HTIntervalEngineAudioProcessor::getSynthEngine()
{
    return synthEngine;
}

MidiKeyboardState& HTIntervalEngineAudioProcessor::getKeyboardState()
{
    return keyboardState;
}

bool HTIntervalEngineAudioProcessor::noteWithinRange(int noteNumber) {
    auto name = MidiMessage::getMidiNoteName(noteNumber, pitchMapper.USE_SHARPS, true, pitchMapper.OCTAVE_FOR_MIDDLE_C);
    auto range = (String) to_string((int) *parameters.rootInputRangeParameter - 3);
  
    auto s = name.substring(name.length() - 2, name.length());
    if (s.contains("-"))
    {
        return s.equalsIgnoreCase(range);
    }
    
    return name.contains(range);
}

/*
bool HTIntervalEngineAudioProcessor::isPlaying()
{
    auto p = getPlayHead();
    auto playing = false;
    
    if (p != nullptr)
    {
        auto pos = p->getPosition();
        playing = pos->getIsPlaying();
    }
    
    return playing;
}
*/

//==============================================================================
const String HTIntervalEngineAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool HTIntervalEngineAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool HTIntervalEngineAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool HTIntervalEngineAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double HTIntervalEngineAudioProcessor::getTailLengthSeconds() const
{
    return 0;
}

int HTIntervalEngineAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int HTIntervalEngineAudioProcessor::getCurrentProgram()
{
    return 0;
}

void HTIntervalEngineAudioProcessor::setCurrentProgram(int index) {}

const String HTIntervalEngineAudioProcessor::getProgramName(int index)
{
    return {};
}

void HTIntervalEngineAudioProcessor::changeProgramName(int index, const String& newName) {}

//==============================================================================
void HTIntervalEngineAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synthEngine.prepareToPlay(samplesPerBlock, sampleRate);
    
    lastRootNote = -1;
}

void HTIntervalEngineAudioProcessor::releaseResources()
{
    synthEngine.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool HTIntervalEngineAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused(layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if(layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if(layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void HTIntervalEngineAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);
    
    if (pitchMapper.onMapChangeSync.exchange(false) || pitchMapper.onMapLoad.exchange(false))
    {
        synthEngine.redrawVoices();
        
        triggerAsyncUpdate();
    }
    
    MidiBuffer filteredMessages;
    
    auto pedalNote = synthEngine.getPedalNote();
    
    if (*parameters.droneParameter == 0 && pedalNote > -1)
    {
        auto off = MidiMessage::noteOff(synthEngine.getPedalChannel(), pedalNote);
        
        filteredMessages.addEvent(off, 0);
        
        synthEngine.setPedalNote(-1);
        pedalEnabled = false;
    }
    
    for (const auto data : midiMessages)
    {
        bool addEvent = true;
        
        const auto time = data.samplePosition;
        const auto message = data.getMessage();
        const auto currentNoteNumber = message.getNoteNumber();
        
        pedalNote = synthEngine.getPedalNote();
        
        if (message.isNoteOnOrOff() && noteWithinRange(currentNoteNumber))
        {
            addEvent = false;
            
            if (message.isNoteOn())
            {
                auto registerFlag = true;
                auto pedalFlag = true;
                
                auto currentNote = pitchMapper.getNoteNumberAsNote(currentNoteNumber);
                auto currentSemitones = pitchMapper.getNoteAsSemitones(currentNote);
                
                // calculate distance from current midi note to key center to compare to the root interval
                auto keyInterval = pitchMapper.getInterval(*parameters.keyCenterParameter - 1, currentSemitones);
                
                if (pitchMapper.getCurrentIntervalMap() && currentNoteNumber != lastRootNote)
                {
                    // cancel operation if current note is identical to the root note, wait for additional input
                    if (keyInterval == pitchMapper.getCurrentRootInterval())
                    {
                        lastRootNote = currentNoteNumber;
                        registerFlag = false;
                    }
                    
                    // calculate distance from current held root note
                    else if (lastRootNote > -1)
                    {
                        auto lastSemitones = pitchMapper.getNoteAsSemitones(pitchMapper.getNoteNumberAsNote(lastRootNote));
                        auto lastInterval = pitchMapper.getInterval(lastSemitones, currentSemitones);
                    
                        auto lastIndex = pitchMapper.getSelectedNoteMapIndex();
                        if (lastIndex != pitchMapper.setNoteMap(lastInterval, true))
                        {
                            pedalEnabled = true;
                            registerFlag = false;
                            pedalFlag = false;
                        }
                    }
                }
                
                // register new root note
                if (registerFlag)
                {
                    if (!*parameters.noteMapPersistsParameter)
                    {
                        pitchMapper.setNoteMap(0, true);
                    }
                    
                    pitchMapper.setCurrentRootInterval(keyInterval);
                    lastRootNote = currentNoteNumber;
                }
                
                if (*parameters.droneParameter == 1 && pedalFlag && currentNoteNumber != pedalNote)
                {
                    if (pedalNote > -1)
                    {
                        auto off = MidiMessage::noteOff(synthEngine.getPedalChannel(), pedalNote);
                        filteredMessages.addEvent(off, time);
                    }
                    
                    synthEngine.setPedalNote(currentNoteNumber, message.getChannel());
                    
                    pedalEnabled = true;
                    addEvent = true;
                }
            }
            else if (message.isNoteOff())
            {
                if (currentNoteNumber == lastRootNote)
                {
                    lastRootNote = -1;
                }
                
                if (currentNoteNumber == pedalNote)
                {
                    if (!pedalEnabled)
                    {
                        synthEngine.setPedalNote(-1);
                        
                        addEvent = true;
                    }
                    else
                    {
                        pedalEnabled = false;
                    }
                }
            }
        }
        
        if (addEvent)
        {
            filteredMessages.addEvent(message, time);
        }
    }
    
    AudioSourceChannelInfo bufferToFill;
    
    bufferToFill.buffer = &buffer;
    bufferToFill.startSample = 0;
    bufferToFill.numSamples = buffer.getNumSamples();
    
    synthEngine.setCurrentBuffer(filteredMessages);
    synthEngine.getNextAudioBlock(bufferToFill);

    midiMessages.clear();
}

//==============================================================================
bool HTIntervalEngineAudioProcessor::hasEditor() const
{
    return true; //(change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* HTIntervalEngineAudioProcessor::createEditor()
{
    return new HTIntervalEngineAudioProcessorEditor(*this);
}

//==============================================================================
void HTIntervalEngineAudioProcessor::getStateInformation(MemoryBlock& destData)
{
    auto state = pluginState.copyState();
    std::unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void HTIntervalEngineAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(pluginState.state.getType()))
        {
            pitchMapper.reset(false);
            
            pluginState.replaceState(ValueTree::fromXml(*xmlState));

            auto value = IntervalMap::getLastKnownFilePath(&pluginState).getValue();
            
            if (!value.isUndefined())
            {
                File file(value);
                
                pitchMapper.loadIntervalMap(&file);
            }
            
            auto map = pitchMapper.getCurrentIntervalMap();
            
            if (map)
            {
                fileTextOutput = "[" + map.get()->baseMap.name + "]";
            }
            else
            {
                fileTextOutput = FILE_TEXT_EMPTY;
            }
            
            synthEngine.createWavetable();
        }
    }
    
    triggerAsyncUpdate();
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HTIntervalEngineAudioProcessor();
}

