#include "PluginProcessor.h"
#include "PluginEditor.h"

ChoirAudioProcessor::ChoirAudioProcessor()
    : AudioProcessor (BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                       .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
    voiceOffsets.resize(12, 0.0f);
}

ChoirAudioProcessor::~ChoirAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout ChoirAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>("pitch", "Pitch Offset", 1.0f, 12.0f, 5.0f));
    params.push_back(std::make_unique<juce::AudioParameterInt>("voices", "Voices", 1, 12, 4));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("delay", "Max Delay", 0.0f, 1000.0f, 50.0f));
    return { params.begin(), params.end() };
}

void ChoirAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // 2 seconds of buffer is plenty for 1000ms delay + pitch shifts
    int bufferSize = static_cast<int>(sampleRate * 2.0);
    delayBuffer.setSize(2, bufferSize);
    delayBuffer.clear();
    writePointer = 0;
}

void ChoirAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    float pitchOffset = apvts.getRawParameterValue("pitch")->load();
    int numVoices = (int)apvts.getRawParameterValue("voices")->load();
    float maxDelayMs = apvts.getRawParameterValue("delay")->load();

    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    int delayLength = delayBuffer.getNumSamples();
    float sampleRate = (float)getSampleRate();

    // 1. Calculate Pitch Array (Same logic as your script)
    std::vector<float> pitchShifts;
    float start = (numVoices % 2 == 0) ? -((numVoices / 2.0f) - 1.0f) * pitchOffset : -((numVoices - 1) / 2.0f) * pitchOffset;
    for (int v = 0; v < numVoices; ++v) {
        pitchShifts.push_back(start + (v * pitchOffset));
    }

    // 2. Main Processing Loop
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            float inputSample = buffer.getSample(channel, sample);
            delayBuffer.setSample(channel, writePointer, inputSample);

            float mixedOutput = 0.0f;
            
            for (int v = 0; v < numVoices; ++v)
            {
                // In real-time, pitch shifting via delay lines requires a moving read pointer.
                // For a simple "choir" feel, we use your shift logic as a static offset 
                // combined with a small random variation.
                float shiftSamples = (pitchShifts[v] * sampleRate) / 1200.0f;
                float randomDelaySamples = (maxDelayMs / 1000.0f) * sampleRate * (v / (float)numVoices);
                
                float readPos = (float)writePointer - (shiftSamples + randomDelaySamples);
                
                // Wrap-around circular buffer
                while (readPos < 0) readPos += (float)delayLength;
                while (readPos >= delayLength) readPos -= (float)delayLength;

                // Linear Interpolation (Same as your s1 + frac * (s2-s1))
                int idx1 = (int)readPos;
                int idx2 = (idx1 + 1) % delayLength;
                float frac = readPos - (float)idx1;

                float s1 = delayBuffer.getSample(channel, idx1);
                float s2 = delayBuffer.getSample(channel, idx2);
                
                // Pan logic from your script: i % 2
                if ((v % 2 == channel) || totalNumInputChannels == 1) {
                    mixedOutput += (s1 + frac * (s2 - s1));
                }
            }

            // Normalization logic
            float gainDivisor = std::max(1.0f, std::floor(numVoices / 2.0f));
            buffer.setSample(channel, sample, (mixedOutput / gainDivisor) * 0.9f);
        }

        writePointer++;
        if (writePointer >= delayLength) writePointer = 0;
    }
}

// Keep your existing state methods...
void ChoirAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void ChoirAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr) apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

void ChoirAudioProcessor::releaseResources() {}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new ChoirAudioProcessor(); }
juce::AudioProcessorEditor* ChoirAudioProcessor::createEditor() { return new ChoirAudioProcessorEditor (*this); }