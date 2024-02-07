/*
  ==============================================================================

    AudioDisplay.h
    Created: 20 Dec 2023 1:49:50pm
    Author:  camro

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Model.h"
#include "UIConfig.h"
//==============================================================================
/*
*/

class PlaybackPositionOverlay : public juce::Component, 
                                private juce::Timer, 
                                private DataModel::Listener
{
public: 
    using Providor = std::function<std::pair<float, float>()>;
    PlaybackPositionOverlay(const DataModel&, const VisibleRangeDataModel&, Providor);

    void paint(juce::Graphics&) override; 

private: 
    void timerCallback() override; 
    double timeToXPosition(double playbackPos);

    //Data Model Listener
    void activeSampleChanged(SampleModel&) override;

    Providor mProvidor; 
    VisibleRangeDataModel mVisibleRange; 
    DataModel mDataModel;

    std::unique_ptr<SampleModel> mActiveSample;
};

class AudioDisplay : public juce::Component,
                     public juce::ChangeListener, 
                     private VisibleRangeDataModel::Listener 
{
public:
    AudioDisplay(const VisibleRangeDataModel&);
    ~AudioDisplay() override;

    void paint(juce::Graphics&) override;

    void resized() override;

    void setThumbnailSource(const juce::File& inputSource);
    void setShowChannels(bool chan1, bool chan2);

private:
    //Callbacks
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void thumbnailChanged();

    //Visible Range Listener
    virtual void visibleRangeChanged(juce::Range<double>) override;

    //Helper methods
    void paintIfNoFileLoaded(juce::Graphics& g, juce::Rectangle<int>& thumbnailBounds);
    void paintIfFileLoaded(juce::Graphics& g, juce::Rectangle<int>& thumbnailBounds);
    std::pair<bool, bool> getShowChannels();

    juce::AudioFormatManager mFormatManager;
    juce::AudioThumbnailCache mThumbnailCache;
    juce::AudioThumbnail mThumbnail;

    VisibleRangeDataModel mVisibleRange; 

    bool mShowChan1;
    bool mShowChan2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioDisplay)
};
