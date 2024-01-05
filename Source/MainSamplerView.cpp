/*
  ==============================================================================

    MainSamplerView.cpp
    Created: 28 Dec 2023 12:29:21pm
    Author:  camro

  ==============================================================================
*/

#include "MainSamplerView.h"


MainSamplerView::MainSamplerView(const DataModel& dataModel)
    :mDataModel(dataModel), sampleButtons(dataModel.getState()), mAudioEditor(dataModel)
{
    mDataModel.addListener(*this); 

    addAndMakeVisible(sampleButtons); 
    addAndMakeVisible(mAudioEditor);
}

MainSamplerView::~MainSamplerView()
{

}

void MainSamplerView::paint(juce::Graphics& g)
{

    g.setColour(juce::Colour::fromRGB(192, 192, 192));
    g.fillAll(); 

    g.setColour(juce::Colour::fromRGB(11, 12, 14));
    g.drawRect(getLocalBounds().toFloat());
}

void MainSamplerView::resized()
{
    sampleButtons.setBounds(0,(getHeight() / 2),getWidth(),(getHeight() / 2));
    mAudioEditor.setBounds(0, 0, getWidth(), (getHeight() / 3));
}
