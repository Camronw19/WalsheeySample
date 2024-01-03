/*
  ==============================================================================

    MainSamplerView.cpp
    Created: 28 Dec 2023 12:29:21pm
    Author:  camro

  ==============================================================================
*/

#include "MainSamplerView.h"


MainSamplerView::MainSamplerView(const DataModel& dataModel)
    :mDataModel(dataModel), sampleButtons(dataModel.getState())
{
    mDataModel.addListener(*this); 

    addAndMakeVisible(sampleButtons); 
    addAndMakeVisible(*mAudioEditor);
}

MainSamplerView::~MainSamplerView()
{

}

void MainSamplerView::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::grey); 
    g.fillAll(); 
}

void MainSamplerView::resized()
{
    sampleButtons.setBounds(0,(getHeight() / 2),getWidth(),(getHeight() / 2));
    mAudioEditor->setBounds(0, 0, getWidth(), (getHeight() / 3));
}
