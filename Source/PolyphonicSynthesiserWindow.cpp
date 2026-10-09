#include "PolyphonicSynthesiserWindow.h"

using namespace juce;
using namespace std;

PolyphonicSynthesiserWindow::PolyphonicSynthesiserWindow(PluginParameters& p, AudioProcessorEditor& parent, PolyphonicSynthesiser& ps) : Window(parent), parameters(p), profile(ps), numModules(6)
{
    const auto saw = ImageCache::getFromMemory(BinaryData::saw_png, BinaryData::saw_pngSize);
    const auto triangle = ImageCache::getFromMemory(BinaryData::triangle_png, BinaryData::triangle_pngSize);
    const auto square = ImageCache::getFromMemory(BinaryData::square_png, BinaryData::square_pngSize);
    const auto square1 = ImageCache::getFromMemory(BinaryData::square1_png, BinaryData::square1_pngSize);
    const auto square2 = ImageCache::getFromMemory(BinaryData::square2_png, BinaryData::square2_pngSize);
    
    sawImage.setImage(saw);
    triangleImage.setImage(triangle);
    squareImage.setImage(square);
    square1Image.setImage(square1);
    square2Image.setImage(square2);
    
    double min = 0.0;
    double max = 1.0;
    double interval = 0.05;
    
    sawSlider.setLookAndFeel(&lookAndFeel);
    sawSlider.setSliderStyle(Slider::LinearVertical);
    sawSlider.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
    sawSlider.setPopupDisplayEnabled(true, false, &parent);
    sawSlider.setRange(min, max, interval);
    
    triangleSlider.setLookAndFeel(&lookAndFeel);
    triangleSlider.setSliderStyle(Slider::LinearVertical);
    triangleSlider.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
    triangleSlider.setPopupDisplayEnabled(true, false, &parent);
    triangleSlider.setRange(min, max, interval);

    squareSlider.setLookAndFeel(&lookAndFeel);
    squareSlider.setSliderStyle(Slider::LinearVertical);
    squareSlider.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
    squareSlider.setPopupDisplayEnabled(true, false, &parent);
    squareSlider.setRange(min, max, interval);
    
    square1Slider.setLookAndFeel(&lookAndFeel);
    square1Slider.setSliderStyle(Slider::LinearVertical);
    square1Slider.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
    square1Slider.setPopupDisplayEnabled(true, false, &parent);
    square1Slider.setRange(min, max, interval);
    
    square2Slider.setLookAndFeel(&lookAndFeel);
    square2Slider.setSliderStyle(Slider::LinearVertical);
    square2Slider.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
    square2Slider.setPopupDisplayEnabled(true, false, &parent);
    square2Slider.setRange(min, max, interval);
    
    auto state = parameters.getPluginState();
    sawAttachment.reset(new SliderAttachment(*state, "x1", sawSlider));
    triangleAttachment.reset(new SliderAttachment(*state, "x2", triangleSlider));
    squareAttachment.reset(new SliderAttachment(*state, "x3", squareSlider));
    square1Attachment.reset(new SliderAttachment(*state, "x4", square1Slider));
    square2Attachment.reset(new SliderAttachment(*state, "x5", square2Slider));
    
    sawSlider.onValueChange = [this] { profile.update(); };
    triangleSlider.onValueChange = [this] { profile.update(); };
    squareSlider.onValueChange = [this] { profile.update(); };
    square1Slider.onValueChange = [this] { profile.update(); };
    square2Slider.onValueChange = [this] { profile.update(); };

    parent.addAndMakeVisible(&triangleImage);
    parent.addAndMakeVisible(&sawImage);
    parent.addAndMakeVisible(&squareImage);
    parent.addAndMakeVisible(&square1Image);
    parent.addAndMakeVisible(&square2Image);
    
    parent.addAndMakeVisible(&triangleSlider);
    parent.addAndMakeVisible(&sawSlider);
    parent.addAndMakeVisible(&squareSlider);
    parent.addAndMakeVisible(&square1Slider);
    parent.addAndMakeVisible(&square2Slider);
    
    modules = vector<Rectangle<int>>(numModules);
}

PolyphonicSynthesiserWindow::~PolyphonicSynthesiserWindow()
{
    modules.clear();
}

void PolyphonicSynthesiserWindow::paint(juce::Graphics& g)
{
    auto borderColour = Colour(250, 149, 115);
    
    g.setColour(borderColour);
    
    for (auto bounds : modules)
    {
        if (!bounds.isEmpty() && bounds.isFinite())
        {
            g.drawRoundedRectangle(bounds.toFloat(), 2, 1.5);
        }
    }
}

void PolyphonicSynthesiserWindow::resized(Rectangle<int> bounds)
{
    jassert(modules.size() == numModules);

    float ratio = bounds.getWidth() / bounds.getHeight();
    float reduction = 0.025;
    
    bounds.reduce(bounds.getWidth() * reduction, bounds.getHeight() * reduction * ratio);
    
    auto width = bounds.getWidth();
    auto height = bounds.getHeight();
    
    float rh = height / 2;
    float r1 = width / 3;
    float r2 = r1;
    
    float x1r1 = 1.5;
    float x2r1 = 1.0;
    float x3r1 = 1.0;
    
    float x1r2 = 0.75;
    float x2r2 = 1.0;
    float x3r2 = 1.0;
    
    float y1 = 1.6;
    float y2 = 1;
    
    float yn = 2 / (y1 + y2);
    float r1n = 3 / (x1r1 + x2r1 + x3r1);
    float r2n = 3 / (x1r2 + x2r2 + x3r2);
    
    x1r1 *= r1n;
    x2r1 *= r1n;
    x3r1 *= r1n;
    x1r2 *= r2n;
    x2r2 *= r2n;
    x3r2 *= r2n;
    y1 *= yn;
    y2 *= yn;
    
    auto r1b = bounds.removeFromTop(rh * y1).translated(0, -2);
    auto b1r1 = r1b.removeFromLeft(r1 * x1r1).translated(-4, 0); // oscillators
    auto b2r1 = r1b.removeFromLeft(r1 * x2r1).translated(0, 0); // filters
    auto b3r1 = r1b.removeFromLeft(r1 * x3r1).translated(4, 0); // effects
    
    auto r2b = bounds.removeFromTop(rh * y2).translated(0, 2);
    auto b1r2 = r2b.removeFromLeft(r2 * x1r2).translated(-4, 0); // lfo / vibrato
    auto b2r2 = r2b.removeFromLeft(r2 * x2r2).translated(0, 0); // filter envelope
    auto b3r2 = r2b.removeFromLeft(r2 * x3r2).translated(4, 0); // amp envelope
    
    modules.at(0) = b1r1;
    modules.at(1) = b2r1;
    modules.at(2) = b3r1;
    modules.at(3) = b1r2;
    modules.at(4) = b2r2;
    modules.at(5) = b3r2;
    
    bounds = b1r1;

    bounds.reduce(bounds.getWidth() * 0.05, bounds.getHeight() * 0.1);
    
    width = bounds.getWidth();
    height = bounds.getHeight();
    
    auto n = 5;
    auto spacing = 5;
    
    int size = (width * 0.7 - spacing * (n - 1)) / n;
    
    float heightReduction = 0.06;
    float widthReduction = 0.2;
    
    auto buffer = bounds.removeFromLeft(size);
    
    buffer.reduce(0, height * heightReduction);
    sawImage.setBounds(buffer.removeFromBottom(size));
    buffer.reduce(size * widthReduction, 0);
    sawSlider.setBounds(buffer);
    
    bounds.removeFromLeft(spacing);
    
    buffer = bounds.removeFromLeft(size);
    buffer.reduce(0, height * heightReduction);
    triangleImage.setBounds(buffer.removeFromBottom(size));
    buffer.reduce(size * widthReduction, 0);
    triangleSlider.setBounds(buffer);
    
    bounds.removeFromLeft(spacing);
    
    buffer = bounds.removeFromLeft(size);
    buffer.reduce(0, height * heightReduction);
    squareImage.setBounds(buffer.removeFromBottom(size));
    buffer.reduce(size * widthReduction, 0);
    squareSlider.setBounds(buffer);
    
    bounds.removeFromLeft(spacing);
    
    buffer = bounds.removeFromLeft(size);
    buffer.reduce(0, height * heightReduction);
    square1Image.setBounds(buffer.removeFromBottom(size));
    buffer.reduce(size * widthReduction, 0);
    square1Slider.setBounds(buffer);
    
    bounds.removeFromLeft(spacing);
    
    buffer = bounds.removeFromLeft(size);
    buffer.reduce(0, height * heightReduction);
    square2Image.setBounds(buffer.removeFromBottom(size));
    buffer.reduce(size * widthReduction, 0);
    square2Slider.setBounds(buffer);
}
