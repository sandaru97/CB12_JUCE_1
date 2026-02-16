#include "PluginProcessor.h"
#include "PluginEditor.h"

ChoirAudioProcessorEditor::ChoirAudioProcessorEditor (ChoirAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setupSlider(pitchSlider, pitchLabel, "Pitch Offset");
    setupSlider(voicesSlider, voicesLabel, "Voices");
    setupSlider(delaySlider, delayLabel, "Max Delay (ms)");

    pitchAttachment = std::make_unique<Attachment>(audioProcessor.getAPVTS(), "pitch", pitchSlider);
    voicesAttachment = std::make_unique<Attachment>(audioProcessor.getAPVTS(), "voices", voicesSlider);
    delayAttachment = std::make_unique<Attachment>(audioProcessor.getAPVTS(), "delay", delaySlider);

    setSize (400, 300);
}

ChoirAudioProcessorEditor::~ChoirAudioProcessorEditor() {}

void ChoirAudioProcessorEditor::setupSlider(juce::Slider& s, juce::Label& l, juce::String name) {
    addAndMakeVisible(s);
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(l);
    l.setText(name, juce::dontSendNotification);
    l.setJustificationType(juce::Justification::centred);
}

void ChoirAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Choir Effect", getLocalBounds().removeFromTop(30), juce::Justification::centred, 1);
}

void ChoirAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);
    area.removeFromTop(30);
    int width = area.getWidth() / 3;
    
    auto pitchArea = area.removeFromLeft(width);
    pitchLabel.setBounds(pitchArea.removeFromTop(20));
    pitchSlider.setBounds(pitchArea);

    auto voicesArea = area.removeFromLeft(width);
    voicesLabel.setBounds(voicesArea.removeFromTop(20));
    voicesSlider.setBounds(voicesArea);

    auto delayArea = area;
    delayLabel.setBounds(delayArea.removeFromTop(20));
    delaySlider.setBounds(delayArea);
}