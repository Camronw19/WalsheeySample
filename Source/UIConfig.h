/*
  ==============================================================================

    UIConfig.h
    Created: 6 Jan 2024 8:24:52am
    Author:  camro

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace spacing
{
    static const int padding1 = 20;
    static const int padding2 = 15;
    static const int padding3 = 10;
    static const int padding4 = 5;
};

namespace AppColors
{
    namespace Dark
    {
        static const juce::Colour textPrimary = juce::Colours::white;
        static const juce::Colour textSecondary = juce::Colour::fromRGBA(255, 255, 255, 178); // 0.7 opacity
        static const juce::Colour textDisabled = juce::Colour::fromRGBA(255, 255, 255, 127); // 0.5 opacity

        static const juce::Colour actionActive = juce::Colours::white;
        static const juce::Colour actionHover = juce::Colour::fromRGBA(255, 255, 255, 20); // 0.08 opacity
        static const juce::Colour actionSelected = juce::Colour::fromRGBA(255, 255, 255, 41); // 0.16 opacity
        static const juce::Colour actionDisabled = juce::Colour::fromRGBA(255, 255, 255, 76); // 0.3 opacity
        static const juce::Colour actionDisabledBackground = juce::Colour::fromRGBA(255, 255, 255, 31); // 0.12 opacity

        static const juce::Colour backgroundDefault = juce::Colour::fromRGB(18, 18, 18);
        static const juce::Colour backgroundPaper = juce::Colour::fromRGB(18, 18, 18);

        static const juce::Colour divider = juce::Colour::fromRGBA(255, 255, 255, 31); // 0.12 

        static const juce::Colour accent = juce::Colour::fromString("#FFBA93FB");
    }

    enum ColourIds
    {
        TextPrimary = 0x2000000,
        TextSecondary,
        TextDisabled,
        ActionActive,
        ActionHover,
        ActionSelected,
        ActionDisabled,
        ActionDisabledBackground,
        BackgroundDefault,
        BackgroundPaper,
        Divider,
        Accent,
    };
};

class DarkLookAndFeel : public juce::LookAndFeel_V4
{
public:
    DarkLookAndFeel()
    {
        //Sliders
        setColour(juce::Slider::thumbColourId, AppColors::Dark::actionActive);
        setColour(juce::Slider::rotarySliderOutlineColourId, AppColors::Dark::backgroundDefault);
        setColour(juce::Slider::rotarySliderFillColourId, AppColors::Dark::accent);

        setColour(juce::Slider::ColourIds::backgroundColourId, AppColors::Dark::backgroundDefault);
        setColour(juce::Slider::ColourIds::trackColourId, AppColors::Dark::accent);


        // Associate each custom ColourId with its corresponding Colour
        setColour(AppColors::ColourIds::TextPrimary, AppColors::Dark::textPrimary);
        setColour(AppColors::ColourIds::TextSecondary, AppColors::Dark::textSecondary);
        setColour(AppColors::ColourIds::TextDisabled, AppColors::Dark::textDisabled);
        setColour(AppColors::ColourIds::ActionActive, AppColors::Dark::actionActive);
        setColour(AppColors::ColourIds::ActionHover, AppColors::Dark::actionHover);
        setColour(AppColors::ColourIds::ActionSelected, AppColors::Dark::actionSelected);
        setColour(AppColors::ColourIds::ActionDisabled, AppColors::Dark::actionDisabled);
        setColour(AppColors::ColourIds::ActionDisabledBackground, AppColors::Dark::actionDisabledBackground);
        setColour(AppColors::ColourIds::BackgroundDefault, AppColors::Dark::backgroundDefault);
        setColour(AppColors::ColourIds::BackgroundPaper, AppColors::Dark::backgroundPaper);
        setColour(AppColors::ColourIds::Divider, AppColors::Dark::divider);
        setColour(AppColors::ColourIds::Accent, AppColors::Dark::accent);
    }

    // Sliders
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPosProportional, float rotaryStartAngle,
        float rotaryEndAngle, juce::Slider& slider) override
    {
        LookAndFeel_V4::drawRotarySlider(g, x, y, width, height, sliderPosProportional,
            rotaryStartAngle, rotaryEndAngle, slider);

        if (slider.getProperties()["shouldDrawValue"])
        {
            auto value = slider.getValue(); 
            juce::Rectangle<int> bounds(x, y, width, height); 
            auto center = bounds.getCentre(); 
            g.drawSingleLineText(juce::String(value), center.x, center.y, juce::Justification::horizontallyCentred);
        }
    }

    // TabbedCompnent 
    void drawTabButton(juce::TabBarButton& button, juce::Graphics& g, bool isMouseOver, bool isMouseDown) override
    {
        auto area = button.getBounds();

        // Customize the background of the tab
        g.setColour(isMouseDown ? findColour(AppColors::ColourIds::ActionSelected) : isMouseOver ? findColour(AppColors::ColourIds::ActionHover) : findColour(AppColors::ColourIds::Divider));
        g.fillRect(area); 

        // Text
        g.setColour(juce::Colours::white);
        g.setFont(14.0f);

        auto textArea = button.getTextArea();
        auto text = button.getButtonText();
        g.drawText(text, textArea, juce::Justification::centred, true);
    }

};

