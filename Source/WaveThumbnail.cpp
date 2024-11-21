/*
  ==============================================================================

    WaveThumbnail.cpp
    Created: 15 Oct 2024 5:29:05pm
    Author:  Michael

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveThumbnail.h"

//==============================================================================
WaveThumbnail::WaveThumbnail(YellowRoseAudioProcessor& p) : audioProcessor (p)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

WaveThumbnail::~WaveThumbnail()
{
}

void WaveThumbnail::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::grey.darker());

    auto waveform = audioProcessor.getWaveForm();

    if (waveform.getNumSamples() > 0) {
        juce::Path p;
        mAudioPoints.clear();

        auto waveform = audioProcessor.getWaveForm();
        auto ratio = waveform.getNumSamples() / getWidth();
        auto buffer = waveform.getReadPointer(0);

        //scale audio on x axis
        for (int sample = 0; sample < waveform.getNumSamples(); sample += ratio) {
            mAudioPoints.push_back(buffer[sample]);
        }

        g.setColour(juce::Colours::yellow);
        p.startNewSubPath(0, getHeight() / 2);

        //scale audio on y axis
        for (int sample = 0; sample < mAudioPoints.size(); ++sample) {
            auto point = juce::jmap<float>(mAudioPoints[sample], -1.0f, 1.0f, getHeight(), 0);
            p.lineTo(sample, point);
        }

        g.strokePath(p, juce::PathStrokeType(2));

        g.setColour(juce::Colours::white);
        g.setFont(juce::FontOptions(14.0f));

        auto textBounds = getLocalBounds().reduced(10, 10);

        g.drawFittedText(mFileName, textBounds, juce::Justification::topRight, 1);
    }
    else {
        g.setColour(juce::Colours::white);
        g.setFont(20.0f);
        g.drawFittedText("Drag and drop audio file to load", getLocalBounds(), juce::Justification::centred, 1);
    }
}

void WaveThumbnail::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

bool WaveThumbnail::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (auto file : files) {
        if (file.contains(".wav") || file.contains(".mp3") || file.contains(".aif") || file.contains(".flac")) {
            return true;
        }
    }

    return false;
}

void WaveThumbnail::filesDropped(const juce::StringArray& files, int x, int y)
{
    for (auto file : files) {
        if (isInterestedInFileDrag(file)) {
            auto myFile = std::make_unique<juce::File>(file);
            mFileName = myFile->getFileNameWithoutExtension();

            audioProcessor.loadFile(file);
            repaint();
        }
    }
}