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

    static const juce::Colour yellow = juce::Colours::burlywood;
    static const juce::Colour blue = juce::Colour::fromRGB(0, 102, 204);
    static const juce::Colour darkBlue = juce::Colour::fromRGB(11, 12, 14);

    static const juce::Colour purple = juce::Colour::fromRGB(208, 0, 255);
    static const juce::Colour darkPurple = juce::Colour::fromRGB(21,0,26);

    static const juce::Colour Silver = juce::Colour::fromRGB(190, 190, 190);
    static const juce::Colour Silver2 = juce::Colour::fromRGB(170, 170, 170);
    static const juce::Colour Silver3 = juce::Colour::fromRGB(140, 140, 140);

    

    // Define colors using RGBA values
    static const juce::Colour mainWindowColour = Silver;
    static const juce::Colour silderWindowColour = Silver2;
    static const juce::Colour silderWindowAccentColour = Silver3;

    static const juce::Colour labelColour = darkBlue;

    static const juce::Colour componentbackgroundColour = darkBlue;
    static const juce::Colour accentColour = blue;


}