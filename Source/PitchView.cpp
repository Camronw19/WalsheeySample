/*
  ==============================================================================

    PitchView.cpp
    Created: 9 Feb 2024 1:48:56pm
    Author:  camro

  ==============================================================================
*/

#include "PitchView.h"

PitchView::PitchView(const DataModel& dm)
    :mDataModel(dm)
{
    mDataModel.addListener(*this);

    mPitchLabel.setText("Pitch Shift", juce::dontSendNotification);
    mPitchLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(mPitchLabel);

    mPitchSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 40, 15);
    mPitchSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mPitchSlider.setRange(juce::Range<double>(-12, 12), 1);
    mPitchSlider.addListener(this);
    mPitchSlider.getProperties().set("shouldDrawValue", true);
    addAndMakeVisible(mPitchSlider);
    
}

PitchView::~PitchView()
{
    mDataModel.removeListener(*this);
}

void PitchView::paint(juce::Graphics& g)
{
    g.setColour(getLookAndFeel().findColour(AppColors::ColourIds::BackgroundDefault));
    g.fillAll();

    g.setColour(getLookAndFeel().findColour(AppColors::ColourIds::Divider));
    auto bounds = getLocalBounds(); 
    bounds.removeFromTop(spacing::padding3);
    bounds.removeFromLeft(spacing::padding3);
    bounds.removeFromRight(spacing::padding3);
    g.fillRoundedRectangle(bounds.toFloat(), spacing::padding4); 
}

void PitchView::resized()
{
    juce::Grid grid;

    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;
    using Px = juce::Grid::Px;

    grid.templateRows = { Track(Fr(3)), Track(Fr(1)), Track(Fr(3)), Track(Fr(1)) };
    grid.templateColumns = { Track(Fr(1)), Track(Fr(1)), Track(Fr(1)), Track(Fr(1)) };
    grid.items = { juce::GridItem(mPitchSlider).withArea(1,1), 
                   juce::GridItem(mPitchLabel).withArea(2,1)
    };

    grid.performLayout(getLocalBounds().reduced(spacing::padding3));
}

void PitchView::nameChanged(juce::String)
{

}

void PitchView::activeSampleChanged(SampleModel& sm)
{
    mPitchSlider.setValue(sm.getPitchShift());

    if (mActiveSample != nullptr)
        mActiveSample->removeListener(*this);

    mActiveSample = std::make_unique<SampleModel>(sm.getState());

    if (mActiveSample != nullptr)
        mActiveSample->addListener(*this);
}

void PitchView::sliderValueChanged(juce::Slider* sliderChanged)
{

    if (mActiveSample != nullptr)
    {
        if (sliderChanged == &mPitchSlider)
        {
            mActiveSample->setPitchShift(mPitchSlider.getValue());
        }
    }
}

void PitchView::lookAndFeelChanged()
{
    if (!dynamic_cast<juce::LookAndFeel_V4*>(&getLookAndFeel()))
    {
        mPitchLabel.setColour(juce::Label::textColourId, getLookAndFeel().findColour(AppColors::ColourIds::TextPrimary));
    }
}
