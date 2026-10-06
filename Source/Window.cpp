#include "Window.h"

using namespace juce;

Window::Window(AudioProcessorEditor& e) : parent(e) {}

Window::~Window() {}

void Window::paint(juce::Graphics& g) {}

void Window::resized(juce::Rectangle<int> bounds) {}
