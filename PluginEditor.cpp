#include "PluginProcessor.h"
#include "PluginEditor.h"

ChoirAudioProcessorEditor::ChoirAudioProcessorEditor (ChoirAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Initialize Sliders
    setupSlider(pitchSlider, pitchLabel, "Pitch Offset");
    setupSlider(voicesSlider, voicesLabel, "Voices");
    setupSlider(delaySlider, delayLabel, "Max Delay (ms)");

    // Attach to APVTS
    pitchAttachment = std::make_unique<Attachment>(audioProcessor.getAPVTS(), "pitch", pitchSlider);
    voicesAttachment = std::make_unique<Attachment>(audioProcessor.getAPVTS(), "voices", voicesSlider);
    delayAttachment = std::make_unique<Attachment>(audioProcessor.getAPVTS(), "delay", delaySlider);

    // Help Button Setup
    addAndMakeVisible(helpButton);
    helpButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey.withAlpha(0.2f));
    
    helpButton.onClick = [this] {
        juce::String helpText = 
            "Choirboy12 is a multi-voice thickening effect that transforms a single input into a lush vocal ensemble.\n\n"
            "PARAMETERS:\n"
            "* Voices: Sets the total number of generated voices.\n"
            "* Offset: Adjusts the pitch interval (semitones) between voices.\n"
            "* Delay: Adds timing offsets for a natural, 'loose' ensemble feel.\n"
            "* Stereo: Controls the width across the L/R channels.\n"
            "* Overload: Adjusts internal drive and saturation.\n\n"
            "Copyright (C) 2025. All Rights Reserved.";

        juce::AlertWindow::showMessageBoxAsync (
            juce::MessageBoxIconType::InfoIcon,
            "About Choirboy12",
            helpText,
            "Close"
        );
    };

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
    g.setFont (18.0f);
    
    // Header Area
    auto header = getLocalBounds().removeFromTop(40);
    g.drawFittedText ("Choirboy12", header, juce::Justification::centred, 1);
    
    // Bottom border for header
    g.setColour(juce::Colours::white.withAlpha(0.2f));
    g.drawLine(10, 40, getWidth() - 10, 40);
}

void ChoirAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);
    
    // Position Help Button top right
    helpButton.setBounds(getWidth() - 35, 10, 25, 25);

    area.removeFromTop(40); // Clear space for the header
    
    int width = area.getWidth() / 3;
    
    // Layout logic for the three main sliders
    auto pitchArea = area.removeFromLeft(width);
    pitchLabel.setBounds(pitchArea.removeFromTop(20));
    pitchSlider.setBounds(pitchArea.reduced(5));

    auto voicesArea = area.removeFromLeft(width);
    voicesLabel.setBounds(voicesArea.removeFromTop(20));
    voicesSlider.setBounds(voicesArea.reduced(5));

    auto delayArea = area;
    delayLabel.setBounds(delayArea.removeFromTop(20));
    delaySlider.setBounds(delayArea.reduced(5));
}