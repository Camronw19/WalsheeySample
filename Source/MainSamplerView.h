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

class MainSamplerView : public juce::Component, 
                        public DataModel::Listener
{
public:
    MainSamplerView(const DataModel& dataModel); 
    ~MainSamplerView() override; 

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    DataModel mDataModel; 

    SampleList sampleButtons; 
    std::shared_ptr<AudioEditor> mAudioEditor = std::make_shared<AudioEditor>();
};