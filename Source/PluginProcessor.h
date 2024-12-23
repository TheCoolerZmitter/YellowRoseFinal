/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class YellowRoseAudioProcessor  : public juce::AudioProcessor, public juce::ValueTree::Listener
{
public:
    //==============================================================================
    YellowRoseAudioProcessor();
    ~YellowRoseAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void loadFile(const juce::String& path);
    void loadFile2(const juce::String& path);

    int getNumSamplerSounds() { return mSampler.getNumSounds(); }
    juce::AudioBuffer<float>& getWaveForm() { return mWaveForm; }

    void updateADSR();

    juce::ADSR::Parameters& getADSRparams() { return mADSRparams; }
    juce::AudioProcessorValueTreeState& getAPVTS() { return mAPVTS; }

    void chopSample();
    void saveAudioToFile();

private:
    juce::Synthesiser mSampler;
    const int mNumVoices{ 3 };
    juce::AudioBuffer<float> mWaveForm;
    juce::AudioBuffer<float> mWaveFormEdited;

    juce::ADSR::Parameters mADSRparams;
    int skipFreq = 5000;
    int timeSkip = -4000;

    juce::AudioFormatManager mFormatManager;
    juce::AudioFormatReader* mFormatReader{ nullptr };

    juce::AudioProcessorValueTreeState mAPVTS;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    void valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) override;
    std::atomic<bool> mShouldUpdate{ false };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (YellowRoseAudioProcessor)
};
