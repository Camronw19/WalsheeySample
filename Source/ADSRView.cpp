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
    :mDataModel(dm),mADSRDisplay(dm)
{
    mDataModel.addListener(*this); 

    addAndMakeVisible(mADSRDisplay);

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
        label->setColour(juce::Label::textColourId, AppColors::backgroundColour);
        label->setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label); 
    }


    mSampleLabel.setJustificationType(juce::Justification::centredBottom); 


    mAttackLabel.setText("Attack", juce::dontSendNotification);
   // mAttackLabel.attachToComponent(&mAttackSlider, false);

    mDecayLabel.setText("Decay", juce::dontSendNotification);
   // mDecayLabel.attachToComponent(&mDecaySlider, false);

    mSustainLabel.setText("Sustain", juce::dontSendNotification);
   // mSustainLabel.attachToComponent(&mSustainSlider, false);

    mReleaseLabel.setText("Release", juce::dontSendNotification); 
   // mReleaseLabel.attachToComponent(&mReleaseSlider, false);
}

ADSRView::~ADSRView()
{
}

void ADSRView::paint (juce::Graphics& g)
{
    g.fillAll(AppColors::mainWindowColour);
    float linePositionY = getLocalBounds().getHeight() * 0.75;
    g.drawLine(getLocalBounds().getX(), linePositionY, getLocalBounds().getRight(), linePositionY);
}

void ADSRView::resized()
{
    auto audioDisplayBounds = getADSRWindowBounds();
    mADSRDisplay.setBounds(audioDisplayBounds);

    //Slider Layout
    juce::FlexBox sliderFlexBox;
    sliderFlexBox.items.add(juce::FlexItem(mAttackSlider).withFlex(1.0, 1.0));
    sliderFlexBox.items.add(juce::FlexItem(mDecaySlider).withFlex(1.0, 1.0));
    sliderFlexBox.items.add(juce::FlexItem(mSustainSlider).withFlex(1.0, 1.0));
    sliderFlexBox.items.add(juce::FlexItem(mReleaseSlider).withFlex(1.0, 1.0));
    //sliderFlexBox.items.add(juce::FlexItem(mSampleLabel).withFlex(1.0, 1.0));

    sliderFlexBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    sliderFlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
    sliderFlexBox.alignContent = juce::FlexBox::AlignContent::center;

    sliderFlexBox.performLayout(getSliderWindowBounds());
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

juce::Rectangle<int> ADSRView::getADSRWindowBounds()
{
    auto bounds = getLocalBounds();
    juce::Rectangle<int> waveformBounds(bounds.getTopLeft().getX(), bounds.getTopLeft().getY(), bounds.getWidth(), bounds.getHeight() * .75);
    waveformBounds = waveformBounds.reduced(10);

    return waveformBounds;
}

juce::Rectangle<int> ADSRView::getSliderWindowBounds()
{
    auto bounds = getLocalBounds();
    int sliderWindowHeight = bounds.getHeight() / 4;
    juce::Rectangle<int> sliderWindowBounds(bounds.getX(), bounds.getBottom() - sliderWindowHeight, bounds.getWidth(), sliderWindowHeight);

    //sliderWindowBounds = sliderWindowBounds.reduced(20);
    return sliderWindowBounds;
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
