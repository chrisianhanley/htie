#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Map.h"
#include "CustomFont.h"

#include "PolyphonicSynthesiserWindow.h"
#include "ElectricPianoWindow.h"

using namespace juce;

//==============================================================================

const String HTIntervalEngineAudioProcessorEditor::FILE_TEXT_BUFFER_EMPTY = "[empty]";

String HTIntervalEngineAudioProcessorEditor::fileTextAppend = "";
String HTIntervalEngineAudioProcessorEditor::fileTextBuffer = "";
String HTIntervalEngineAudioProcessorEditor::fileTextOutput = FILE_TEXT_BUFFER_EMPTY;

HTIntervalEngineAudioProcessorEditor::HTIntervalEngineAudioProcessorEditor(Processor& p)
: AudioProcessorEditor(&p), processor(p), keyboardComponent(processor.getKeyboardState(), MidiKeyboardComponent::horizontalKeyboard)
{
    auto& pluginState = processor.getPluginState();
    
    LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName(CustomFont::TYPEFACE_NAME);
    
    keyboardComponent.setOctaveForMiddleC(processor.getPitchMapper().OCTAVE_FOR_MIDDLE_C);
    keyboardComponent.setLowestVisibleKey(36); // C2
    
    // volume slider
    volumeSlider.setLookAndFeel(&lookAndFeel);
    volumeSlider.setSliderStyle(Slider::LinearHorizontal);
    volumeSlider.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
    volumeSlider.setPopupDisplayEnabled(true, false, this);
    volumeSlider.setRange(-100, -12, 0.1);
    volumeSlider.setSkewFactor(5);
    volumeSlider.setValue(Decibels::gainToDecibels(processor.getParameters().gainValue.load()));
    volumeSlider.onValueChange = [this]
    {
        auto gain = Decibels::decibelsToGain((double) volumeSlider.getValue());
        
        processor.getParameters().gainValue = gain;
    };
    
    // key center
    keyCenterLabel.setLookAndFeel(&lookAndFeel);
    keyCenterSelection.setLookAndFeel(&lookAndFeel);
    keyCenterLabel.setFont(CustomFont::REGULAR);
    keyCenterLabel.setText("key: ", dontSendNotification);
    keyCenterSelection.addItem("C", 1);
    keyCenterSelection.addItem("C#", 2);
    keyCenterSelection.addItem("D", 3);
    keyCenterSelection.addItem("D#", 4);
    keyCenterSelection.addItem("E", 5);
    keyCenterSelection.addItem("F", 6);
    keyCenterSelection.addItem("F#", 7);
    keyCenterSelection.addItem("G", 8);
    keyCenterSelection.addItem("G#", 9);
    keyCenterSelection.addItem("A", 10);
    keyCenterSelection.addItem("A#", 11);
    keyCenterSelection.addItem("B", 12);
    //keyCenterSelection.onChange = [this] { updateCurrentRootIntervalText(); };
    keyCenterAttachment.reset(new ComboBoxAttachment(pluginState, "keyCenter", keyCenterSelection));
    
    // root input range
    rootInputRangeLabel.setLookAndFeel(&lookAndFeel);
    rootInputRangeSelection.setLookAndFeel(&lookAndFeel);
    rootInputRangeLabel.setFont(CustomFont::REGULAR);
    rootInputRangeLabel.setText("input range: ", dontSendNotification);
    rootInputRangeSelection.addItem("C-2 - C-1", 1);
    rootInputRangeSelection.addItem("C-1 - C0", 2);
    rootInputRangeSelection.addItem("C0 - C1", 3);
    rootInputRangeSelection.addItem("C1 - C2", 4);
    rootInputRangeSelection.addItem("C2 - C3", 5);
    rootInputRangeSelection.addItem("C3 - C4", 6);
    rootInputRangeSelection.addItem("C4 - C5", 7);
    rootInputRangeSelection.addItem("C5 - C6", 8);
    rootInputRangeSelection.addItem("C6 - C7", 9);
    rootInputRangeSelection.addItem("C7 - C8", 10);
    rootInputRangeAttachment.reset(new ComboBoxAttachment(pluginState, "rootInputRange", rootInputRangeSelection));
    
    // quantize root
    quantizeRootLabel.setLookAndFeel(&lookAndFeel);
    quantizeRootToggle.setLookAndFeel(&lookAndFeel);
    quantizeRootLabel.setFont(CustomFont::REGULAR);
    quantizeRootLabel.setText("quantize: ", dontSendNotification);
    quantizeRootAttachment.reset(new ButtonAttachment(pluginState, "quantizeRoot", quantizeRootToggle));
    
    // pedal root
    pedalRootLabel.setLookAndFeel(&lookAndFeel);
    pedalRootToggle.setLookAndFeel(&lookAndFeel);
    pedalRootLabel.setFont(CustomFont::REGULAR);
    pedalRootLabel.setText("pedal: ", NotificationType::dontSendNotification);
    pedalRootAttachment.reset(new ButtonAttachment(pluginState, "pedalRoot", pedalRootToggle));
    
    // current root
    currentRootIntervalLabel.setLookAndFeel(&lookAndFeel);
    currentRootIntervalLabel.setFont(CustomFont::ITALIC);
    currentRootIntervalLabel.setJustificationType(Justification::right);
    //processor.pitchMapper.onRootIntervalChangeAsync = [this] { updateCurrentRootIntervalText(); };

    // interval map chooser
    intervalMapButton.setLookAndFeel(&lookAndFeel);
    intervalMapButton.setButtonText("<load interval map>");
    fileLoadLabel.setLookAndFeel(&lookAndFeel);
    fileLoadLabel.setFont(CustomFont::ITALIC);
    fileLoadLabel.setText(fileTextOutput, dontSendNotification);
    
    intervalMapChooser = std::make_unique<FileChooser>("Select interval map to load...", File::getSpecialLocation(File::userHomeDirectory), "*.json");
    
    startTimer(50); // since i have to keep GUI updates on the main thread
    
    intervalMapButton.onClick = [this]
    {
        auto load = [this] (File file, bool reload = false)
        {
            int result = processor.getPitchMapper().loadIntervalMap(&file);
            
            string append = "";
            
            switch (result) {
                case -1:
                    append = "(file not found)";
                    
                    break;
                    
                case -2:
                    append = "(parsing error)";
                    
                    break;
                    
                default:
                    break;
            }
            
            String name;
            
            auto last = processor.getPitchMapper().currentIntervalMap;
            
            if (!last.isEmpty())
            {
                fileTextBuffer = "[" + last.baseMap.name + "]";
            }
            else
            {
                fileTextBuffer = FILE_TEXT_BUFFER_EMPTY;
            }
            
            if (result == 0)
            {
                fileTextOutput = reload ? "successfully reloaded " + fileTextBuffer : "successfully loaded " + fileTextBuffer;
            }
            else
            {
                fileTextOutput = "file load unsuccessful " + append;
                
                if (reload)
                {
                    last.resetLastKnownFilePath(&processor.getPluginState());
                    
                    fileTextBuffer = FILE_TEXT_BUFFER_EMPTY;
                }
            }
            
            callAfterDelay(2500, [this, last]
            {
                auto current = processor.getPitchMapper().currentIntervalMap;
                
                if (!current.isEmpty())
                {
                    int index = processor.getPitchMapper().getSelectedNoteMapIndex();
                    auto nms = current.noteMaps;
                    
                    if (nms.find(index) != nms.end())
                    {
                        auto map = nms[index];
                        auto name = map.name;
                        
                        fileTextAppend = " -> " + name + " [" + to_string(index) + "] ";
                    }
                    else
                    {
                        fileTextAppend = "";
                    }
                }
                else
                {
                    fileTextAppend = "";
                }
                
                if (last == current)
                {
                    fileTextOutput = fileTextBuffer + fileTextAppend;
                }
            });
        };
        
        auto path = processor.getPitchMapper().currentIntervalMap.getLastKnownFilePath(&processor.getPluginState());
        
        if (intervalMapButton.getButtonText() == "<reload>" && !path.getValue().isUndefined())
        {
            load(File(path.getValue()), true);
        }
        else
        {
            auto folderChooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectDirectories | FileBrowserComponent::canSelectFiles;
            
            intervalMapChooser->launchAsync(folderChooserFlags, [this, load] (const FileChooser& chooser)
            {
                File file(chooser.getResult());
                
                load(file);
            });
        }
    };
    
    processor.getPitchMapper().onMapChangeAsync = [this]
    {
        auto index = processor.getPitchMapper().getSelectedNoteMapIndex();
        auto im = processor.getPitchMapper().currentIntervalMap;
        
        if (!im.isEmpty())
        {
            auto nms = im.noteMaps;
            if (nms.find(index) != nms.end())
            {
                auto map = nms[index];
                auto name = map.name;
                
                fileTextAppend = " -> " + name + " [" + to_string(index) + "] ";
            }
            else
            {
                fileTextAppend = "";
            }
            
            fileTextOutput = fileTextBuffer + fileTextAppend;
            
            return;
        }
        
        fileTextAppend = "";
        fileTextOutput = fileTextBuffer;
    };
    
    // settings gui
    settingsButton.setLookAndFeel(&lookAndFeel);
    settingsButton.setButtonText("<settings>");
    settingsButton.onClick = [this]
    {
        if (dynamic_cast<SettingsWindow*>(currentWindow.get()))
        {
            auto id = soundSelection.getSelectedId();
            auto profile = processor.setSelectedSoundProfile(id);
            
            settingsButton.setButtonText("<settings>");
            
            setCurrentWindow(profile->createWindow(*this));
        }
        else
        {
            settingsButton.setButtonText("<close>");
            
            setCurrentWindow(new SettingsWindow(processor, *this));
        }
    };
    
    // sound selection
    auto& profiles = processor.getSoundProfiles();
    
    for (int i = 0; i < profiles.size(); i++)
    {
        auto p = profiles[i];
        
        soundSelection.addItem(p->getDisplayName(), p->profileId);
    }
    
    soundSelection.setLookAndFeel(&lookAndFeel);

    if (processor.getSelectedSoundProfile() && !currentWindow)
    {
        auto profile = processor.getSelectedSoundProfile();
        
        soundSelection.setSelectedId(profile->profileId);
        
        setCurrentWindow(profile->createWindow(*this));
    }
    
    soundSelection.onChange = [this]
    {
        auto id = soundSelection.getSelectedId();
        auto profile = processor.setSelectedSoundProfile(id);

        setCurrentWindow(profile->createWindow(*this));
    };
    
    superimposeLabel.setLookAndFeel(&lookAndFeel);
    superimposeLabel.setText("superimpose: ", dontSendNotification);
    
    superimposeSelection.setLookAndFeel(&lookAndFeel);
    superimposeSelection.addItem("1", 1);
    superimposeSelection.addItem("2", 2);
    superimposeSelection.addItem("3", 3);
    superimposeSelection.addItem("4", 4);
    superimposeSelection.addItem("5", 5);
    superimposeSelection.addItem("6", 6);
    superimposeSelection.addItem("7", 7);
    superimposeSelection.addItem("8", 8);
    superimposeSelection.addItem("9", 9);
    superimposeSelection.addItem("10", 10);
    superimposeSelection.addItem("11", 11);
    superimposeSelection.addItem("off", 12);
    superimposeSelection.setSelectedId(12);
    superimposeAttachment.reset(new ComboBoxAttachment(pluginState, "superimpose", superimposeSelection));
    
    mixLabel.setLookAndFeel(&lookAndFeel);
    mixLabel.setText("mix: ", dontSendNotification);
    
    mixSlider.setLookAndFeel(&lookAndFeel);
    mixSlider.setLookAndFeel(&lookAndFeel);
    mixSlider.setSliderStyle(Slider::LinearHorizontal);
    mixSlider.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
    mixSlider.setPopupDisplayEnabled(true, false, this);
    mixSlider.setRange(0, 1, 0.05f);
    mixSlider.setValue(0.5f);
    mixAttachment.reset(new SliderAttachment(pluginState, "mix", mixSlider));
    
    numVoicesLabel.setLookAndFeel(&lookAndFeel);
    numVoicesLabel.setText("num voices: ", dontSendNotification);
    
    numVoicesSelection.setLookAndFeel(&lookAndFeel);
    numVoicesSelection.addItem("1", 1);
    numVoicesSelection.addItem("2", 2);
    numVoicesSelection.addItem("3", 3);
    numVoicesSelection.addItem("4", 4);
    numVoicesSelection.addItem("5", 5);
    numVoicesSelection.addItem("6", 6);
    numVoicesSelection.addItem("7", 7);
    numVoicesSelection.addItem("8", 8);
    numVoicesSelection.addItem("9", 9);
    numVoicesSelection.addItem("10", 10);
    numVoicesSelection.addItem("11", 11);
    numVoicesSelection.setSelectedId(1);
    numVoicesAttachment.reset(new ComboBoxAttachment(pluginState, "numVoices", numVoicesSelection));
    
    // make visible
    addAndMakeVisible(&volumeSlider);
    addAndMakeVisible(&keyboardComponent);
    addAndMakeVisible(&keyCenterLabel);
    addAndMakeVisible(&keyCenterSelection);
    addAndMakeVisible(&rootInputRangeLabel);
    addAndMakeVisible(&rootInputRangeSelection);
    addAndMakeVisible(&quantizeRootLabel);
    addAndMakeVisible(&quantizeRootToggle);
    addAndMakeVisible(&pedalRootLabel);
    addAndMakeVisible(&pedalRootToggle);
    addAndMakeVisible(&currentRootIntervalLabel);
    addAndMakeVisible(&intervalMapButton);
    addAndMakeVisible(&fileLoadLabel);
    addAndMakeVisible(&settingsButton);
    addAndMakeVisible(&soundSelection);
    addAndMakeVisible(&superimposeLabel);
    addAndMakeVisible(&superimposeSelection);
    addAndMakeVisible(&mixLabel);
    addAndMakeVisible(&mixSlider);
    addAndMakeVisible(&numVoicesLabel);
    addAndMakeVisible(&numVoicesSelection);
    
    updateCurrentRootIntervalText();
    
    // create window
    const float ratio = 1.46f;
    const float size = 440;
    const float min = 440;
    const float max = 500;
    
    setSize(round(size * ratio), size);
    setResizable(true, true);
    setResizeLimits(min * ratio, min, max * ratio, max);
    
    getConstrainer()->setFixedAspectRatio(ratio);
}

HTIntervalEngineAudioProcessorEditor::~HTIntervalEngineAudioProcessorEditor()
{
    stopTimer();
}

void HTIntervalEngineAudioProcessorEditor::setCurrentWindow(Window* window)
{
    currentWindow = unique_ptr<Window>(window);
    
    resized();
    repaint();
}

void HTIntervalEngineAudioProcessorEditor::updateCurrentRootIntervalText()
{
    auto mapper = processor.getPitchMapper();
    auto root = mapper.getCurrentRootInterval();
    auto text = "root: " + mapper.getCurrentRootNote().toStdString() + (root > 0 ? " [" + to_string(root) + " semitones]" : " [key]");
    
    currentRootIntervalLabel.setText(text, dontSendNotification);
    currentRootIntervalLabel.repaint();
}

void HTIntervalEngineAudioProcessorEditor::timerCallback()
{
    updateCurrentRootIntervalText();
    
    if (fileLoadLabel.getText() != fileTextOutput)
    {
        fileLoadLabel.setText(fileTextOutput, dontSendNotification);
    }
    
    auto mapper = processor.getPitchMapper();
    
    if (ModifierKeys::getCurrentModifiers().isShiftDown() && !mapper.currentIntervalMap.isEmpty() &&  !mapper.currentIntervalMap.getLastKnownFilePath(&processor.getPluginState()).getValue().isUndefined())
    {
        intervalMapButton.setButtonText("<reload>");
    }
    else if (intervalMapButton.getButtonText() != "<load interval map>")
    {
        intervalMapButton.setButtonText("<load interval map>");
    }
    
    auto bounds = getLocalBounds();
    
    calculateDynamicComponentBounds(bounds);
}

//==============================================================================

const Colour BACKGROUND_COLOUR = Colour(251, 142, 106);
const Colour WINDOW_COLOUR = Colour(251, 138, 101);

void HTIntervalEngineAudioProcessorEditor::paint(Graphics& g)
{
    g.fillAll(BACKGROUND_COLOUR);
    
    if (!boundsToFill.isEmpty() && boundsToFill.isFinite())
    {
        g.setColour(WINDOW_COLOUR);
        g.fillRect(boundsToFill);
        
        g.setColour(Colours::brown.withAlpha((uint8) 6));
        
        if (currentWindow)
        {
            currentWindow->paint(g);
        }
    }
}

void HTIntervalEngineAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    auto margin1 = CustomFont::DEFAULT_SIZE + 15;
    auto margin2 = margin1 * 0.7f;
    
    keyboardComponent.setBounds(bounds.removeFromBottom(70));
    
    auto buffer1 = bounds.removeFromBottom(margin1);
    auto buffer2 = bounds.removeFromBottom(margin2);
    
    buffer1.translate(3, -3);
    buffer2.translate(9, -3);
    
    keyCenterLabel.setBounds(buffer1.removeFromLeft(GlyphArrangement::getStringWidth(keyCenterLabel.getFont(), keyCenterLabel.getText()) + 10));
    keyCenterSelection.setBounds(buffer1.removeFromLeft(60));
    rootInputRangeLabel.setBounds(buffer1.removeFromLeft(GlyphArrangement::getStringWidth(rootInputRangeLabel.getFont(), rootInputRangeLabel.getText()) + 10));
    rootInputRangeSelection.setBounds(buffer1.removeFromLeft(95));
    quantizeRootLabel.setBounds(buffer1.removeFromLeft(GlyphArrangement::getStringWidth(quantizeRootLabel.getFont(), quantizeRootLabel.getText()) + 10));
    quantizeRootToggle.setBounds(buffer1.removeFromLeft(margin1));
    pedalRootLabel.setBounds(buffer1.removeFromLeft(GlyphArrangement::getStringWidth(pedalRootLabel.getFont(), pedalRootLabel.getText()) + 10));
    pedalRootToggle.setBounds(buffer1.removeFromLeft(margin1));
    
    superimposeLabel.setBounds(buffer2.removeFromLeft(GlyphArrangement::getStringWidth(superimposeLabel.getFont(), superimposeLabel.getText()) + 10));
    superimposeSelection.setBounds(buffer2.removeFromLeft(60));
    mixLabel.setBounds(buffer2.removeFromLeft(GlyphArrangement::getStringWidth(mixLabel.getFont(), mixLabel.getText()) + 10));
    mixSlider.setBounds(buffer2.removeFromLeft(90));
    numVoicesLabel.setBounds(buffer2.removeFromLeft(GlyphArrangement::getStringWidth(numVoicesLabel.getFont(), numVoicesLabel.getText()) + 10));
    numVoicesSelection.setBounds(buffer2.removeFromLeft(60));
    
    buffer1.translate(-6, 0);
    
    currentRootIntervalLabel.setBounds(buffer1.removeFromRight(GlyphArrangement::getStringWidth(currentRootIntervalLabel.getFont(), currentRootIntervalLabel.getText()) + 100));
    
    calculateDynamicComponentBounds(bounds);
    updateCurrentRootIntervalText();
    
    bounds.reduce(15, 7);
    
    boundsToFill = bounds;
    
    buffer1 = bounds.removeFromTop(margin1);
    buffer1.translate(0, 1);
    
    soundSelection.setBounds(buffer1.reduced(200, 0));
    
    buffer1.translate(-1, 0);
    
    settingsButton.setBounds(buffer1.removeFromRight(GlyphArrangement::getStringWidth(rootInputRangeLabel.getFont(), settingsButton.getButtonText()) + 20));
    
    if (currentWindow)
    {
        currentWindow->resized(bounds);
    }
}

void HTIntervalEngineAudioProcessorEditor::calculateDynamicComponentBounds(Rectangle<int>& bounds) {
    auto margin = CustomFont::DEFAULT_SIZE + 15;
    auto buffer = bounds.removeFromTop(margin);
    
    buffer.translate(5, 3);
    
    intervalMapButton.setBounds(buffer.removeFromLeft(GlyphArrangement::getStringWidth(CustomFont::REGULAR, intervalMapButton.getButtonText()) + 20));
    fileLoadLabel.setBounds(buffer.removeFromLeft(GlyphArrangement::getStringWidth(fileLoadLabel.getFont(), fileLoadLabel.getText()) + 20));
    
    buffer.translate(-7, 0);
    
    volumeSlider.setBounds(buffer.removeFromRight(75));
}
