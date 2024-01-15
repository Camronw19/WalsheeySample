/*
  ==============================================================================

    SVGSlider.h
    Created: 15 Jan 2024 11:10:17am
    Author:  camro

  ==============================================================================
*/

#pragma once

/*
  ==============================================================================

    CustomL&F.h
    Created: 14 Jan 2024 2:22:06pm
    Author:  camro

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class SVGStyle
{
public: 
    SVGStyle(const juce::String d, const juce::String& p)
        :dial(d), point(p) {}

    juce::File dial; 
    juce::File point; 
};

class LookAndFeel : public juce::LookAndFeel_V4
{
public:
    LookAndFeel();

    void drawRotarySlider(juce::Graphics&,
        int x, int y, int width, int height,
        float sliderPosProportional,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider&) override;

    void setSliderStyle(const SVGStyle&); 

private:
    std::unique_ptr<juce::Drawable> knobDrawable;
    std::unique_ptr<juce::Drawable> pointDrawable;
};


class SVGRotarySlider : public juce::Slider
{
public: 
    SVGRotarySlider(); 
    ~SVGRotarySlider(); 
    
    void paint(juce::Graphics& g) override;
    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

    void setSliderStyle(const SVGStyle&); 

    juce::Rectangle<int> getSliderBounds() const;

    static struct SVGStyles
    {
        static SVGStyle Midnight; 
    };

private:
    LookAndFeel lnf;
    double customIncrement = 10;
};