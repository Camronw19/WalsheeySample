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
    :mDataModel(dm),  mPlaybackOverlay(mDataModel, mVisibleRange, std::move(providor)), mAudioDisplay(mVisibleRange)
{
    mDataModel.addListener(*this);

    addAndMakeVisible(mAudioDisplay);
    addAndMakeVisible(mPlaybackOverlay);
    addAndMakeVisible(mActiveSampleName);

    initializeComponents(); 
}

AudioEditor::~AudioEditor()
{
}

void AudioEditor::paint(juce::Graphics& g)
{
    g.setColour(AppColors::silderWindowColour);
    g.fillRect(getSliderWindowBounds());
  
    g.setColour(AppColors::silderWindowAccentColour);
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

    mChannelSelect.setColour(juce::ComboBox::ColourIds::backgroundColourId, AppColors::silderWindowColour);
    mChannelSelect.setColour(juce::ComboBox::ColourIds::outlineColourId, AppColors::silderWindowAccentColour);
    mChannelSelect.setColour(juce::ComboBox::ColourIds::focusedOutlineColourId, AppColors::silderWindowAccentColour);
    mChannelSelect.setColour(juce::ComboBox::ColourIds::textColourId, AppColors::labelColour);


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
    }
    else if (slider == &mHorizontalZoom)
    {
        updateZoomRange(); 
        updateScrollRange(); 
    }
    else if (slider == &mHorizontalScroll)
    {
        updateScrollRange();
    }
}

void AudioEditor::updateScrollRange()
{
    //Normalize the scroll value 
    float sliderValue = static_cast<float>(mHorizontalScroll.getValue());
    float normalizedScrollValue = juce::jmap(sliderValue, static_cast<float>(mHorizontalScroll.getMaximum()), static_cast<float>(mHorizontalScroll.getMinimum()), 1.0f, 0.0f);

    //Get total range and current visible range
    auto totalRange = mVisibleRange.getTotalRange();
    auto currentVisibleRange = mVisibleRange.getVisibleRange();

    //Compute new visible range based on normalized scroll value
    double visibleLength = currentVisibleRange.getLength();
    double maxStart = totalRange.getEnd() - visibleLength;
    double start = normalizedScrollValue * maxStart;
    double end = start + visibleLength;

    mVisibleRange.setVisibleRange(juce::Range<double>(start, end));
}

void AudioEditor::updateZoomRange()
{
    //Normalize the zoom slider value
    float sliderValue = static_cast<float>(mHorizontalZoom.getValue());
    float normalizedZoomValue = juce::jmap(sliderValue, static_cast<float>(mHorizontalZoom.getMaximum()), static_cast<float>(mHorizontalZoom.getMinimum()), .97f, 0.0f);

    //Get total range and current visible range
    auto totalRange = mVisibleRange.getTotalRange();
    auto currentVisibleRange = mVisibleRange.getVisibleRange();

    //Compute new visible range based off normalized slider value
    double visibleLength = totalRange.getLength() * (1 - normalizedZoomValue);
    double start = currentVisibleRange.getStart();
    double end = start + visibleLength;

    // Ensure the new end does not exceed the total range
    if (end > totalRange.getEnd())
    {
        end = totalRange.getEnd();
        start = end - visibleLength;
    }

    mVisibleRange.setVisibleRange(juce::Range<double>(start, end));
}


void AudioEditor::buttonClicked(juce::Button* button)
{

}

void AudioEditor::activeSampleChanged(SampleModel& modelChanged)
{
    updateWaveform(modelChanged.getAudioFile());
    updateNameLabel(modelChanged.getName());
    updateVisibleRange(modelChanged.getTotalRange()); 

    if (mActiveSample != nullptr)
        mActiveSample->removeListener(*this);

    mActiveSample = std::make_unique<SampleModel>(modelChanged.getState());

    if (mActiveSample != nullptr)
        mActiveSample->addListener(*this);
}

void AudioEditor::fileChanged(std::shared_ptr<juce::File> file)
{
    updateWaveform(file); 
}

void AudioEditor::nameChanged(juce::String name)
{
    updateNameLabel(name);
}

void AudioEditor::updateWaveform(const std::shared_ptr<juce::File>& file)
{
    if (file != nullptr)
        setThumbnailSource(*file);
    else
        setThumbnailSource(juce::File()); 
}

void AudioEditor::updateVisibleRange(const juce::Range<double>& newRange)
{
    mVisibleRange.setTotalRange(newRange); 
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

void AudioEditor::initializeComponents()
{
    mActiveSampleName.setJustificationType(juce::Justification::centred);
    mActiveSampleName.setColour(juce::Label::textColourId, AppColors::componentbackgroundColour);
    

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