/*
  ==============================================================================

    ChopComponent.h
    Created: 30 Oct 2024 4:43:39pm
    Author:  Michael

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class ChopComponent  : public juce::Component, public juce::Button::Listener
{
public:
    ChopComponent(YellowRoseAudioProcessor& p);
    ~ChopComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;

private:
    void makeSlider(juce::Slider& slider, juce::Label& label, const juce::String& lableText);

    juce::Slider mTimeSkipSlider, mSkipFreqSlider;
    juce::TextButton mChopButton;
    juce::Label mTimeSkipLabel, mSkipFreqLabel, mChopLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mTimeSkipAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mSkipFreqAttachment;

    YellowRoseAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChopComponent)
};
