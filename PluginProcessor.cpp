#include "PluginProcessor.h"
#include "PluginEditor.h"

GainKnobAudioProcessor::GainKnobAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
    gainParam = apvts.getRawParameterValue (gainParamID);
}

juce::AudioProcessorValueTreeState::ParameterLayout GainKnobAudioProcessor::createParameterLayout()
{
    return {
        std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { gainParamID, 1 },
            "Gain",
            juce::NormalisableRange<float> (minGainDb, maxGainDb, 0.1f),
            0.0f,
            juce::AudioParameterFloatAttributes().withLabel ("dB"))
    };
}

void GainKnobAudioProcessor::prepareToPlay (double sampleRate, int /*samplesPerBlock*/)
{
    // 20 ms ramp so turning the knob doesn't cause zipper noise / clicks
    smoothedGain.reset (sampleRate, 0.02);
    smoothedGain.setCurrentAndTargetValue (dbToLinear (gainParam->load()));
}

bool GainKnobAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& out = layouts.getMainOutputChannelSet();

    if (out != juce::AudioChannelSet::mono() && out != juce::AudioChannelSet::stereo())
        return false;

    return out == layouts.getMainInputChannelSet();
}

void GainKnobAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const auto numSamples = buffer.getNumSamples();

    // Clear any output channels that have no matching input
    for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear (ch, 0, numSamples);

    smoothedGain.setTargetValue (dbToLinear (gainParam->load()));
    smoothedGain.applyGain (buffer, numSamples);
}

juce::AudioProcessorEditor* GainKnobAudioProcessor::createEditor()
{
    return new GainKnobAudioProcessorEditor (*this);
}

void GainKnobAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, destData);
}

void GainKnobAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GainKnobAudioProcessor();
}
