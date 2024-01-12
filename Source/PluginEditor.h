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
                                            private DataModel::Listener,
                                            private SampleModel::Listener
                                                                        
{
public:
    WalsheeySampleAudioProcessorEditor (WalsheeySampleAudioProcessor&);
    ~WalsheeySampleAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    //Data model listener
    void activeSampleChanged(SampleModel&) override;
    void fileChanged(SampleModel&) override; 

    //Sample model listener
    virtual void adsrChanged(ADSRParameters) override; 

    DataModel mDataModel;
    std::unique_ptr<SampleModel> mActiveSample; 
    juce::AudioFormatManager mFormatManager; 

    MainSamplerView mMainSamplerView; 

    WalsheeySampleAudioProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WalsheeySampleAudioProcessorEditor)
};



