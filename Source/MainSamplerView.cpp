/*
  ==============================================================================

    MainSamplerView.cpp
    Created: 28 Dec 2023 12:29:21pm
    Author:  camro

  ==============================================================================
*/

#include "MainSamplerView.h"


MainSamplerView::MainSamplerView(const DataModel& dataModel, PlaybackPositionOverlay::Providor providor)
    :mDataModel(dataModel), mSampleButtons(dataModel.getState()), mAudioEditor(dataModel, std::move(providor)), mADSRView(dataModel)
{
    mDataModel.addListener(*this); 

    addAndMakeVisible(mSampleButtons); 
    addAndMakeVisible(mAudioEditor);
    addAndMakeVisible(mADSRView); 
}

MainSamplerView::~MainSamplerView()
{

}

void MainSamplerView::paint(juce::Graphics& g)
{

    g.setColour(AppColors::mainWindowColour);
    g.fillAll(); 
}

void MainSamplerView::resized()
{
    auto bounds = getLocalBounds();
    auto height = getHeight() / 3;

    mAudioEditor.setBounds(bounds.removeFromTop(height));
    mADSRView.setBounds(bounds.removeFromTop(height));
    mSampleButtons.setBounds(bounds.removeFromTop(height));
}
