#include "CustomSamplerSound.h"

CustomSamplerSound::CustomSamplerSound(const juce::String& name,
    juce::AudioBuffer<float>& bufferToUse,
    const juce::BigInteger& midiNotes,
    int midiNoteForNormalPitch,
    float attackTimeSecs,
    float releaseTimeSecs,
    double maxSampleLengthSeconds)
    : juce::SamplerSound(name,
        nullptr, // Pass nullptr for the reader; we will handle it differently
        midiNotes,
        midiNoteForNormalPitch,
        attackTimeSecs,
        releaseTimeSecs,
        maxSampleLengthSeconds),
    audioBuffer(bufferToUse)
{
}

const juce::AudioBuffer<float>* CustomSamplerSound::getAudioData() const
{
    return &audioBuffer;
}