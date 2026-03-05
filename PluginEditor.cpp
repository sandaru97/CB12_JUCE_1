#include "PluginProcessor.h"
#include "PluginEditor.h"

ChoirAudioProcessorEditor::ChoirAudioProcessorEditor (ChoirAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Setup Menu Bar
    addAndMakeVisible(menuBar);
    menuBar.setModel(this);
    menuBar.setColour(juce::PopupMenu::backgroundColourId, juce::Colours::transparentBlack);
    menuBar.setColour(juce::PopupMenu::headerTextColourId, juce::Colours::black);

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
    helpButton.setColour(juce::TextButton::buttonColourId, juce::Colours::black.withAlpha(0.05f));
    helpButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    
    // Use Courier for the help button specifically
    helpButton.setLookAndFeel(&juce::LookAndFeel::getDefaultLookAndFeel()); 

    helpButton.onClick = [this] { showHelpWindow(); };

    setSize (400, 300);
}

ChoirAudioProcessorEditor::~ChoirAudioProcessorEditor() 
{
    menuBar.setModel(nullptr);
}

// --- Reusable Help Function ---
void ChoirAudioProcessorEditor::showHelpWindow()
{
    juce::String helpText = "Choirboy12: Multi-voice vocal thickening ensemble.\n\nCopyright (C) 2026.";
    juce::AlertWindow::showMessageBoxAsync (juce::MessageBoxIconType::InfoIcon, "About", helpText, "Close");
}

// --- Menu Bar Logic ---
juce::StringArray ChoirAudioProcessorEditor::getMenuBarNames() 
{ 
    return { "Presets", "Help" }; // Added Help to top bar
}

juce::PopupMenu ChoirAudioProcessorEditor::getMenuForIndex (int menuIndex, const juce::String& menuName)
{
    juce::PopupMenu menu;
    if (menuName == "Presets")
    {
        menu.addItem (1, "TightDouble");
        menu.addItem (2, "LushQuadret");
        menu.addItem (3, "GrandEnsemble");
        menu.addItem (4, "Haunted");
        menu.addItem (5, "Microunison");
    }
    else if (menuName == "Help")
    {
        menu.addItem (100, "About Choirboy12");
    }
    return menu;
}

void ChoirAudioProcessorEditor::menuItemSelected (int menuItemID, int topLevelMenuIndex) 
{
    if (menuItemID == 100) // "About" clicked in Help menu
        showHelpWindow();
}

// --- UI Setup ---
void ChoirAudioProcessorEditor::setupSlider(juce::Slider& s, juce::Label& l, juce::String name) {
    addAndMakeVisible(s);
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::black);
    s.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::black.withAlpha(0.2f));
    s.setColour(juce::Slider::thumbColourId, juce::Colours::black);
    s.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    
    addAndMakeVisible(l);
    l.setText(name, juce::dontSendNotification);
    l.setFont(juce::Font("Courier", 14.0f, juce::Font::plain)); // Label font
    l.setJustificationType(juce::Justification::centred);
    l.setColour(juce::Label::textColourId, juce::Colours::black);
}

void ChoirAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGBA(255, 255, 240, 255)); // Ivory
    g.setColour (juce::Colours::black);
    
    // Set Courier for the Header
    g.setFont (juce::Font("Courier", 20.0f, juce::Font::bold));
    
    auto headerArea = getLocalBounds().removeFromTop(60).withTrimmedTop(25);
    g.drawFittedText ("Choirboy12", headerArea, juce::Justification::centred, 1);
    
    g.setColour(juce::Colours::black.withAlpha(0.1f));
    g.drawLine(10.0f, 60.0f, (float)getWidth() - 10.0f, 60.0f);
}

void ChoirAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    menuBar.setBounds(area.removeFromTop(25));
    helpButton.setBounds(getWidth() - 35, 30, 25, 25);
    area.removeFromTop(35);
    
    auto sliderArea = area.reduced(20);
    int width = sliderArea.getWidth() / 3;
    
    auto pitchArea = sliderArea.removeFromLeft(width);
    pitchLabel.setBounds(pitchArea.removeFromTop(20));
    pitchSlider.setBounds(pitchArea.reduced(5));

    auto voicesArea = sliderArea.removeFromLeft(width);
    voicesLabel.setBounds(voicesArea.removeFromTop(20));
    voicesSlider.setBounds(voicesArea.reduced(5));

    auto delayArea = sliderArea;
    delayLabel.setBounds(delayArea.removeFromTop(20));
    delaySlider.setBounds(delayArea.reduced(5));
}