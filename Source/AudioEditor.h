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
class AudioEditor : public juce::Component,
    public juce::Slider::Listener,
    public juce::Button::Listener, 
    public DataModel::Listener
{
public:
    AudioEditor(const DataModel&, PlaybackPositionOverlay::Providor);
    ~AudioEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void setThumbnailSource(const juce::File& inputSource);
    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;

    void activeSampleChanged(SampleModel&) override; 
    void fileChanged(SampleModel&) override;
    void nameChanged(SampleModel&) override;
    void updateWaveform(SampleModel& modelChanged);
    void updateNameLabel(juce::String);
    void ChannelMenuChanged();

    juce::Rectangle<int> getWaveformWindowBounds();
    juce::Rectangle<int> getSliderWindowBounds();
private:
    AudioDisplay mAudioDisplay;
    PlaybackPositionOverlay mPlaybackOverlay; 
    juce::Slider mVerticalZoom;
    juce::Slider mHorizontalZoom;
    juce::Slider mHorizontalScroll;
    juce::ComboBox mChannelSelect;
    juce::Label mActiveSampleName;
    DataModel dataModel; 

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEditor)
};
