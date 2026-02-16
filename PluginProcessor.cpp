#include "PluginProcessor.h"
#include "PluginEditor.h"

ChoirAudioProcessor::ChoirAudioProcessor()
    : AudioProcessor (BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                       .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{}

ChoirAudioProcessor::~ChoirAudioProcessor() {}

// This is the "Entry Point" JUCE needs to create your plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChoirAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout ChoirAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>("pitch", "Pitch Offset", 1.0f, 12.0f, 5.0f));
    params.push_back(std::make_unique<juce::AudioParameterInt>("voices", "Voices", 1, 12, 4));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("delay", "Max Delay", 0.0f, 1000.0f, 50.0f));
    return { params.begin(), params.end() };
}

void ChoirAudioProcessor::prepareToPlay (double sampleRate, int /*samplesPerBlock*/){
    int bufferSize = static_cast<int>(sampleRate * 2.0); 
    voiceBuffers.clear();
    for (int i = 0; i < 12; ++i) {
        juce::AudioBuffer<float> buf(2, bufferSize);
        buf.clear();
        voiceBuffers.push_back(std::move(buf));
    }
    writePointer = 0;
}

void ChoirAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/){
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Get current parameter values from the UI
    auto numVoices = apvts.getRawParameterValue("voices")->load();
    auto maxDelayMs = apvts.getRawParameterValue("delay")->load();

    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    int bufferLength = voiceBuffers[0].getNumSamples();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float inputSample = channelData[sample];
            float mixedOutput = 0.0f;

            for (int v = 0; v < numVoices; ++v) {
                voiceBuffers[v].setSample(channel, writePointer, inputSample);
                
                float voiceDelayOffset = (v * (maxDelayMs / 1000.0f) * (float)getSampleRate()) / numVoices;
                float readPos = (float)writePointer - voiceDelayOffset;
                if (readPos < 0) readPos += (float)bufferLength;

                int index1 = (int)readPos;
                int index2 = (index1 + 1) % bufferLength;
                float frac = readPos - (float)index1;

                float s1 = voiceBuffers[v].getSample(channel, index1);
                float s2 = voiceBuffers[v].getSample(channel, index2);
                mixedOutput += s1 + frac * (s2 - s1);
            }

            channelData[sample] = mixedOutput / (float)numVoices;
        }
    }
    writePointer = (writePointer + buffer.getNumSamples()) % bufferLength;
}

void ChoirAudioProcessor::releaseResources() {}
juce::AudioProcessorEditor* ChoirAudioProcessor::createEditor() { return new ChoirAudioProcessorEditor (*this); }
void ChoirAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}
void ChoirAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr) apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}