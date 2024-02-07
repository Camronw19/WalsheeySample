/*
  ==============================================================================

    AudioDisplay.cpp
    Created: 20 Dec 2023 1:49:50pm
    Author:  camro

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AudioDisplay.h"


PlaybackPositionOverlay::PlaybackPositionOverlay(const DataModel& dm, const VisibleRangeDataModel& vrdm, Providor providorIn)
    :mDataModel(dm), mProvidor(std::move(providorIn)), mVisibleRange(vrdm)
{
    mDataModel.addListener(*this);
    startTimer(16);
}

void PlaybackPositionOverlay::paint(juce::Graphics& g)
{
    if (mVisibleRange.getTotalRange().getLength() > 0)
    {
        g.setColour(juce::Colours::red);
        auto playbackInfo = mProvidor(); 

        if (mActiveSample->getMidiNote() == playbackInfo.second)
        {
            auto xPos = timeToXPosition(playbackInfo.first); 

            juce::Path playhead; 
            playhead.addTriangle(juce::Point<float>(xPos - 10, 0), juce::Point<float>(xPos + 10, 0), juce::Point<float>(xPos, 10));

            g.fillPath(playhead);
            g.drawVerticalLine(xPos, 0, static_cast<float>(getHeight()));
        }
    }
}

void PlaybackPositionOverlay::timerCallback()
{
    repaint(); 
}

double PlaybackPositionOverlay::timeToXPosition(double playbackPos)
{
    auto range = mVisibleRange.getVisibleRange();

    return (playbackPos - range.getStart()) * getWidth()
        / range.getLength();
}

void PlaybackPositionOverlay::activeSampleChanged(SampleModel& modelChanged)
{
    mActiveSample = std::make_unique<SampleModel>(modelChanged.getState());
}

//==============================================================================
AudioDisplay::AudioDisplay(const VisibleRangeDataModel& vrdm)
    : mThumbnailCache(5), mShowChan1(true), mShowChan2(true),
    mThumbnail(1024, mFormatManager, mThumbnailCache), 
    mVisibleRange(vrdm)
{
    mFormatManager.registerBasicFormats();
    mThumbnail.addChangeListener(this);
}

AudioDisplay::~AudioDisplay()
{
}

void AudioDisplay::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    juce::Rectangle<int> thumbnailBounds(bounds);

    if (mThumbnail.getNumChannels() == 0)
        paintIfNoFileLoaded(g, thumbnailBounds);
    else
        paintIfFileLoaded(g, thumbnailBounds);
}

void AudioDisplay::paintIfNoFileLoaded(juce::Graphics& g, juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour(AppColors::componentbackgroundColour);
    g.fillRect(thumbnailBounds);

    g.setColour(AppColors::accentColour);
    g.drawFittedText("No File Loaded", thumbnailBounds, juce::Justification::centred, 1);
}

void AudioDisplay::paintIfFileLoaded(juce::Graphics& g, juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour(AppColors::componentbackgroundColour);
    g.fillRect(thumbnailBounds);

    auto startTime = mVisibleRange.getVisibleRange().getStart();
    auto endTime = mVisibleRange.getVisibleRange().getEnd();

    g.setColour(AppColors::accentColour);

    if (mShowChan1 && !mShowChan2)
    {
        mThumbnail.drawChannel(g, thumbnailBounds, startTime, endTime, 0, 1);
    }
    else if (!mShowChan1 && mShowChan2)
    {
        mThumbnail.drawChannel(g, thumbnailBounds, startTime, endTime, 1, 1);
    }
    else
    {
        mThumbnail.drawChannels(g, thumbnailBounds, startTime, endTime, 1);
    }
}

void AudioDisplay::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &mThumbnail)
        thumbnailChanged();
}

void AudioDisplay::thumbnailChanged()
{
    repaint();
}

void AudioDisplay::setThumbnailSource(const juce::File& inputSource)
{
    mThumbnail.setSource(new juce::FileInputSource(inputSource));
}

void AudioDisplay::visibleRangeChanged(juce::Range<double> visibleRange)
{
    repaint(); 
}

void AudioDisplay::setShowChannels(bool chan1, bool chan2)
{
    mShowChan1 = chan1;
    mShowChan2 = chan2;
    repaint();
}

std::pair<bool, bool> AudioDisplay::getShowChannels()
{
    return std::pair<bool, bool>(mShowChan1, mShowChan2);
}

void AudioDisplay::resized()
{

}

