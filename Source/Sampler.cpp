/*
  ==============================================================================

    Sampler.cpp
    Created: 28 Dec 2023 9:30:12am
    Author:  camro

  ==============================================================================
*/

#include "Sampler.h"

Sample::Sample(juce::AudioFormatReader& source, double maxSampleLengthSecs)
    : sampleRate(source.sampleRate),
    length(juce::jmin(int(source.lengthInSamples),
        int(maxSampleLengthSecs* sampleRate))),
    data(juce::jmin(2, int(source.numChannels)), length + 4)
{
    if (length == 0)
        throw std::runtime_error("Unable to load sample");

    source.read(&data, 0, length + 4, 0, true, true);
}

//================================================================================
ExtendedSamplerSound::ExtendedSamplerSound(const juce::String& soundName,
    juce::AudioFormatReader& source,
    const juce::BigInteger& notes,
    int midiNoteForNormalPitch,
    double attackTimeSecs,
    double releaseTimeSecs,
    double maxSampleLengthSeconds)
    : name(soundName),
    sourceSampleRate(source.sampleRate),
    midiNotes(notes),
    midiRootNote(midiNoteForNormalPitch)
{
    if (sourceSampleRate > 0 && source.lengthInSamples > 0)
    {
        length = juce::jmin((int)source.lengthInSamples,
            (int)(maxSampleLengthSeconds * sourceSampleRate));

        data.reset(new juce::AudioBuffer<float>(juce::jmin(2, (int)source.numChannels), length + 4));

        source.read(data.get(), 0, length + 4, 0, true, true);

        params.attack = static_cast<float> (attackTimeSecs);
        params.release = static_cast<float> (releaseTimeSecs);
    }
}

ExtendedSamplerSound::~ExtendedSamplerSound()
{
}

bool ExtendedSamplerSound::appliesToNote(int midiNoteNumber)
{
    return midiNotes[midiNoteNumber];
}

bool ExtendedSamplerSound::appliesToChannel(int /*midiChannel*/)
{
    return true;
}

//================================================================================
ExtendedSamplerVoice::ExtendedSamplerVoice() {}
ExtendedSamplerVoice::~ExtendedSamplerVoice() {}

bool ExtendedSamplerVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<const ExtendedSamplerSound*> (sound) != nullptr;
}

void ExtendedSamplerVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* s, int /*currentPitchWheelPosition*/)
{
    if (auto* sound = dynamic_cast<const ExtendedSamplerSound*> (s))
    {
        isNotePlaying = true; 
        sourceSampleRate = sound->sourceSampleRate; 

        pitchRatio = std::pow(2.0, (midiNoteNumber - sound->midiRootNote) / 12.0)
            * sound->sourceSampleRate / getSampleRate();

        sourceSamplePosition = 0.0;
        lgain = velocity;
        rgain = velocity;

        adsr.setSampleRate(sound->sourceSampleRate);
        adsr.setParameters(sound->params);

        adsr.noteOn();
    }
    else
    {
        jassertfalse; // this object can only play SamplerSounds!
    }
}

void ExtendedSamplerVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    isNotePlaying = false;
    if (allowTailOff)
    {
        adsr.noteOff();
    }
    else
    {
        clearCurrentNote();
        adsr.reset();
    }

    sourceSamplePosition = 0.0;
}

void ExtendedSamplerVoice::pitchWheelMoved(int /*newValue*/) {}
void ExtendedSamplerVoice::controllerMoved(int /*controllerNumber*/, int /*newValue*/) {}

//==============================================================================
void ExtendedSamplerVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (!isNotePlaying) return;
    if (auto* playingSound = static_cast<ExtendedSamplerSound*> (getCurrentlyPlayingSound().get()))
    {
        auto& data = *playingSound->data;
        const float* const inL = data.getReadPointer(0);
        const float* const inR = data.getNumChannels() > 1 ? data.getReadPointer(1) : nullptr;

        float* outL = outputBuffer.getWritePointer(0, startSample);
        float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer(1, startSample) : nullptr;

        while (--numSamples >= 0)
        {
            auto pos = (int)sourceSamplePosition;
            auto alpha = (float)(sourceSamplePosition - pos);
            auto invAlpha = 1.0f - alpha;

            // just using a very simple linear interpolation here..
            float l = (inL[pos] * invAlpha + inL[pos + 1] * alpha);
            float r = (inR != nullptr) ? (inR[pos] * invAlpha + inR[pos + 1] * alpha)
                : l;

            auto envelopeValue = adsr.getNextSample();

            l *= lgain * envelopeValue;
            r *= rgain * envelopeValue;

            if (outR != nullptr)
            {
                *outL++ += l;
                *outR++ += r;
            }
            else
            {
                *outL++ += (l + r) * 0.5f;
            }

            sourceSamplePosition += pitchRatio;

            if (sourceSamplePosition > playingSound->length)
            {
                stopNote(0.0f, false);
                break;
            }
        }
    }
}

double ExtendedSamplerVoice::getSourceSamplePosition()
{
    return sourceSamplePosition; 
}

double ExtendedSamplerVoice::getSouceSampleRate()
{
    return sourceSampleRate; 
}


