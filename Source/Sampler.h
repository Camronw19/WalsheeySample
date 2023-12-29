/*
  ==============================================================================

    Sampler.h
    Created: 28 Dec 2023 9:30:12am
    Author:  camro

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

//Represents the constant parts of an audio sample
class Sample
{
public: 
    Sample(juce::AudioFormatReader& source, double maxSampleLengthSecs);

    int getLength() const { return length; }
    double getSampleRate() const { return sampleRate; }
    const juce::AudioBuffer<float>& getBuffer() const { return data;  }

private: 
    int length; 
    double sampleRate; 
    juce::AudioBuffer<float> data; 
};

class SamplerSound
{

};