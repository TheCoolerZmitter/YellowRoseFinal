/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "WaveThumbnail.h"
#include "ADSRComponent.h"
#include "ChopComponent.h"

//==============================================================================
/**
*/
class YellowRoseAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    YellowRoseAudioProcessorEditor (YellowRoseAudioProcessor&);
    ~YellowRoseAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    WaveThumbnail mWaveThumbnail;
    ADSRComponent mADSR;
    ChopComponent mChop;

    YellowRoseAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(YellowRoseAudioProcessorEditor)
};
