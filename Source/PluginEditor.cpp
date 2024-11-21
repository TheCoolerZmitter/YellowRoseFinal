/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
YellowRoseAudioProcessorEditor::YellowRoseAudioProcessorEditor (YellowRoseAudioProcessor& p)
    : AudioProcessorEditor (&p), mWaveThumbnail(p), mADSR(p), mChop(p), audioProcessor(p)
{
    //mWaveThumbnail.onClick = [&]() { audioProcessor.loadFile(); };
    addAndMakeVisible(mWaveThumbnail);
    addAndMakeVisible(mADSR);
    addAndMakeVisible(mChop);

    setSize(600, 400);
}

YellowRoseAudioProcessorEditor::~YellowRoseAudioProcessorEditor()
{
}

//==============================================================================
void YellowRoseAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::grey);
}

void YellowRoseAudioProcessorEditor::resized()
{
    mWaveThumbnail.setBoundsRelative(0.0f, 0.15f, 1.0f, 0.5f);
    mADSR.setBoundsRelative(0.5f, 0.75f, 0.5f, 0.25f);
    mChop.setBoundsRelative(0.0f, 0.75f, 0.5f, 0.25f);
}