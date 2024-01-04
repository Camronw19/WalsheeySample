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
    AudioEditor(const DataModel&);
    ~AudioEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void setThumbnailSource(const juce::File& inputSource);
    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;

    void activeSampleChanged(SampleModel&) override; 

private:
    AudioDisplay mAudioDisplay;
    juce::Slider mVerticalZoom;
    juce::Slider mHorisontalZoom;
    juce::Slider mHorisontalScroll;
    juce::ToggleButton mChan1Toggle;
    juce::ToggleButton mChan2Toggle;

    DataModel dataModel; 
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEditor)
};
