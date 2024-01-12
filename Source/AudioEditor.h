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
                    private DataModel::Listener, 
                    private SampleModel::Listener
{
public:
    AudioEditor(const DataModel&, PlaybackPositionOverlay::Providor);
    ~AudioEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void setThumbnailSource(const juce::File& inputSource);
private:
    //Component Listener
    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;

    //Data Model Listener
    void activeSampleChanged(SampleModel&) override; 

    //Sample Model Listener
    void fileChanged(std::shared_ptr<juce::File>) override;
    void nameChanged(juce::String) override;

    //Helper Methods
    void ChannelMenuChanged();
    void updateWaveform(const std::shared_ptr<juce::File>& file);
    void updateNameLabel(juce::String);
    void updateVisibleRange(const juce::Range<double>&);
    void updateScrollRange(); 
    void updateZoomRange(); 

    juce::Rectangle<int> getWaveformWindowBounds();
    juce::Rectangle<int> getSliderWindowBounds();

    DataModel dataModel; 
    VisibleRangeDataModel visibleRange; 
    std::unique_ptr<SampleModel> activeSample; 

    AudioDisplay mAudioDisplay;
    PlaybackPositionOverlay mPlaybackOverlay; 
    juce::Slider mVerticalZoom;
    juce::Slider mHorizontalZoom;
    juce::Slider mHorizontalScroll;
    juce::ComboBox mChannelSelect;
    juce::Label mActiveSampleName;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEditor)
};
