#include "PluginEditor.h"

GainKnobAudioProcessorEditor::GainKnobAudioProcessorEditor (GainKnobAudioProcessor& p)
    : AudioProcessorEditor (&p),
      gainAttachment (p.apvts, GainKnobAudioProcessor::gainParamID, gainSlider)
{
    gainSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 90, 24);
    gainSlider.setTextValueSuffix (" dB");
    gainSlider.setNumDecimalPlacesToDisplay (1);
    addAndMakeVisible (gainSlider);

    gainLabel.setText ("Gain", juce::dontSendNotification);
    gainLabel.setJustificationType (juce::Justification::centred);
    gainLabel.setFont (juce::FontOptions (18.0f));
    addAndMakeVisible (gainLabel);

    setSize (240, 280);
}

void GainKnobAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void GainKnobAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (20);
    gainLabel.setBounds (area.removeFromTop (28));
    gainSlider.setBounds (area);
}
