/*
  ==============================================================================

    ADSRComponent.cpp
    Created: 15 Oct 2024 6:18:27pm
    Author:  Michael

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ADSRComponent.h"

void ADSRComponent::makeSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText)
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
ADSRComponent::ADSRComponent(YellowRoseAudioProcessor& p) : audioProcessor(p)
{
    makeSlider(mAttackSlider, mAttackLabel, "Attack");
    addAndMakeVisible(mAttackSlider);
    mAttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), "ATTACK", mAttackSlider);

    makeSlider(mDecaySlider, mDecayLabel, "Decay");
    addAndMakeVisible(mDecaySlider);
    mDecayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), "DECAY", mDecaySlider);

    makeSlider(mSustainSlider, mSustainLabel, "Sustain");
    addAndMakeVisible(mSustainSlider);
    mSustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), "SUSTAIN", mSustainSlider);

    makeSlider(mReleaseSlider, mReleaseLabel, "Release");
    addAndMakeVisible(mReleaseSlider);
    mReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), "RELEASE", mReleaseSlider);

}

ADSRComponent::~ADSRComponent()
{
}

void ADSRComponent::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::grey);
}

void ADSRComponent::resized()
{
    const auto startX = 0.1f;
    const auto startY = 0.1f;
    const auto dialWidth = 0.2f;
    const auto dialHeight = 0.75f;

    mAttackSlider.setBoundsRelative(startX, startY, dialWidth, dialHeight);
    mDecaySlider.setBoundsRelative(startX + dialWidth, startY, dialWidth, dialHeight);
    mSustainSlider.setBoundsRelative(startX + 2 * dialWidth, startY, dialWidth, dialHeight);
    mReleaseSlider.setBoundsRelative(startX + 3 * dialWidth, startY, dialWidth, dialHeight);
}
