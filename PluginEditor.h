#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class ChoirAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                   public juce::MenuBarModel
{
public:
    ChoirAudioProcessorEditor (ChoirAudioProcessor&);
    ~ChoirAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    // MenuBarModel overrides
    juce::StringArray getMenuBarNames() override;
    juce::PopupMenu getMenuForIndex (int menuIndex, const juce::String& menuName) override;
    void menuItemSelected (int menuItemID, int topLevelMenuIndex) override;

private:
    void setupSlider(juce::Slider& s, juce::Label& l, juce::String name);
    void showHelpWindow(); // Reusable help logic

    ChoirAudioProcessor& audioProcessor;

    juce::MenuBarComponent menuBar; 
    juce::Slider pitchSlider, voicesSlider, delaySlider;
    juce::Label pitchLabel, voicesLabel, delayLabel;
    juce::TextButton helpButton { "?" };

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> pitchAttachment, voicesAttachment, delayAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChoirAudioProcessorEditor)
};