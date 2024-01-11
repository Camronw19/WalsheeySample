/*
  ==============================================================================

    AudioEditor.cpp
    Created: 19 Dec 2023 2:44:08pm
    Author:  camro

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AudioEditor.h"
#include "UIConfig.h"

//==============================================================================
AudioEditor::AudioEditor(const DataModel& dm, PlaybackPositionOverlay::Providor providor)
    :dataModel(dm), mPlaybackOverlay(dm, std::move(providor))
{
    dataModel.addListener(*this); 

    addAndMakeVisible(mAudioDisplay);
    addAndMakeVisible(mPlaybackOverlay); 

    addAndMakeVisible(mActiveSampleName);
    mActiveSampleName.setJustificationType(juce::Justification::centred);
    mActiveSampleName.setColour(juce::Label::textColourId, AppColors::backgroundColour);
    mActiveSampleName.setColour(juce::Label::outlineColourId, AppColors::backgroundColour);

    //Zoom sliders
    mVerticalZoom.addListener(this);
    mVerticalZoom.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mVerticalZoom.setValue(0.0, juce::dontSendNotification);
    mVerticalZoom.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    mVerticalZoom.setColour(juce::Slider::thumbColourId, AppColors::accentColour);
    addAndMakeVisible(mVerticalZoom);

    mHorizontalZoom.addListener(this);
    mHorizontalZoom.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mHorizontalZoom.setValue(0., juce::dontSendNotification);
    mHorizontalZoom.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    mHorizontalZoom.setColour(juce::Slider::thumbColourId, AppColors::accentColour);
    addAndMakeVisible(mHorizontalZoom);

    //Scroll sliders
    mHorizontalScroll.addListener(this);
    mHorizontalScroll.setSliderStyle(juce::Slider::LinearHorizontal);
    mHorizontalScroll.setValue(0.0, juce::dontSendNotification);
    mHorizontalScroll.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    mHorizontalScroll.setColour(juce::Slider::rotarySliderFillColourId, AppColors::accentColour);
    mHorizontalScroll.setColour(juce::Slider::thumbColourId, AppColors::accentColour);
    addAndMakeVisible(mHorizontalScroll);

    //ChannelSelect Menu
    addAndMakeVisible(mChannelSelect);
    mChannelSelect.addItem("Both", 1);
    mChannelSelect.addItem("Channel 1", 2);
    mChannelSelect.addItem("Channel 2", 3);
   
    mChannelSelect.onChange = [this] { ChannelMenuChanged(); };
    mChannelSelect.setSelectedId(1);
}

AudioEditor::~AudioEditor()
{
}

void AudioEditor::paint(juce::Graphics& g)
{
    g.setColour(AppColors::backgroundColour);
    g.drawRect(getLocalBounds().toFloat());
  
    float linePositionY = getLocalBounds().getHeight() * 0.75;
    g.drawLine(getLocalBounds().getX(), linePositionY, getLocalBounds().getRight(), linePositionY);
}


void AudioEditor::resized()
{
    auto audioDisplayBounds = getWaveformWindowBounds(); 
    mAudioDisplay.setBounds(audioDisplayBounds);
    mPlaybackOverlay.setBounds(audioDisplayBounds); 

    //Slider Layout
    juce::FlexBox sliderFlexBox;
    sliderFlexBox.items.add(juce::FlexItem(mHorizontalScroll).withFlex(1.0, 1.0));
    sliderFlexBox.items.add(juce::FlexItem(mVerticalZoom).withFlex(1.0,  1.0));
    sliderFlexBox.items.add(juce::FlexItem(mActiveSampleName).withFlex(1.0, 1.0));
    sliderFlexBox.items.add(juce::FlexItem(mHorizontalZoom).withFlex(1.0, 1.0));
    sliderFlexBox.items.add(juce::FlexItem(mChannelSelect).withFlex(1.0, 1.0));

    sliderFlexBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    sliderFlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
    sliderFlexBox.alignContent = juce::FlexBox::AlignContent::center;
   
    sliderFlexBox.performLayout(getSliderWindowBounds());
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
    else if (slider == &mHorizontalZoom)
    {
        float sliderValue = static_cast<float>(mHorizontalZoom.getValue());
        float normalizedSliderValue = juce::jmap(sliderValue, static_cast<float>(mHorizontalZoom.getMaximum()), static_cast<float>(mHorizontalZoom.getMinimum()), .97f, 0.0f);
        mAudioDisplay.setHorizontalZoom(normalizedSliderValue);
    }
    else if (slider == &mHorizontalScroll)
    {
        float sliderValue = static_cast<float>(mHorizontalScroll.getValue());
        float normalizedSliderValue = juce::jmap(sliderValue, static_cast<float>(mHorizontalScroll.getMaximum()), static_cast<float>(mHorizontalScroll.getMinimum()), 1.0f, 0.0f);
        mAudioDisplay.setHorizontalScroll(normalizedSliderValue);
        DBG(normalizedSliderValue);
    }
}

void AudioEditor::buttonClicked(juce::Button* button)
{

}

void AudioEditor::activeSampleChanged(SampleModel& modelChanged)
{
    updateWaveform(modelChanged);
    updateNameLabel(modelChanged.getName());
}

void AudioEditor::fileChanged(SampleModel& modelChanged)
{
    updateWaveform(modelChanged); 
    updateNameLabel(modelChanged.getName());
}

void AudioEditor::nameChanged(SampleModel& modelChanged)
{
    updateNameLabel(modelChanged.getName());
}

void AudioEditor::updateWaveform(SampleModel& modelChanged)
{
    auto file = modelChanged.getAudioFile();
    if (file != nullptr)
        setThumbnailSource(*file);
    else
        setThumbnailSource(juce::File()); 
}

void AudioEditor::updateNameLabel(juce::String name)
{
    mActiveSampleName.setText(name, juce::dontSendNotification);
}

void AudioEditor::ChannelMenuChanged()
{
    switch (mChannelSelect.getSelectedId())
    {
    case 1:  mAudioDisplay.setShowChannels(true,true);  break;

    case 2:  mAudioDisplay.setShowChannels(true,false); break;

    case 3:  mAudioDisplay.setShowChannels(false,true); break;
    
    default: break;
    }
}

juce::Rectangle<int> AudioEditor::getWaveformWindowBounds()
{
    auto bounds = getLocalBounds();
    juce::Rectangle<int> waveformBounds(bounds.getTopLeft().getX(), bounds.getTopLeft().getY(), bounds.getWidth(), bounds.getHeight() * .75);
    waveformBounds = waveformBounds.reduced(10);

    return waveformBounds;
}

juce::Rectangle<int> AudioEditor::getSliderWindowBounds()
{
    auto bounds = getLocalBounds();
    int sliderWindowHeight = bounds.getHeight() / 4;
    juce::Rectangle<int> sliderWindowBounds(bounds.getX(), bounds.getBottom() - sliderWindowHeight, bounds.getWidth(), sliderWindowHeight);

    return sliderWindowBounds;
}