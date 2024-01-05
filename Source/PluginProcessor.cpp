/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WalsheeySampleAudioProcessor::WalsheeySampleAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    mSampler.addVoice(new juce::SamplerVoice()); 
}

WalsheeySampleAudioProcessor::~WalsheeySampleAudioProcessor()
{
}

//==============================================================================
const juce::String WalsheeySampleAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WalsheeySampleAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool WalsheeySampleAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool WalsheeySampleAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double WalsheeySampleAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WalsheeySampleAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int WalsheeySampleAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WalsheeySampleAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String WalsheeySampleAudioProcessor::getProgramName (int index)
{
    return {};
}

void WalsheeySampleAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void WalsheeySampleAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    mSampler.setCurrentPlaybackSampleRate(sampleRate);
}

void WalsheeySampleAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WalsheeySampleAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void WalsheeySampleAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    process(buffer, midiMessages); 
}

//==============================================================================
bool WalsheeySampleAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* WalsheeySampleAudioProcessor::createEditor()
{
    juce::SpinLock::ScopedLockType lock(commandQueueMutex);
    return new WalsheeySampleAudioProcessorEditor (*this);
}

//==============================================================================
void WalsheeySampleAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void WalsheeySampleAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WalsheeySampleAudioProcessor();
}


void WalsheeySampleAudioProcessor::setSample(std::unique_ptr<juce::AudioFormatReader> reader, int midiNote)
{
    class SetSampleCommand
    {
    public:
        SetSampleCommand(std::unique_ptr<juce::AudioFormatReader> r, int mNote)
            :reader(std::move(r)), midiNote(mNote)
        {

        }

        void operator() (WalsheeySampleAudioProcessor& proc)
        {
            juce::BigInteger range;
            range.setRange(midiNote, 1, true);
     
            proc.mSampler.addSound(new juce::SamplerSound("Sample", *reader, range, midiNote, 0.1, 0.1, 10.0));
        }

        std::unique_ptr<juce::AudioFormatReader> reader; 
        int midiNote; 
    };

    if (reader == nullptr)
    {
        return; 
    }
    else 
    {
        DBG("YES"); 
        mCommands.push(SetSampleCommand(std::move(reader), midiNote));
    }
}

void WalsheeySampleAudioProcessor::setADSR(ADSRParameters adsr)
{
    class SetADSRCommand
    {
    public:
        SetADSRCommand(ADSRParameters adsrParams)
            :adsr(adsrParams)
        {

        }

        void operator() (WalsheeySampleAudioProcessor& proc)
        {
            auto sound = dynamic_cast<juce::SamplerSound*>(proc.mSampler.getSound(0).get());
            sound->setEnvelopeParameters(juce::ADSR::Parameters(adsr.attack, adsr.decay, adsr.sustain, adsr.release)); 
        }

        ADSRParameters adsr; 
    };

     mCommands.push(SetADSRCommand(adsr));
}

void WalsheeySampleAudioProcessor::process(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    const juce::GenericScopedTryLock<juce::SpinLock> lock(commandQueueMutex);

    if (lock.isLocked())
        mCommands.call(*this);

    mSampler.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples()); 
}

    