/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Identifiers.h"
#include "MainSamplerView.h"


//==============================================================================
/**
*/
class WalsheeySampleAudioProcessorEditor  : public juce::AudioProcessorEditor, 
                                            public DataModel::Listener
{
public:
    WalsheeySampleAudioProcessorEditor (WalsheeySampleAudioProcessor&);
    ~WalsheeySampleAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void nameChanged(SampleModel& sample) override; 

private:
    DataModel mDataModel; 

    MainSamplerView mMainSamplerView; 

    WalsheeySampleAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WalsheeySampleAudioProcessorEditor)
};

