/*
  ==============================================================================

    MainSamplerView.cpp
    Created: 28 Dec 2023 12:29:21pm
    Author:  camro

  ==============================================================================
*/

#include "MainSamplerView.h"


MainSamplerView::MainSamplerView(const DataModel& dataModel, PlaybackPositionOverlay::Providor providor)
    :mDataModel(dataModel), 
    mSampleButtons(dataModel.getState()), 
    mAudioEditor(dataModel, std::move(providor)), 
    mADSRView(dataModel), 
    mPitchView(dataModel), 
    mTabView(juce::TabbedButtonBar::Orientation::TabsAtTop)
{
    mDataModel.addListener(*this);

    mTabView.addTab("Editor", AppColors::mainWindowColour, &mAudioEditor, false);
    mTabView.addTab("ADSR", AppColors::mainWindowColour, &mADSRView, false);
    mTabView.addTab("Pitch", AppColors::mainWindowColour, &mPitchView, false);
    mTabView.setCurrentTabIndex(0); 

    addAndMakeVisible(mSampleButtons); 
    addAndMakeVisible(mTabView);
}

MainSamplerView::~MainSamplerView()
{
    mDataModel.removeListener(*this); 
}

void MainSamplerView::paint(juce::Graphics& g)
{
    g.setColour(AppColors::mainWindowColour);
    g.fillAll(); 
}

void MainSamplerView::resized()
{
    auto bounds = getLocalBounds();
    
    mSampleButtons.setBounds(bounds.removeFromBottom(150)); 
    mTabView.setBounds(bounds); 
}
