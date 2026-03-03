#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class ChoirAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ChoirAudioProcessorEditor (ChoirAudioProcessor&);
    ~ChoirAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    /** * Helper to set up slider styles and colors in one go.
     * Defined in PluginEditor.cpp
     */
    void setupSlider(juce::Slider& s, juce::Label& l, juce::String name);

    ChoirAudioProcessor& audioProcessor;

    // UI Components
    juce::Slider pitchSlider, voicesSlider, delaySlider;
    juce::Label pitchLabel, voicesLabel, delayLabel;
    juce::TextButton helpButton { "?" };

    // APVTS Attachments
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    std::unique_ptr<Attachment> pitchAttachment;
    std::unique_ptr<Attachment> voicesAttachment;
    std::unique_ptr<Attachment> delayAttachment;

    // Note: If you add 'Stereo' and 'Overload' sliders later, 
    // you'll want to declare their attachments here as well.

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChoirAudioProcessorEditor)
};