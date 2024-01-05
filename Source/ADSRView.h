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

//==============================================================================
/*
*/
class ADSRView  : public juce::Component, 
                  public DataModel::Listener, 
                  public juce::Slider::Listener, 
                  public SampleModel::Listener
{
public:
    ADSRView(const DataModel& dataModel);
    ~ADSRView() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void sliderValueChanged(juce::Slider* slider) override;
    void activeSampleChanged(SampleModel&) override;

    void nameChanged(juce::String) override; 

    std::vector<juce::Slider*> getSliders(); 

private:
    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider; 
    juce::Label activeSampleName; 
    
    DataModel dataModel;
    std::unique_ptr<SampleModel> activeSample; 

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADSRView)
};
