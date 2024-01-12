/*
  ==============================================================================

    ADSRView.cpp
    Created: 5 Jan 2024 9:40:11am
    Author:  Camron

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ADSRView.h"
#include "UIConfig.h"

//==============================================================================
ADSRView::ADSRView(const DataModel& dm)
    :mDataModel(dm)
{
    mDataModel.addListener(*this); 

    // Initialize sliders 
    for (auto& slider : getSliders())
    {
        slider->setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0); 
        slider->setSliderStyle(juce::Slider::RotaryVerticalDrag); 
        slider->setColour(juce::Slider::textBoxTextColourId, AppColors::backgroundColour);
        slider->setColour(juce::Slider::thumbColourId, AppColors::accentColour);
        slider->addListener(this); 
        addAndMakeVisible(slider); 
    }

    mAttackSlider.setRange(.1, 5, .1); 
    mDecaySlider.setRange(.1, 5, .1);
    mSustainSlider.setRange(.1, 1, .01);
    mReleaseSlider.setRange(0, 5, .1);

    // Initialize labels
    for (auto& label : getLabels())
    {
        label->setJustificationType(juce::Justification::centred);
        label->setColour(juce::Label::textColourId, AppColors::backgroundColour);
        addAndMakeVisible(label); 
    }

    mSampleLabel.setJustificationType(juce::Justification::left); 
    mAttackLabel.setText("Attack", juce::dontSendNotification);
    mDecayLabel.setText("Decay", juce::dontSendNotification);
    mSustainLabel.setText("Sustain", juce::dontSendNotification);
    mReleaseLabel.setText("Release", juce::dontSendNotification); 
}

ADSRView::~ADSRView()
{
}

void ADSRView::paint (juce::Graphics& g)
{
    g.fillAll(AppColors::mainWindowColour);
}

void ADSRView::resized()
{
    auto bounds = getLocalBounds(); 
    auto labelBounds = bounds.removeFromBottom(20); 
    mSampleLabel.setBounds(bounds.removeFromTop(20));

    auto width = getWidth() / 4;

    for (auto& label : getLabels())
    {
        if(label != &mSampleLabel)
            label->setBounds(labelBounds.removeFromLeft(width));
    }
   
    for (auto& slider : getSliders())
        slider->setBounds(bounds.removeFromLeft(width)); 
}

void ADSRView::sliderValueChanged(juce::Slider* slider)
{
    if (mActiveSample != nullptr)
    {
        if (slider == &mAttackSlider)
        {
            mActiveSample->setAttack(mAttackSlider.getValue()); 
        }
        else if (slider == &mDecaySlider)
        {
            mActiveSample->setDecay(mDecaySlider.getValue());
        }
        else if (slider == &mSustainSlider)
        {
            mActiveSample->setSustain(mSustainSlider.getValue());
        }
        else if (slider == &mReleaseSlider)
        {
            mActiveSample->setRelease(mReleaseSlider.getValue());
        }
    }
}

void ADSRView::activeSampleChanged(SampleModel& sm)
{
    if (mActiveSample != nullptr)
        mActiveSample->removeListener(*this);

    mActiveSample = std::make_unique<SampleModel>(sm.getState()); 

    if (mActiveSample != nullptr)
        mActiveSample->addListener(*this); 

    //Set ADSR parameters
    ADSRParameters params = mActiveSample->getADSR(); 
    mAttackSlider.setValue(params.attack);
    mDecaySlider.setValue(params.decay);
    mSustainSlider.setValue(params.sustain);
    mReleaseSlider.setValue(params.release);

    //Set sample label 
    mSampleLabel.setText(mActiveSample->getName(), juce::dontSendNotification); 
}

void ADSRView::nameChanged(juce::String name)
{
    mSampleLabel.setText(name, juce::dontSendNotification);
}

std::vector<juce::Slider*> ADSRView::getSliders()
{
    return { &mAttackSlider, &mDecaySlider, &mSustainSlider, &mReleaseSlider };
}

std::vector<juce::Label*> ADSRView::getLabels()
{
    return { &mSampleLabel, &mAttackLabel, &mDecayLabel, &mSustainLabel, &mReleaseLabel };
}


//===============================================================

PitchView::PitchView(const DataModel& dm)
    :mDataModel(dm)
{
    mDataModel.addListener(*this); 

    mPitchLabel.setText("Pitch Shift", juce::dontSendNotification); 
    mPitchLabel.setJustificationType(juce::Justification::centred);
    mPitchLabel.setColour(juce::Label::textColourId, AppColors::backgroundColour);
    addAndMakeVisible(mPitchLabel);

    mPitchSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    mPitchSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mPitchSlider.setColour(juce::Slider::textBoxTextColourId, AppColors::backgroundColour);
    mPitchSlider.setColour(juce::Slider::thumbColourId, AppColors::accentColour);
    mPitchSlider.setRange(juce::Range<double>(-12, 12), 1); 
    mPitchSlider.addListener(this);
    addAndMakeVisible(mPitchSlider);
}

PitchView::~PitchView()
{
    mDataModel.removeListener(*this); 
}

void PitchView::paint(juce::Graphics&)
{

}

void PitchView::resized()
{
    auto bounds = getLocalBounds(); 
    auto labelBounds = bounds.removeFromBottom(20); 

    mPitchLabel.setBounds(labelBounds); 
    mPitchSlider.setBounds(bounds); 
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