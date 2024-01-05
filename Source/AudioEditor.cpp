/*
  ==============================================================================

    AudioEditor.cpp
    Created: 19 Dec 2023 2:44:08pm
    Author:  camro

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AudioEditor.h"

//==============================================================================
AudioEditor::AudioEditor(const DataModel& dm)
    :dataModel(dm)
{

    dataModel.addListener(*this); 

    addAndMakeVisible(mAudioDisplay);

    //Zoom sliders
    mVerticalZoom.addListener(this);
    mVerticalZoom.setSliderStyle(juce::Slider::LinearBarVertical);
    mVerticalZoom.setValue(0.0, juce::dontSendNotification);
    mVerticalZoom.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(mVerticalZoom);

    mHorisontalZoom.addListener(this);
    mHorisontalZoom.setSliderStyle(juce::Slider::LinearBarVertical);
    mHorisontalZoom.setValue(0., juce::dontSendNotification);
    mHorisontalZoom.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(mHorisontalZoom);

    //Scroll sliders
    mHorisontalScroll.addListener(this);
    mHorisontalScroll.setSliderStyle(juce::Slider::LinearBar);
    mHorisontalScroll.setValue(0.0, juce::dontSendNotification);
    mHorisontalScroll.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(mHorisontalScroll);

    //Show channel toggle buttons
    std::pair<bool, bool> showChannel = mAudioDisplay.getShowChannels();
    addAndMakeVisible(mChan1Toggle);
    mChan1Toggle.addListener(this);
    mChan1Toggle.setToggleState(showChannel.first, juce::NotificationType::dontSendNotification);

    addAndMakeVisible(mChan2Toggle);
    mChan2Toggle.addListener(this);
    mChan2Toggle.setToggleState(showChannel.second, juce::NotificationType::dontSendNotification);
}

AudioEditor::~AudioEditor()
{
}

void AudioEditor::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::burlywood);
    g.drawRect(getLocalBounds().toFloat());
}


void AudioEditor::resized()
{
    auto bounds = getLocalBounds();
    mAudioDisplay.setBounds(bounds.reduced(50));
    mVerticalZoom.setBounds(bounds.removeFromRight(20));
    mHorisontalZoom.setBounds(bounds.removeFromRight(20));
    mHorisontalScroll.setBounds(bounds.removeFromBottom(20));

    auto toggleBounds = bounds.removeFromLeft(50);
    mChan1Toggle.setBounds(toggleBounds.removeFromTop(toggleBounds.getHeight() / 2));
    mChan2Toggle.setBounds(toggleBounds);
}


void AudioEditor::setThumbnailSource(const juce::File& inputSource)
{
    mAudioDisplay.setThumbnailSource(inputSource);
}

void AudioEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &mVerticalZoom)
    {
        float sliderValue = static_cast<float>(mVerticalZoom.getValue());
        float normalizedSliderValue = juce::jmap(sliderValue, static_cast<float>(mVerticalZoom.getMaximum()), static_cast<float>(mVerticalZoom.getMinimum()), 1.0f, 0.1f);
        mAudioDisplay.setVerticalZoom(normalizedSliderValue);
    }
    else if (slider == &mHorisontalZoom)
    {
        float sliderValue = static_cast<float>(mHorisontalZoom.getValue());
        float normalizedSliderValue = juce::jmap(sliderValue, static_cast<float>(mHorisontalZoom.getMaximum()), static_cast<float>(mHorisontalZoom.getMinimum()), .97f, 0.0f);
        mAudioDisplay.setHorisontalZoom(normalizedSliderValue);
    }
    else if (slider == &mHorisontalScroll)
    {
        float sliderValue = static_cast<float>(mHorisontalScroll.getValue());
        float normalizedSliderValue = juce::jmap(sliderValue, static_cast<float>(mHorisontalScroll.getMaximum()), static_cast<float>(mHorisontalScroll.getMinimum()), 1.0f, 0.0f);
        mAudioDisplay.setHorisontalScroll(normalizedSliderValue);
        DBG(normalizedSliderValue);
    }
}

void AudioEditor::buttonClicked(juce::Button* button)
{
    if (button == &mChan1Toggle || button == &mChan2Toggle)
    {
        if (!mChan1Toggle.getToggleState() && !mChan2Toggle.getToggleState())
        {
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                "Alert",
                "At least one channel must be selected!",
                "OK");

            if (button == &mChan1Toggle)
            {
                mChan1Toggle.setToggleState(true, juce::dontSendNotification);
            }
            else if (button == &mChan2Toggle)
            {
                mChan2Toggle.setToggleState(true, juce::dontSendNotification);

            }
        }

        mAudioDisplay.setShowChannels(mChan1Toggle.getToggleState(), mChan2Toggle.getToggleState());
    }
}

void AudioEditor::activeSampleChanged(SampleModel& modelChanged)
{
    std::shared_ptr<juce::File> file(modelChanged.getAudioFile()); 

    if (file != nullptr)
        setThumbnailSource(*file); 
    else
        setThumbnailSource(juce::File()); 
}
