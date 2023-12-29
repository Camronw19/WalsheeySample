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
    
    mDataModel.addListener(*this); 
    mDataModel.initializeDefualtModel(8); 

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
    auto bounds = getLocalBounds(); 
    mMainSamplerView.setBounds(bounds.reduced(20)); 
}


void WalsheeySampleAudioProcessorEditor::nameChanged(SampleModel& sample)
{
    DBG("EDITOR NAME CHAGED METHOD");
    juce::String message("Sample " + juce::String(sample.getId()) + " name changed to " + sample.getName()); 
    DBG(message); 
    //Change name for corresponding sample in the processor
}

void WalsheeySampleAudioProcessorEditor::fileChanged(SampleModel& sample)
{
    DBG("FILE CHAGED METHOD");
    juce::String message("Sample " + juce::String(sample.getId()) + " file changed to " );
    DBG(message);
    //Change name for corresponding sample in the processor
}