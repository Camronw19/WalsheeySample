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
    :mDataModel(dm),  
    mPlaybackOverlay(mDataModel, mVisibleRange, std::move(providor)), mAudioDisplay(mVisibleRange), 
    mSampleViewControls(dm, mVisibleRange)
{
    mDataModel.addListener(*this);

    addAndMakeVisible(mAudioDisplay);
    addAndMakeVisible(mPlaybackOverlay);
    addAndMakeVisible(mSampleViewControls);

    initializeComponents(); 
}

AudioEditor::~AudioEditor()
{
}

void AudioEditor::paint(juce::Graphics& g)
{
    g.setColour(getLookAndFeel().findColour(AppColors::ColourIds::BackgroundDefault));
    g.fillAll();
}


void AudioEditor::resized()
{
    auto bounds = getLocalBounds(); 
    auto audioDisplayBounds = bounds.removeFromTop(bounds.getHeight() * .7); 
    mAudioDisplay.setBounds(audioDisplayBounds);
    mPlaybackOverlay.setBounds(audioDisplayBounds.reduced(spacing::padding3));
    mSampleViewControls.setBounds(bounds); 
}


void AudioEditor::setThumbnailSource(const juce::File& inputSource)
{
    mAudioDisplay.setThumbnailSource(inputSource);
}


void AudioEditor::activeSampleChanged(SampleModel& modelChanged)
{
    updateWaveform(modelChanged.getAudioFile());
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


void AudioEditor::initializeComponents()
{
    
}

void AudioEditor::lookAndFeelChanged()
{
    if (!dynamic_cast<juce::LookAndFeel_V4*>(&getLookAndFeel()))
    {
       
    }
}

//================================================================

SampleViewControls::SampleViewControls(const DataModel& dm, const VisibleRangeDataModel& vr)
    :mDataModel(dm), mVisibleRange(vr)
{
    mDataModel.addListener(*this);

    mActiveSampleName.setJustificationType(juce::Justification::centred);

    mHorizontalZoom.addListener(this);
    mHorizontalZoom.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mHorizontalZoom.setValue(0., juce::dontSendNotification);
    mHorizontalZoom.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(mHorizontalZoom);

    mHorizontalScroll.addListener(this);
    mHorizontalScroll.setSliderStyle(juce::Slider::LinearHorizontal);
    mHorizontalScroll.setValue(0.0, juce::dontSendNotification);
    mHorizontalScroll.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(mHorizontalScroll);

    addAndMakeVisible(mActiveSampleName);

    mHZoomLabel.setText("Zoom", juce::dontSendNotification);
    addAndMakeVisible(mHZoomLabel); 

    mHScroll.setText("Scroll", juce::dontSendNotification);
    mHScroll.setJustificationType(juce::Justification::centred); 
    addAndMakeVisible(mHScroll); 



    ////ChannelSelect Menu
    //addAndMakeVisible(mChannelSelect);
    //mChannelSelect.addItem("Both", 1);
    //mChannelSelect.addItem("Channel 1", 2);
    //mChannelSelect.addItem("Channel 2", 3);

    //mChannelSelect.onChange = [this] { ChannelMenuChanged(); };
    //mChannelSelect.setSelectedId(1);

    ////Zoom sliders
    //mVerticalZoom.addListener(this);
    //mVerticalZoom.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    //mVerticalZoom.setValue(0.0, juce::dontSendNotification);
    //mVerticalZoom.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    //addAndMakeVisible(mVerticalZoom);
    //mVZoomLabel.setText("Vertical Zoom", juce::dontSendNotification);
}

SampleViewControls::~SampleViewControls()
{
    mDataModel.removeListener(*this); 
}

void SampleViewControls::paint(juce::Graphics& g)
{
    g.setColour(AppColors::Dark::divider);
    auto rectBounds = getLocalBounds().toFloat();
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(spacing::padding3, 0), spacing::padding4);
}

void SampleViewControls::updateScrollRange()
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

void SampleViewControls::updateZoomRange()
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

void SampleViewControls::ChannelMenuChanged()
{
    switch (mChannelSelect.getSelectedId())
    {
    case 1:   break;

    case 2:   break;

    case 3:   break;
    
    default:  break;
    }
}

void SampleViewControls::resized()
{
    //Slider Layout
    juce::FlexBox sliderFlexBox;
    sliderFlexBox.items.add(juce::FlexItem(mHorizontalScroll).withFlex(1.0, 1.0));
    sliderFlexBox.items.add(juce::FlexItem().withFlex(1.0, 1.0));
    sliderFlexBox.items.add(juce::FlexItem(mActiveSampleName).withFlex(1.0, 1.0));
    sliderFlexBox.items.add(juce::FlexItem().withFlex(1.0, 1.0));
    sliderFlexBox.items.add(juce::FlexItem(mHorizontalZoom).withFlex(1.0, 1.0));

    sliderFlexBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    sliderFlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
    sliderFlexBox.alignContent = juce::FlexBox::AlignContent::center;

    sliderFlexBox.performLayout(getLocalBounds().reduced(spacing::padding3));
}

void SampleViewControls::lookAndFeelChanged()
{
    if (!dynamic_cast<juce::LookAndFeel_V4*>(&getLookAndFeel()))
    {
        mActiveSampleName.setColour(juce::Label::textColourId, getLookAndFeel().findColour(AppColors::ColourIds::TextPrimary));
    }
}

void SampleViewControls::sliderValueChanged(juce::Slider* slider)
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
        mActiveSample->setHZoom(slider->getValue()); 
    }
    else if (slider == &mHorizontalScroll)
    {
        updateScrollRange();
        mActiveSample->setHScroll(slider->getValue());
    }
}

void SampleViewControls::activeSampleChanged(SampleModel& modelChanged)
{
    mActiveSampleName.setText(modelChanged.getName(), juce::dontSendNotification); 
    mHorizontalZoom.setValue(modelChanged.getHZoom());
    mHorizontalScroll.setValue(modelChanged.getHScroll());

    if (mActiveSample != nullptr)
        mActiveSample->removeListener(*this);

    mActiveSample = std::make_unique<SampleModel>(modelChanged.getState());

    if (mActiveSample != nullptr)
        mActiveSample->addListener(*this);
}