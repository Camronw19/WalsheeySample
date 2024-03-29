/*
  ==============================================================================

    Sampler.h
    Created: 28 Dec 2023 9:30:12am
    Author:  camro

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

//=====================================================================
class ExtendedSamplerSound : public juce::SynthesiserSound
{
public:

    ExtendedSamplerSound(const juce::String& name,
        juce::AudioFormatReader& source,
        const juce::BigInteger& midiNotes,
        int midiNoteForNormalPitch,
        double attackTimeSecs,
        double releaseTimeSecs,
        double maxSampleLengthSeconds);

    ~ExtendedSamplerSound() override;

    const juce::String& getName() const noexcept { return name; }
    juce::AudioBuffer<float>* getAudioData() const noexcept { return data.get(); }

    void setEnvelopeParameters(juce::ADSR::Parameters parametersToUse) { params = parametersToUse; }
    void setPitchShift(int); 

    bool appliesToNote(int midiNoteNumber) override;
    bool appliesToChannel(int midiChannel) override;


private:
    //==============================================================================
    friend class ExtendedSamplerVoice;

    juce::String name;
    std::unique_ptr<juce::AudioBuffer<float>> data;
    double sourceSampleRate;
    juce::BigInteger midiNotes;
    int length = 0, midiRootNote = 0;

    int pitchShiftSemitones = 0; 
    juce::ADSR::Parameters params;

    JUCE_LEAK_DETECTOR(ExtendedSamplerSound)
};

class ExtendedSamplerVoice : public juce::SynthesiserVoice
{
public:
    ExtendedSamplerVoice();
    ~ExtendedSamplerVoice() override;

    //==============================================================================
    bool canPlaySound(juce::SynthesiserSound*) override;

    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int pitchWheel) override;
    void stopNote(float velocity, bool allowTailOff) override;

    void pitchWheelMoved(int newValue) override;
    void controllerMoved(int controllerNumber, int newValue) override;

    void renderNextBlock(juce::AudioBuffer<float>&, int startSample, int numSamples) override;
    using SynthesiserVoice::renderNextBlock;

    //Extended functionality
    double getSourceSamplePosition();
    double getSouceSampleRate(); 

private:
    //==============================================================================
    double pitchRatio = 0;
    double sourceSamplePosition = 0;
    double sourceSampleRate = 0; 
    float lgain = 0, rgain = 0;

    juce::ADSR adsr;

    bool isNotePlaying = false; 
    JUCE_LEAK_DETECTOR(ExtendedSamplerVoice)
};