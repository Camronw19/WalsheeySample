/*
  ==============================================================================

    MainSamplerView.cpp
    Created: 28 Dec 2023 12:29:21pm
    Author:  camro

  ==============================================================================
*/

#include "MainSamplerView.h"


MainSamplerView::MainSamplerView(const DataModel& dataModel)
    :mDataModel(dataModel), sampleButtons(dataModel.getState()), mAudioEditor(dataModel), mADSRView(dataModel)
{
    mDataModel.addListener(*this); 

    addAndMakeVisible(sampleButtons); 
    addAndMakeVisible(mAudioEditor);
    addAndMakeVisible(mADSRView); 
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
    auto bounds = getLocalBounds();
    auto height = getHeight() / 3;

    mAudioEditor.setBounds(bounds.removeFromTop(height));
    mADSRView.setBounds(bounds.removeFromTop(height));
    sampleButtons.setBounds(bounds.removeFromTop(height));
}
