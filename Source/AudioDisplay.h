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
                                private DataModel::Listener,
                                private SampleModel::Listener, 
                                private juce::Timer
                                
{
public: 
    using Providor = std::function<float()>; 
    PlaybackPositionOverlay(const DataModel&, Providor); 

    void paint(juce::Graphics&) override; 
    void timerCallback() override; 

    void activeSampleChanged(SampleModel&) override;

    double timeToXPosition(); 

private: 
    Providor providor; 
    DataModel dataModel;
    std::unique_ptr<SampleModel> activeSample;
};



class AudioDisplay : public juce::Component,
                     public juce::ChangeListener
{
public:
    AudioDisplay();
    ~AudioDisplay() override;

    void paint(juce::Graphics&) override;
    void paintIfNoFileLoaded(juce::Graphics& g, juce::Rectangle<int>& thumbnailBounds);
    void paintIfFileLoaded(juce::Graphics& g, juce::Rectangle<int>& thumbnailBounds);

    void resized() override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void setThumbnailSource(const juce::File& inputSource);
    void thumbnailChanged();
    void setVerticalZoom(float vZoom);
    void setHorizontalZoom(float hZoom);
    void setHorizontalScroll(float hScroll);
    void setShowChannels(bool chan1, bool chan2);
    std::pair<bool, bool> getShowChannels();

private:
    juce::AudioFormatManager mFormatManager;
    juce::AudioThumbnailCache mThumbnailCache;
    juce::AudioThumbnail mThumbnail;

    

    double mVerticalZoom;
    double mHorizontalZoom;
    double mHorizontalScroll;
    bool mShowChan1;
    bool mShowChan2;

    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioDisplay)
};
