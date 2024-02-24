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

class ADSRGraph : public juce::Component,
    private DataModel::Listener,
    private SampleModel::Listener
{
public:
    ADSRGraph(const DataModel&);
    ~ADSRGraph() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:

    void activeSampleChanged(SampleModel&) override;
    void adsrChanged(ADSRParameters) override;
    void updateADSRPoints(ADSRParameters);
    void paintIfNoFileLoaded(juce::Graphics&);
    void paintIfFileLoaded(juce::Graphics&);

    DataModel mDataModel;
    std::unique_ptr<SampleModel> mActiveSample;

    std::vector<juce::Point<float>> ADSRPoints;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ADSRGraph)
};

class ADSRControls : public juce::Component,
                     public juce::Slider::Listener, 
                     private DataModel::Listener, 
                     private SampleModel::Listener
{
public:
    ADSRControls(const DataModel&); 
    ~ADSRControls() override; 

    void paint(juce::Graphics&) override; 
    void resized() override; 
    
private: 
    // Helper methods
    std::vector<juce::Slider*> getSliders();
    std::vector<juce::Label*> getLabels();

    // Data model listener
    void activeSampleChanged(SampleModel&) override;

    // Slider listener 
    void sliderValueChanged(juce::Slider*) override;

    juce::Slider mAttackSlider, mDecaySlider, mSustainSlider, mReleaseSlider;
    juce::Label mSampleLabel, mAttackLabel, mDecayLabel, mSustainLabel, mReleaseLabel;

    DataModel mDataModel;
    std::unique_ptr<SampleModel> mActiveSample;
};

class ADSRView  : public juce::Component
{
public:
    ADSRView(const DataModel&);
    ~ADSRView() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    ADSRGraph mADSRGraph;
    ADSRControls mADSRControls; 
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADSRView)
};


