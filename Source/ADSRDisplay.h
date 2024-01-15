/*
  ==============================================================================

    ADSRDisplay.h
    Created: 12 Jan 2024 10:46:51am
    Author:  liamw

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
class ADSRDisplay  : public juce::Component, 
                     private DataModel::Listener,
                     private SampleModel::Listener
{
public:
    ADSRDisplay(const DataModel&);
    ~ADSRDisplay() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:

    void activeSampleChanged(SampleModel&) override;
    void adsrChanged(ADSRParameters) override;
    void updateADSRPoints(ADSRParameters);

    DataModel mDataModel;
    std::unique_ptr<SampleModel> mActiveSample;

    std::vector<juce::Point<float>> ADSRPoints;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADSRDisplay)
};
