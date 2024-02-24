/*
  ==============================================================================

    PitchView.h
    Created: 9 Feb 2024 1:48:56pm
    Author:  camro

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Model.h"
#include "UIConfig.h"

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
    void lookAndFeelChanged() override; 

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
