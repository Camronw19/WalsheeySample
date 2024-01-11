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
    :dataModel(dm)
{
    dataModel.addListener(*this); 

    //Initialize sliders 
    for (auto& slider : getSliders())
    {
        slider->setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0); 
        slider->setSliderStyle(juce::Slider::RotaryVerticalDrag); 
        addAndMakeVisible(slider); 
        slider->addListener(this); 
        slider->setColour(juce::Slider::textBoxTextColourId, AppColors::backgroundColour);
        slider->setColour(juce::Slider::thumbColourId, AppColors::accentColour);
    }

    attackSlider.setRange(.1, 5, .1); 
    decaySlider.setRange(.1, 5, .1);
    sustainSlider.setRange(.1, 1, .01);
    releaseSlider.setRange(0, 5, .1);

    //Initialize labels
    for (auto& label : getLabels())
    {
        label->setJustificationType(juce::Justification::centred);
        label->setColour(juce::Label::textColourId, AppColors::backgroundColour);
    }

    sampleLabel.setJustificationType(juce::Justification::left); 
    attackLabel.setText("Attack", juce::dontSendNotification);
    decayLabel.setText("Decay", juce::dontSendNotification);
    sustainLabel.setText("Sustain", juce::dontSendNotification);
    releaseLabel.setText("Release", juce::dontSendNotification); 
}

ADSRView::~ADSRView()
{
}

void ADSRView::paint (juce::Graphics& g)
{
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    g.fillAll(AppColors::mainWindowColour);
}

void ADSRView::resized()
{
    auto bounds = getLocalBounds(); 

    sampleLabel.setBounds(bounds.removeFromTop(20));

    auto width = getWidth() / 4;
    auto labelBounds = bounds.removeFromBottom(20); 

    attackLabel.setBounds(labelBounds.removeFromLeft(width));
    decayLabel.setBounds(labelBounds.removeFromLeft(width));
    sustainLabel.setBounds(labelBounds.removeFromLeft(width));
    releaseLabel.setBounds(labelBounds.removeFromLeft(width));

    for (auto& slider : getSliders())
        slider->setBounds(bounds.removeFromLeft(width)); 
}

void ADSRView::sliderValueChanged(juce::Slider* slider)
{
    if (activeSample != nullptr)
    {
        if (slider == &attackSlider)
        {
            activeSample->setAttack(attackSlider.getValue()); 
        }
        else if (slider == &decaySlider)
        {
            activeSample->setDecay(decaySlider.getValue());
        }
        else if (slider == &sustainSlider)
        {
            activeSample->setSustain(sustainSlider.getValue());
        }
        else if (slider == &releaseSlider)
        {
            activeSample->setRelease(releaseSlider.getValue());
        }
    }
}

void ADSRView::activeSampleChanged(SampleModel& sm)
{
    if (activeSample != nullptr)
        activeSample->removeListener(*this);

    activeSample = std::make_unique<SampleModel>(sm.getState()); 

    if (activeSample != nullptr)
        activeSample->addListener(*this); 

    //Set ADSR parameters
    ADSRParameters params = activeSample->getADSR(); 
    attackSlider.setValue(params.attack);
    decaySlider.setValue(params.decay);
    sustainSlider.setValue(params.sustain);
    releaseSlider.setValue(params.release);

    //Set sample label 
    sampleLabel.setText(activeSample->getName(), juce::dontSendNotification); 
}

void ADSRView::nameChanged(juce::String name)
{
    sampleLabel.setText(name, juce::dontSendNotification);
}

std::vector<juce::Slider*> ADSRView::getSliders()
{
    return { &attackSlider, &decaySlider, &sustainSlider, &releaseSlider };
}

std::vector<juce::Label*> ADSRView::getLabels()
{
    return { &sampleLabel, &attackLabel, &decayLabel, &sustainLabel, &releaseLabel };
}
