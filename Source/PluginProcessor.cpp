#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Map.h"

#include "PolyphonicSynthesiser.h"
#include "ElectricPiano.h"

//==============================================================================

using namespace juce;

//using Parameter = AudioProcessorValueTreeState::Parameter;

HTIntervalEngineAudioProcessor::HTIntervalEngineAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor(BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput("Output", AudioChannelSet::stereo(), true)
                     #endif
                      ), pluginState(*this, nullptr, Identifier("HTIntervalEngine"), createLayout()), pitchMapper(parameters), synthEngine(parameters, pitchMapper)
#endif
{
    parameters.createReferences(&pluginState);
    
    soundProfiles.add(new PolyphonicSynthesiser(synthEngine, pitchMapper, parameters, 1));
    soundProfiles.add(new ElectricPiano(synthEngine, pitchMapper, parameters, 2));
    
    selectedSoundProfile = soundProfiles[0];
    selectedSoundProfile->enable();
    
    keyboardState.addListener(this);
}

HTIntervalEngineAudioProcessor::~HTIntervalEngineAudioProcessor() {}

AudioProcessorValueTreeState::ParameterLayout HTIntervalEngineAudioProcessor::createLayout()
{
    AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(make_unique<juce::AudioParameterInt>(ParameterID { "keyCenter", 1 }, "key", 1, 12, 1));
    layout.add(make_unique<juce::AudioParameterInt>(ParameterID { "rootInputRange", 1 }, "input range", 1, 10, 5));
    layout.add(make_unique<juce::AudioParameterBool>(ParameterID { "quantizeRoot", 1 }, "quantize", false));
    layout.add(make_unique<juce::AudioParameterBool>(ParameterID { "pedalRoot", 1 }, "pedal", false));
    layout.add(make_unique<juce::AudioParameterInt>(ParameterID { "wavetableResolution", 1 }, "wavetable res", 1, 6, 5));
    layout.add(make_unique<juce::AudioParameterBool>(ParameterID { "toggleNoteMap", 1 }, "toggle nm", true));
    layout.add(make_unique<juce::AudioParameterInt>(ParameterID { "superimpose", 1 }, "superimpose", 1, 12, 12));
    layout.add(make_unique<juce::AudioParameterFloat>(ParameterID { "mix", 1 }, "mix", 0, 1, 0.5f));
    layout.add(make_unique<juce::AudioParameterInt>(ParameterID { "numVoices", 1 }, "num voices", 1, 11, 1));
    
    Mixer::initializeValues(layout);
    
    return layout;
}

AudioProcessorValueTreeState& HTIntervalEngineAudioProcessor::getPluginState()
{
    return pluginState;
}

PluginParameters& HTIntervalEngineAudioProcessor::getParameters()
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

OwnedArray<SoundProfile>& HTIntervalEngineAudioProcessor::getSoundProfiles()
{
    return soundProfiles;
}

SoundProfile* HTIntervalEngineAudioProcessor::getSelectedSoundProfile()
{
    return selectedSoundProfile;
}

SoundProfile* HTIntervalEngineAudioProcessor::setSelectedSoundProfile(int profileId)
{
    for (int i = 0; i < soundProfiles.size(); i++)
    {
        auto p = soundProfiles[i];
        
        if (p && p->profileId == profileId)
        {
            if (selectedSoundProfile)
            {
                selectedSoundProfile->disable();
            }
            
            p->enable();
            
            selectedSoundProfile = p;
            
            return p;
        }
    }
    
    jassertfalse;
    
    return nullptr;
}

MidiKeyboardState& HTIntervalEngineAudioProcessor::getKeyboardState()
{
    return keyboardState;
}

void HTIntervalEngineAudioProcessor::handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity)
{
    if (!isAddingFromMidiInput)
    {
        auto on = MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
        on.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
        
        generatedEvents.addEvent(on, 0);
    }
}

void HTIntervalEngineAudioProcessor::handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity)
{
    if (!isAddingFromMidiInput)
    {
        auto off = MidiMessage::noteOff(midiChannel, midiNoteNumber);
        off.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
        
        generatedEvents.addEvent(off, 0);
    }
}

void HTIntervalEngineAudioProcessor::handleIncomingMidiMessage(MidiInput* input, const MidiMessage& message)
{
    const ScopedValueSetter<bool> scopedInputFlag(isAddingFromMidiInput, true);
    
    keyboardState.processNextMidiEvent(message);
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
    MidiBuffer filteredMessages;
    
    auto pedalNote = synthEngine.getPedalNote();
    
    if (*parameters.pedalRootParameter == 0 && pedalNote > -1)
    {
        auto off = MidiMessage::noteOff(synthEngine.getPedalChannel(), pedalNote);
        
        filteredMessages.addEvent(off, 0);
        
        synthEngine.setPedalNote(-1);
        pedalEnabled = false;
    }
    
    midiMessages.addEvents(generatedEvents, 0, generatedEvents.getNumEvents(), 0);

    generatedEvents.clear();
    
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
                
                if (!pitchMapper.currentIntervalMap.isEmpty() && currentNoteNumber != lastRootNote)
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
                    
                        if (pitchMapper.getSelectedNoteMapIndex() != pitchMapper.setNoteMap(lastInterval, true))
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
                    if (!*parameters.toggleNoteMapParameter)
                    {
                        pitchMapper.setNoteMap(0, true);
                    }
                    
                    pitchMapper.setCurrentRootInterval(keyInterval);
                    lastRootNote = currentNoteNumber;
                }
                
                if (*parameters.pedalRootParameter == 1 && pedalFlag && currentNoteNumber != pedalNote)
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

            auto value = pitchMapper.currentIntervalMap.getLastKnownFilePath(&pluginState).getValue();
            
            if (!value.isUndefined())
            {
                File file(value);
                
                pitchMapper.loadIntervalMap(&file);
            }
            
            auto map = pitchMapper.currentIntervalMap;
            
            if (!map.isEmpty())
            {
                HTIntervalEngineAudioProcessorEditor::fileTextOutput = "[" + map.baseMap.name + "]";
            }
            else
            {
                HTIntervalEngineAudioProcessorEditor::fileTextOutput = HTIntervalEngineAudioProcessorEditor::FILE_TEXT_BUFFER_EMPTY;
            }
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HTIntervalEngineAudioProcessor();
}

