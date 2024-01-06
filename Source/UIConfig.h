/*
  ==============================================================================

    UIConfig.h
    Created: 6 Jan 2024 8:24:52am
    Author:  camro

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace AppColors
{
    // Define colors using RGBA values
    static const juce::Colour backgroundColour = juce::Colour::fromRGBA(30, 30, 30, 255);
    static const juce::Colour accentColour = juce::Colour::fromRGBA(180, 70, 70, 255);

    // Or define colors using hexadecimal
    static const juce::Colour primaryColour = juce::Colour::fromString("#FF5733");
    static const juce::Colour secondaryColour = juce::Colour::fromString("#33FF57");
}