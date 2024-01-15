/*
  ==============================================================================

    SVGSlider.cpp
    Created: 15 Jan 2024 11:10:17am
    Author:  camro

  ==============================================================================
*/

#include "SVGSlider.h"

LookAndFeel::LookAndFeel()
    :juce::LookAndFeel_V4() {}

void LookAndFeel::drawRotarySlider(juce::Graphics& g,
    int x, int y, int width, int height,
    float sliderPosProportional,
    float rotaryStartAngle,
    float rotaryEndAngle,
    juce::Slider& slider)
{

    auto bounds = juce::Rectangle<float>(x, y, width, height);
    bounds.reduce(10, 10); 

    if (knobDrawable != nullptr && pointDrawable != nullptr)
    {
        knobDrawable->setTransformToFit(bounds, juce::RectanglePlacement::fillDestination | juce::RectanglePlacement::centred);
        pointDrawable->setTransformToFit(bounds, juce::RectanglePlacement::fillDestination | juce::RectanglePlacement::centred);

        float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        pointDrawable->setTransform(pointDrawable->getTransform().rotated(angle, bounds.getCentreX(), bounds.getCentreY()));

        knobDrawable->draw(g, 1.0f);
        pointDrawable->draw(g, 1.0f);
    }
    else
    {
        LookAndFeel_V4::drawRotarySlider( g, x, y, width, height,
             sliderPosProportional,
             rotaryStartAngle,
             rotaryEndAngle,
             slider);
    }
}

void LookAndFeel::setSliderStyle(const SVGStyle& style)
{
    if (style.dial.existsAsFile() && style.point.existsAsFile())
    {
        auto drawable = juce::Drawable::createFromSVGFile(style.dial);
        auto drawable2 = juce::Drawable::createFromSVGFile(style.point);

        if (drawable != nullptr && drawable2 != nullptr)
        {
            knobDrawable = std::move(drawable);
            pointDrawable = std::move(drawable2);
        }
    }
    else
    {
        DBG("FILE NOT FOUND");
    }
}

//==============================================================================
SVGRotarySlider::SVGRotarySlider()
    :juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
     juce::Slider::TextEntryBoxPosition::NoTextBox)
{
    setLookAndFeel(&lnf);
    
}

SVGRotarySlider::~SVGRotarySlider()
{
    setLookAndFeel(nullptr);
}

void SVGRotarySlider::paint(juce::Graphics& g)
{
    auto startAng = juce::degreesToRadians(180.f + 60.f);
    auto endAng = juce::degreesToRadians(180.f - 60.f) + juce::MathConstants<float>::twoPi;

    auto range = getRange();
    auto sliderBounds = getSliderBounds();

    getLookAndFeel().drawRotarySlider(g,
        sliderBounds.getX(),
        sliderBounds.getY(),
        sliderBounds.getWidth(),
        sliderBounds.getHeight(),
        juce::jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0),
        startAng,
        endAng,
        *this);
}

void SVGRotarySlider::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    if (wheel.deltaY != 0)
    {
        double newDelta = wheel.deltaY * customIncrement; // customIncrement is your desired step
        setValue(getValue() + newDelta, juce::sendNotificationAsync);
    }
}

juce::Rectangle<int> SVGRotarySlider::getSliderBounds() const
{
    auto bounds = getLocalBounds(); 
    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());

    juce::Rectangle<int> sliderBounds(size, size);
    int centeredX = bounds.getX() + (bounds.getWidth() - sliderBounds.getWidth()) / 2;
    int centeredY = bounds.getY() + (bounds.getHeight() - sliderBounds.getHeight()) / 2;

    sliderBounds.setPosition(centeredX, centeredY);
    return sliderBounds;
}

void SVGRotarySlider::setSliderStyle(const SVGStyle& style)
{
    lnf.setSliderStyle(style); 
}

//Need to change where the asset folder is for release mode! Might need to put them in the binary!
SVGStyle SVGRotarySlider::SVGStyles::Midnight = {
    juce::File::getCurrentWorkingDirectory().getChildFile("Assets").getChildFile("MidnightDial.svg").getFullPathName(),
    juce::File::getCurrentWorkingDirectory().getChildFile("Assets").getChildFile("MidnightDialPoint.svg").getFullPathName()
};

