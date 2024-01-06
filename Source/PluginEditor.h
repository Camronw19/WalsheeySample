/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Model.h"
#include "MainSamplerView.h"


//==============================================================================
/**
*/
class WalsheeySampleAudioProcessorEditor  : public juce::AudioProcessorEditor, 
                                            public DataModel::Listener,
                                            public SampleModel::Listener
                                                                        
{
public:
    WalsheeySampleAudioProcessorEditor (WalsheeySampleAudioProcessor&);
    ~WalsheeySampleAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void fileChanged(SampleModel& sample) override; 
    void activeSampleChanged(SampleModel&) override;
    virtual void adsrChanged(ADSRParameters) override; 

private:
    DataModel mDataModel;
    std::unique_ptr<SampleModel> mActiveSample; 
    juce::AudioFormatManager mFormatManager; 

    MainSamplerView mMainSamplerView; 

    WalsheeySampleAudioProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WalsheeySampleAudioProcessorEditor)
};



