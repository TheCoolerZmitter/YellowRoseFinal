#pragma once
#include <JuceHeader.h>

class CustomSamplerSound : public juce::SamplerSound
{
public:
    CustomSamplerSound(const juce::String& name,
        juce::AudioBuffer<float>& bufferToUse,
        const juce::BigInteger& midiNotes,
        int midiNoteForNormalPitch,
        float attackTimeSecs,
        float releaseTimeSecs,
        double maxSampleLengthSeconds);

    const juce::AudioBuffer<float>* getAudioData() const;

private:
    juce::AudioBuffer<float> audioBuffer;
};