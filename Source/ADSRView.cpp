/*
  ==============================================================================

    ADSRView.cpp
    Created: 5 Jan 2024 9:40:11am
    Author:  Camron

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ADSRView.h"

//==============================================================================
ADSRView::ADSRView(const DataModel& dm)
    :dataModel(dm)
{
    dataModel.addListener(*this); 

    for (auto& slider : getSliders())
    {
        addAndMakeVisible(slider);
        slider->setSliderStyle(juce::Slider::RotaryVerticalDrag); 
        slider->addListener(this); 
        slider->setColour(juce::Slider::textBoxTextColourId, juce::Colour::fromRGB(11, 12, 14));
        slider->setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(0, 102, 204));
    }

    attackSlider.setRange(.1, 5, .1); 
    decaySlider.setRange(.1, 5, .1);
    sustainSlider.setRange(.1, 5, .1);
    releaseSlider.setRange(0, 1, .1);

    addAndMakeVisible(activeSampleName); 
    activeSampleName.setJustificationType(juce::Justification::left); 
    activeSampleName.setColour(juce::Label::textColourId, juce::Colour::fromRGB(11, 12, 14));
}

ADSRView::~ADSRView()
{
}

void ADSRView::paint (juce::Graphics& g)
{
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    g.fillAll(juce::Colours::silver);
}

void ADSRView::resized()
{
    auto bounds = getLocalBounds(); 

    activeSampleName.setBounds(bounds.removeFromTop(20)); 

    auto width = getWidth() / 4;

    attackSlider.setBounds(bounds.removeFromLeft(width));
    decaySlider.setBounds(bounds.removeFromLeft(width));
    sustainSlider.setBounds(bounds.removeFromLeft(width));
    releaseSlider.setBounds(bounds.removeFromLeft(width));
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
    activeSampleName.setText(activeSample->getName(), juce::dontSendNotification); 
}

void ADSRView::nameChanged(juce::String name)
{
    activeSampleName.setText(name, juce::dontSendNotification); 
}

std::vector<juce::Slider*> ADSRView::getSliders()
{
    return { &attackSlider, &decaySlider, &sustainSlider, &releaseSlider };
}
