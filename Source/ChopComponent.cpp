/*
  ==============================================================================

    ChopComponent.cpp
    Created: 30 Oct 2024 4:43:39pm
    Author:  Michael

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ChopComponent.h"

void ChopComponent::makeSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
    slider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::yellow);
    slider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::black);
    label.setFont(10.0f);
    label.setText(labelText, juce::NotificationType::dontSendNotification);
    label.setJustificationType(juce::Justification::centredBottom);
    label.attachToComponent(&slider, false);
}

//==============================================================================
ChopComponent::ChopComponent(YellowRoseAudioProcessor& p) : audioProcessor(p)
{
    makeSlider(mTimeSkipSlider, mTimeSkipLabel, "Time to Chop");
    addAndMakeVisible(mTimeSkipSlider);
    mTimeSkipAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), "TIMESKIP", mTimeSkipSlider);

    makeSlider(mSkipFreqSlider, mSkipFreqLabel, "Frequency");
    addAndMakeVisible(mSkipFreqSlider);
    mSkipFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), "SKIPFREQ", mSkipFreqSlider);

    mChopLabel.setFont(10.0f);
    mChopLabel.setText("Click to Update", juce::NotificationType::dontSendNotification);
    mChopLabel.setJustificationType(juce::Justification::centredBottom);
    mChopLabel.attachToComponent(&mChopButton, false);
    addAndMakeVisible(mChopButton);
    mChopButton.addListener(this);
}

ChopComponent::~ChopComponent()
{
    mChopButton.removeListener(this);
}

void ChopComponent::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::grey);
}

void ChopComponent::resized()
{
    const auto startX = 0.1f;
    const auto startY = 0.1f;
    const auto dialWidth = 0.2f;
    const auto dialHeight = 0.75f;

    mTimeSkipSlider.setBoundsRelative(startX, startY, dialWidth, dialHeight);
    mSkipFreqSlider.setBoundsRelative(startX + dialWidth, startY, dialWidth, dialHeight);
    mChopButton.setBoundsRelative(startX + 2 * dialWidth, startY, dialWidth , dialHeight);
}

void ChopComponent::buttonClicked(juce::Button* button)
{
    if (button == &mChopButton)
    {
        audioProcessor.chopSample();
        audioProcessor.saveAudioToFile();
    }
}