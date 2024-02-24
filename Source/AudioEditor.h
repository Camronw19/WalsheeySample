/*
  ==============================================================================

    AudioEditor.h
    Created: 19 Dec 2023 2:44:08pm
    Author:  camro

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "AudioDisplay.h"
#include "Model.h"

//==============================================================================
/*
*/
class SampleViewControls : public juce::Component,
                           public juce::Slider::Listener, 
                           private DataModel::Listener,
                           private SampleModel::Listener
{
public:
    SampleViewControls(const DataModel&, const VisibleRangeDataModel&);
    ~SampleViewControls(); 

    void paint(juce::Graphics&) override;
    void resized() override;
    void lookAndFeelChanged() override;

private:
    void ChannelMenuChanged();
    void updateScrollRange();
    void updateZoomRange();

    //Component Listener
    void sliderValueChanged(juce::Slider* slider) override;
    void activeSampleChanged(SampleModel&) override;

    juce::Slider mVerticalZoom, mHorizontalZoom, mHorizontalScroll;
    juce::Label mVZoomLabel, mHZoomLabel, mHScroll; 
    juce::ComboBox mChannelSelect;
    juce::Label mActiveSampleName;

    DataModel mDataModel;
    VisibleRangeDataModel mVisibleRange;
    std::unique_ptr<SampleModel> mActiveSample;
};



class AudioEditor : public juce::Component,
                    private DataModel::Listener, 
                    private SampleModel::Listener
{
public:
    AudioEditor(const DataModel&, PlaybackPositionOverlay::Providor);
    ~AudioEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void setThumbnailSource(const juce::File& inputSource);
    void lookAndFeelChanged() override;

private:


    //Data Model Listener
    void activeSampleChanged(SampleModel&) override; 

    //Sample Model Listener
    void fileChanged(std::shared_ptr<juce::File>) override;

    //Helper Methods
    void updateWaveform(const std::shared_ptr<juce::File>& file);
    void updateVisibleRange(const juce::Range<double>&);

    void initializeComponents(); 


    DataModel mDataModel; 
    VisibleRangeDataModel mVisibleRange; 
    std::unique_ptr<SampleModel> mActiveSample; 

    AudioDisplay mAudioDisplay;
    PlaybackPositionOverlay mPlaybackOverlay; 
    SampleViewControls mSampleViewControls; 


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEditor)
};


