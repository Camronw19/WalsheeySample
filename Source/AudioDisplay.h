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
                                private VisibleRangeDataModel::Listener
{
public: 
    using Providor = std::function<float()>; 
    PlaybackPositionOverlay(const VisibleRangeDataModel&, Providor);

    void paint(juce::Graphics&) override; 
    void timerCallback() override; 

    virtual void visibleRangeChanged(juce::Range<double>) override; 

    double timeToXPosition(); 

private: 
    Providor providor; 
    VisibleRangeDataModel visibleRange; 
};

class AudioDisplay : public juce::Component,
                     public juce::ChangeListener, 
                     private VisibleRangeDataModel::Listener 
{
public:
    AudioDisplay(const VisibleRangeDataModel&);
    ~AudioDisplay() override;

    void paint(juce::Graphics&) override;
    void paintIfNoFileLoaded(juce::Graphics& g, juce::Rectangle<int>& thumbnailBounds);
    void paintIfFileLoaded(juce::Graphics& g, juce::Rectangle<int>& thumbnailBounds);

    void resized() override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void setThumbnailSource(const juce::File& inputSource);
    void thumbnailChanged();

    void setShowChannels(bool chan1, bool chan2);
    std::pair<bool, bool> getShowChannels();

    //Visible Range Listener
    virtual void visibleRangeChanged(juce::Range<double>) override;

private:
    juce::AudioFormatManager mFormatManager;
    juce::AudioThumbnailCache mThumbnailCache;
    juce::AudioThumbnail mThumbnail;

    VisibleRangeDataModel mVisibleRange; 

    bool mShowChan1;
    bool mShowChan2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioDisplay)
};
