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
