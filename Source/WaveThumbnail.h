/*
  ==============================================================================

    WaveThumbnail.h
    Created: 15 Oct 2024 5:29:05pm
    Author:  Michael

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class WaveThumbnail  : public juce::Component, public juce::FileDragAndDropTarget
{
public:
    WaveThumbnail(YellowRoseAudioProcessor& p);
    ~WaveThumbnail() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    bool juce::FileDragAndDropTarget::isInterestedInFileDrag(const juce::StringArray& files);
    void juce::FileDragAndDropTarget::filesDropped(const juce::StringArray& files, int x, int y);

private:
    std::vector<float> mAudioPoints;
    bool mShouldBePainting{ false };

    juce::String mFileName{ "" };

    YellowRoseAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveThumbnail)
};
