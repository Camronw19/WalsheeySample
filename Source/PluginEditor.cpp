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
    mFormatManager.registerBasicFormats(); 

    mDataModel.addListener(*this); 
    mDataModel.initializeDefaultModel(8);

    addAndMakeVisible(mMainSamplerView); 

    setResizable(true, true); 
    setResizeLimits(400, 300, 1200, 900); 
    setSize (800, 600);
}

WalsheeySampleAudioProcessorEditor::~WalsheeySampleAudioProcessorEditor()
{
}

//==============================================================================
void WalsheeySampleAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void WalsheeySampleAudioProcessorEditor::resized()
{
    mMainSamplerView.setBounds(getLocalBounds()); 
}

void WalsheeySampleAudioProcessorEditor::fileChanged(SampleModel& sample)
{
    std::shared_ptr<juce::File> sampleFile(sample.getAudioFile());

    if (sampleFile->exists())
        audioProcessor.setSample(std::unique_ptr<juce::AudioFormatReader>(mFormatManager.createReaderFor(*sampleFile)), sample.getMidiNote(), sample.getId());
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