#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class ChoirAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ChoirAudioProcessorEditor (ChoirAudioProcessor&);
    ~ChoirAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void setupSlider(juce::Slider& s, juce::Label& l, juce::String name);

    ChoirAudioProcessor& audioProcessor;

    juce::Slider pitchSlider, voicesSlider, delaySlider;
    juce::Label pitchLabel, voicesLabel, delayLabel;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> pitchAttachment, voicesAttachment, delayAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChoirAudioProcessorEditor)
};