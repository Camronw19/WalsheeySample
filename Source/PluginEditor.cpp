/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WalsheeySampleAudioProcessorEditor::WalsheeySampleAudioProcessorEditor (WalsheeySampleAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), mMainSamplerView(mDataModel)
{
    mFormatManager.registerBasicFormats(); 

    mDataModel.addListener(*this); 
    mDataModel.initializeDefaultModel(8);

    addAndMakeVisible(mMainSamplerView); 

    setResizable(true, true); 
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
    juce::String message("Sample " + juce::String(sample.getId()) + " file changed to ");
    DBG(message);
    
    std::shared_ptr<juce::File> sampleFile(sample.getAudioFile());

    if (sampleFile->exists())
        audioProcessor.setSample(std::unique_ptr<juce::AudioFormatReader>(mFormatManager.createReaderFor(*sampleFile)), sample.getMidiNote());
}