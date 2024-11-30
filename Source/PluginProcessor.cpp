/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
YellowRoseAudioProcessor::YellowRoseAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), mAPVTS(*this, nullptr, "PARAMETERS", createParameters())
#endif
{
    mFormatManager.registerBasicFormats();
    mAPVTS.state.addListener(this);

    for (int i = 0; i < mNumVoices; i++) {
        mSampler.addVoice(new juce::SamplerVoice());
    }
}

YellowRoseAudioProcessor::~YellowRoseAudioProcessor()
{
}

//==============================================================================
const juce::String YellowRoseAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool YellowRoseAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool YellowRoseAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool YellowRoseAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double YellowRoseAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int YellowRoseAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int YellowRoseAudioProcessor::getCurrentProgram()
{
    return 0;
}

void YellowRoseAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String YellowRoseAudioProcessor::getProgramName (int index)
{
    return {};
}

void YellowRoseAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void YellowRoseAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    mSampler.setCurrentPlaybackSampleRate(sampleRate);
    updateADSR();
}

void YellowRoseAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool YellowRoseAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void YellowRoseAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear the buffer to avoid overwriting with unwanted data
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    if (mShouldUpdate) {
        updateADSR();
        mShouldUpdate = false;
    }

    // Render the sample into the buffer
    mSampler.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool YellowRoseAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* YellowRoseAudioProcessor::createEditor()
{
    return new YellowRoseAudioProcessorEditor (*this);
}

//==============================================================================
void YellowRoseAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void YellowRoseAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void YellowRoseAudioProcessor::loadFile(const juce::String& path)
{
    mSampler.clearSounds();

    auto file = juce::File(path);
    mFormatReader = mFormatManager.createReaderFor(file);

    auto sampleLength = static_cast<int>(mFormatReader->lengthInSamples);

    mWaveForm.setSize(1, sampleLength);

    mFormatReader->read(&mWaveForm, 0, sampleLength, 0, true, false);

    chopSample();
    saveAudioToFile();
}

void YellowRoseAudioProcessor::loadFile2(const juce::String& path) {
    mSampler.clearSounds();

    auto file = juce::File(path);
    mFormatReader = mFormatManager.createReaderFor(file);

    auto sampleLength = static_cast<int>(mFormatReader->lengthInSamples);

    mWaveForm.setSize(1, sampleLength);

    mFormatReader->read(&mWaveForm, 0, sampleLength, 0, true, false);

    juce::BigInteger range;
    range.setRange(0, 127, true);
    mSampler.addSound(new juce::SamplerSound("Sample", *mFormatReader, range, 60, 0, 0, 60));

    updateADSR();
    file.deleteFile();
}

void YellowRoseAudioProcessor::chopSample() {

    timeSkip = mAPVTS.getRawParameterValue("TIMESKIP")->load();
    skipFreq = mAPVTS.getRawParameterValue("SKIPFREQ")->load();

    if (skipFreq + timeSkip < 500 && skipFreq + timeSkip > -500) {
        timeSkip = 0;
    }

    int numChannels = mWaveForm.getNumChannels();
    int originalNumSamples = mWaveForm.getNumSamples();
    
    auto fullCycles = originalNumSamples / (skipFreq + timeSkip);
    auto remainingSamples = originalNumSamples % (skipFreq + timeSkip);

    auto numSamples = (fullCycles * skipFreq) + std::min(remainingSamples, skipFreq);

    //auto numSamples = originalNumSamples;

    mWaveFormEdited.setSize(numChannels, numSamples);

    for (int channel = 0; channel < numChannels; channel++)
    {
        float* newData = mWaveFormEdited.getWritePointer(channel);
        const float* oldData = mWaveForm.getReadPointer(channel);

        int originalIndex = 0;

        for (int i = 0; i < numSamples; i++) {
            
            if (i != 0 && i % skipFreq == 0) {
                originalIndex += timeSkip;
            }
            else {
                originalIndex++;
            }

            if (originalIndex >= 0 && originalIndex < originalNumSamples) {
                newData[i] = oldData[originalIndex];
            }
            else if (originalIndex < 0) {
                newData[i] = 0;
            }
            else if (originalIndex >= originalNumSamples) {
                newData[i] = 0;
            }
        }
    }
}

void YellowRoseAudioProcessor::saveAudioToFile() {
    // Register basic formats (like WAV)
    mFormatManager.registerBasicFormats();

    // Create an output file
    juce::File outputFile = juce::File::getCurrentWorkingDirectory().getChildFile("broISwearToGodPleaseDoNotHaveAFileWithThisNameAlready.wav");

    // Create a file output stream for the output file
    std::unique_ptr<juce::FileOutputStream> fileOutputStream(outputFile.createOutputStream());

    if (fileOutputStream != nullptr)
    {
        // Find the WAV format
        if (auto* wavFormat = mFormatManager.findFormatForFileExtension("wav"))
        {
            // Create the writer for the output file
            std::unique_ptr<juce::AudioFormatWriter> writer(wavFormat->createWriterFor(
                fileOutputStream.release(), // Release ownership of the output stream
                44100.0, // Sample rate
                mWaveFormEdited.getNumChannels(), // Number of channels
                16, // Bits per sample
                {}, // Optional metadata
                0  // Compression type (0 for none)
            ));

            if (writer != nullptr)
            {
                // Write the audio buffer to the file
                writer->writeFromAudioSampleBuffer(mWaveFormEdited, 0, mWaveFormEdited.getNumSamples());
                DBG("Audio written to file successfully.");
            }
            else
            {
                DBG("Failed to create writer.");
            }
        }
        else
        {
            DBG("WAV format not found.");
        }
    }
    else
    {
        DBG("Failed to create output file stream.");
    }

    loadFile2(outputFile.getFullPathName());
}

void YellowRoseAudioProcessor::updateADSR() {
    mADSRparams.attack = mAPVTS.getRawParameterValue("ATTACK")->load();
    mADSRparams.decay = mAPVTS.getRawParameterValue("DECAY")->load();
    mADSRparams.sustain = mAPVTS.getRawParameterValue("SUSTAIN")->load();
    mADSRparams.release = mAPVTS.getRawParameterValue("RELEASE")->load();

    for (int i = 0; i < mSampler.getNumSounds(); i++) {
        if (auto sound = dynamic_cast<juce::SamplerSound*>(mSampler.getSound(i).get())) {
            sound->setEnvelopeParameters(mADSRparams);
        }
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout YellowRoseAudioProcessor::createParameters() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    parameters.push_back(std::make_unique < juce::AudioParameterFloat > ("ATTACK", "Attack", 0.0f, 2.0f, 0.0f));
    parameters.push_back(std::make_unique < juce::AudioParameterFloat > ("DECAY", "Decay", 0.0f, 2.0f, 1.0f));
    parameters.push_back(std::make_unique < juce::AudioParameterFloat > ("SUSTAIN", "Sustain", 0.0f, 1.0f, 1.0f));
    parameters.push_back(std::make_unique < juce::AudioParameterFloat > ("RELEASE", "Release", 0.0f, 1.0f, 0.1f));
    parameters.push_back(std::make_unique < juce::AudioParameterInt > ("TIMESKIP", "Time to Skip", 0, 10000, 0));
    parameters.push_back(std::make_unique < juce::AudioParameterInt > ("SKIPFREQ", "Frequency", 500, 20000, 2000));

    return { parameters.begin(), parameters.end() };
}

void YellowRoseAudioProcessor::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) {
    mShouldUpdate = true;
}



//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new YellowRoseAudioProcessor();
}
