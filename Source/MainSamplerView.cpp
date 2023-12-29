/*
  ==============================================================================

    MainSamplerView.cpp
    Created: 28 Dec 2023 12:29:21pm
    Author:  camro

  ==============================================================================
*/

#include "MainSamplerView.h"


MainSamplerView::MainSamplerView(const DataModel& dataModel)
    :mDataModel(dataModel), sampleButtons(dataModel.valueTree)
{
    mDataModel.addListener(*this); 

    addAndMakeVisible(sampleButtons); 
}


MainSamplerView::~MainSamplerView()
{


}

void MainSamplerView::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::darkgrey); 
    g.fillAll(); 
}

void MainSamplerView::resized()
{
    auto bounds = getLocalBounds(); 
    sampleButtons.setBounds(bounds); 
}