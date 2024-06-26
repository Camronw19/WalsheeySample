/*
  ==============================================================================

    MainSamplerView.h
    Created: 28 Dec 2023 12:29:21pm
    Author:  camro

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Model.h"
#include "SampleList.h"
#include "AudioEditor.h"
#include "ADSRView.h"
#include "UIConfig.h"
#include "AudioDisplay.h"
#include "PitchView.h"

class MainSamplerView : public juce::Component, 
                        private DataModel::Listener
{
public:
    MainSamplerView(const DataModel& dataModel, PlaybackPositionOverlay::Providor); 
    ~MainSamplerView() override; 

    void paint(juce::Graphics&) override;
    void resized() override;
    void lookAndFeelChanged() override; 

private:
    DataModel mDataModel; 

    juce::TabbedComponent mTabView;

    SampleList mSampleButtons; 
    AudioEditor mAudioEditor; 
    ADSRView mADSRView; 
    PitchView mPitchView; 
};