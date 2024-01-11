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
#include "AudioDisplay.h"

class MainSamplerView : public juce::Component, 
                        public DataModel::Listener
{
public:
    MainSamplerView(const DataModel& dataModel, PlaybackPositionOverlay::Providor); 
    ~MainSamplerView() override; 

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    DataModel mDataModel; 

    SampleList sampleButtons; 
    AudioEditor mAudioEditor; 
    ADSRView mADSRView; 
};