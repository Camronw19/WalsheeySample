/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WalsheeySampleAudioProcessorEditor::WalsheeySampleAudioProcessorEditor (WalsheeySampleAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), mMainSamplerView(mDataModel, [&p] { return p.getPlaybackPosition(); })
{
    setLookAndFeel(&mLF);
    mMainSamplerView.setLookAndFeel(&mLF); 
    mFormatManager.registerBasicFormats(); 

    mDataModel.addListener(*this); 
    mDataModel.initializeDefaultModel(8);

    addAndMakeVisible(mMainSamplerView); 
    

    setSize (800, 500);
}

WalsheeySampleAudioProcessorEditor::~WalsheeySampleAudioProcessorEditor()
{
    setLookAndFeel(nullptr); 
}

//==============================================================================
void WalsheeySampleAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour(AppColors::ColourIds::BackgroundDefault));
}

void WalsheeySampleAudioProcessorEditor::resized()
{
    mMainSamplerView.setBounds(getLocalBounds()); 
}

void WalsheeySampleAudioProcessorEditor::fileChanged(SampleModel& ModelChanged)
{
    auto file = ModelChanged.getAudioFile(); 
    if (file->exists())
        audioProcessor.setSample(std::unique_ptr<juce::AudioFormatReader>(mFormatManager.createReaderFor(*file)), ModelChanged.getMidiNote(), ModelChanged.getId());

}

void WalsheeySampleAudioProcessorEditor::activeSampleChanged(SampleModel& sm)
{
    if (mActiveSample != nullptr)
        mActiveSample->removeListener(*this);

    mActiveSample = std::make_unique<SampleModel>(sm.getState());

    if (mActiveSample != nullptr)
        mActiveSample->addListener(*this);
}

void WalsheeySampleAudioProcessorEditor::adsrChanged(ADSRParameters adsrParams)
{
   audioProcessor.setADSR(adsrParams, mActiveSample->getId()); 
}

void WalsheeySampleAudioProcessorEditor::pitchShiftChanged(int semitones)
{
    audioProcessor.setPitchShift(semitones, mActiveSample->getId()); 
}