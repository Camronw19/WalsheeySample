/*
  ==============================================================================

    ADSRView.h
    Created: 5 Jan 2024 9:40:11am
    Author:  Camron

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include "Model.h"
#include "UIConfig.h"
//==============================================================================
/*
*/
class ADSRView  : public juce::Component, 
                  public juce::Slider::Listener, 
                  private DataModel::Listener, 
                  private SampleModel::Listener
{
public:
    ADSRView(const DataModel&);
    ~ADSRView() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // Sample model listener
    void nameChanged(juce::String) override; 

    // Data model listener
    void activeSampleChanged(SampleModel&) override;

    // Slider listener 
    void sliderValueChanged(juce::Slider*) override;

    // Helper methods
    std::vector<juce::Slider*> getSliders();
    std::vector<juce::Label*> getLabels();

    juce::Slider mAttackSlider, mDecaySlider, mSustainSlider, mReleaseSlider; 
    juce::Label mSampleLabel, mAttackLabel, mDecayLabel, mSustainLabel, mReleaseLabel; 
    
    DataModel mDataModel;
    std::unique_ptr<SampleModel> mActiveSample; 

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADSRView)
};


class PitchView : public juce::Component,
                  public juce::Slider::Listener,
                  private DataModel::Listener,
                  private SampleModel::Listener
{
public:
    PitchView(const DataModel&);
    ~PitchView() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // Sample model listener
    void nameChanged(juce::String) override;

    // Data model listener
    void activeSampleChanged(SampleModel&) override;

    // Slider listener 
    void sliderValueChanged(juce::Slider*) override;


    juce::Slider mPitchSlider;
    juce::Label mPitchLabel;

    DataModel mDataModel;
    std::unique_ptr<SampleModel> mActiveSample;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchView)
};
